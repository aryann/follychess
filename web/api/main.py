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

"""The FollyTest ingestion API.

This is a write-only API: the frontend reads Firestore directly, so the only
endpoints here are the ones scripts/sprt.py uses to record runs. All
endpoints require the API key in the X-Api-Key header.
"""

import os

import fastapi
import pydantic

import store

app = fastapi.FastAPI(title="FollyTest API")

_store = None


def get_store():
    global _store
    if _store is None:
        if os.environ.get("STORE") == "memory":
            _store = store.MemoryStore()
        else:
            _store = store.FirestoreStore()
    return _store


def require_api_key(x_api_key: str = fastapi.Header(default="")) -> None:
    expected = os.environ.get("API_KEY")
    if not expected:
        raise fastapi.HTTPException(500, "API_KEY is not configured.")
    if x_api_key != expected:
        raise fastapi.HTTPException(401, "Invalid API key.")


class CreateRunRequest(pydantic.BaseModel):
    mode: str
    elo0: float
    elo1: float
    nodes: int
    book: str
    candidate_commit: str
    candidate_dirty: bool
    baseline_ref: str
    baseline_commit: str
    note: str = ""


class Game(pydantic.BaseModel):
    n: int
    white: str
    black: str
    result: str
    termination: str = ""
    pgn: str


class BatchRequest(pydantic.BaseModel):
    games: list[Game] = []
    stats: dict | None = None


class UpdateRunRequest(pydantic.BaseModel):
    status: str | None = None
    stats: dict | None = None

    @pydantic.field_validator("status")
    @classmethod
    def validate_status(cls, value):
        allowed = {"running", "passed", "failed", "inconclusive",
                   "interrupted"}
        if value is not None and value not in allowed:
            raise ValueError(f"status must be one of {sorted(allowed)}")
        return value


@app.get("/healthz")
def healthz():
    return {"ok": True}


@app.post("/runs", dependencies=[fastapi.Depends(require_api_key)])
def create_run(request: CreateRunRequest):
    run_id = get_store().create_run(request.model_dump())
    return {"id": run_id}


@app.post("/runs/{run_id}/batch",
          dependencies=[fastapi.Depends(require_api_key)])
def post_batch(run_id: str, request: BatchRequest):
    db = get_store()
    if not db.run_exists(run_id):
        raise fastapi.HTTPException(404, "No such run.")
    db.write_games(run_id, [game.model_dump() for game in request.games],
                   request.stats)
    return {"ok": True, "games_written": len(request.games)}


@app.patch("/runs/{run_id}", dependencies=[fastapi.Depends(require_api_key)])
def update_run(run_id: str, request: UpdateRunRequest):
    db = get_store()
    if not db.run_exists(run_id):
        raise fastapi.HTTPException(404, "No such run.")
    fields = {key: value for key, value in request.model_dump().items()
              if value is not None}
    if fields:
        db.update_run(run_id, fields)
    return {"ok": True}
