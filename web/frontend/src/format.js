export function formatElo(stats) {
  if (!stats || stats.elo === undefined || stats.elo === null) return "—";
  const error = stats.elo_error !== undefined ? ` ± ${stats.elo_error}` : "";
  return `${stats.elo}${error}`;
}

export function formatScore(stats) {
  if (!stats || stats.wins === undefined) return "—";
  return `+${stats.wins} −${stats.losses} =${stats.draws}`;
}

export function formatTimestamp(value) {
  if (!value) return "—";
  const date = value.toDate ? value.toDate() : new Date(value);
  return date.toLocaleString(undefined, {
    year: "numeric", month: "short", day: "numeric",
    hour: "2-digit", minute: "2-digit",
  });
}

export function shortCommit(commit) {
  return commit ? commit.slice(0, 7) : "—";
}
