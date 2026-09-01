# FollyTest is a web app for browsing FollyChess SPRT test results.
#
# Copyright (C) 2025-2026 Aryan Naraghi <aryan.naraghi@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

"""Storage backends for the FollyTest API.

The Firestore layout is:

    runs/{id}:           run metadata, status, and rolling stats.
    runs/{id}/games/{n}: one document per game, keyed by the zero-padded
                         game number so that batch retries are idempotent.

The in-memory backend exists for local development and tests, selected with
STORE=memory.
"""

import datetime
import secrets


class MemoryStore:
    def __init__(self):
        self.runs = {}
        self.games = {}

    def create_run(self, metadata: dict) -> str:
        run_id = secrets.token_urlsafe(8)
        self.runs[run_id] = {
            **metadata,
            "created_at": datetime.datetime.now(datetime.UTC).isoformat(),
            "status": "running",
            "games_count": 0,
            "stats": {},
        }
        self.games[run_id] = {}
        return run_id

    def run_exists(self, run_id: str) -> bool:
        return run_id in self.runs

    def write_games(self, run_id: str, games: list[dict],
                    stats: dict | None) -> None:
        for game in games:
            self.games[run_id][f"{game['n']:04d}"] = game

        run = self.runs[run_id]
        run["games_count"] = len(self.games[run_id])
        if stats:
            run["stats"] = stats

    def update_run(self, run_id: str, fields: dict) -> None:
        self.runs[run_id].update(fields)


class FirestoreStore:
    def __init__(self):
        # Imported lazily so that the memory backend works without Google
        # Cloud credentials or the client library.
        from google.cloud import firestore

        self.client = firestore.Client()

    def create_run(self, metadata: dict) -> str:
        from google.cloud import firestore

        ref = self.client.collection("runs").document()
        ref.set({
            **metadata,
            "created_at": firestore.SERVER_TIMESTAMP,
            "status": "running",
            "games_count": 0,
            "stats": {},
        })
        return ref.id

    def run_exists(self, run_id: str) -> bool:
        return self.client.collection("runs").document(run_id).get().exists

    def write_games(self, run_id: str, games: list[dict],
                    stats: dict | None) -> None:
        from google.cloud import firestore

        run_ref = self.client.collection("runs").document(run_id)

        batch = self.client.batch()
        for game in games:
            batch.set(run_ref.collection("games").document(f"{game['n']:04d}"),
                      game)
        batch.commit()

        aggregate = self.client.collection("runs").document(run_id).collection(
            "games").count().get()
        updates = {"games_count": int(aggregate[0][0].value)}
        if stats:
            updates["stats"] = stats
        run_ref.update(updates)

    def update_run(self, run_id: str, fields: dict) -> None:
        self.client.collection("runs").document(run_id).update(fields)
