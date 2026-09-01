# FollyTest

FollyTest is a web app for browsing FollyChess SPRT test results, in the
spirit of [tests.stockfishchess.org](https://tests.stockfishchess.org/tests).

## Architecture

* `api/`: a write-only ingestion API (Python + FastAPI) that
  [scripts/sprt.py](../scripts/sprt.py) posts runs and game batches to. Runs
  on Cloud Run. Requires the `X-Api-Key` header on every request.
* Firestore: `runs/{id}` documents hold run metadata, status, and rolling
  statistics; `runs/{id}/games/{n}` holds one document per game, keyed by
  game number so that batch retries are idempotent.
* `frontend/`: a React app (Vite) served by Firebase Hosting. It reads
  Firestore directly using the public read-only security rules in
  [firestore.rules](firestore.rules), so run pages update live while a match
  is in progress.

The local `testing/results/` directories remain the source of truth; the
uploader is best-effort and `scripts/sprt.py --backfill RUN_DIR` re-uploads
any run after the fact.

## Local development

API (in-memory storage, no Google Cloud needed):

```shell
cd web/api
python3.13 -m venv .venv && .venv/bin/pip install -r requirements.txt
STORE=memory API_KEY=dev .venv/bin/uvicorn main:app --port 8080
```

Frontend:

```shell
cd web/frontend
npm install
cp .env.example .env.local  # Fill in the Firebase web API key.
npm run dev
```

To make `scripts/sprt.py` upload, set:

```shell
export FOLLYTEST_API_URL=http://localhost:8080  # Or the Cloud Run URL.
export FOLLYTEST_API_KEY=dev
```

## Deployment

One-time setup: create the `follychess` Firebase project, enable Firestore
(Native mode), and register a web app in the Firebase console to obtain the
web API key for `frontend/.env.local`.

The API key lives in Secret Manager so that it never appears in source,
command lines, or the Cloud Run console. To rotate it:

```shell
openssl rand -hex 24 | tr -d '\n' | \
    gcloud secrets versions add follytest-api-key --data-file=- \
        --project follychess
```

API to Cloud Run:

```shell
cd web/api
gcloud run deploy follytest-api --source . --project follychess \
    --region us-west1 --allow-unauthenticated \
    --set-secrets API_KEY=follytest-api-key:latest
```

Firestore rules and the frontend:

```shell
cd web
firebase deploy --only firestore:rules
(cd frontend && npm run build)
firebase deploy --only hosting
```
