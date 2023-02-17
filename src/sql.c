#include "./sql.h"
#include "./config.h"

#include <assert.h>
#include <err.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void do_exit(PGconn *conn, PGresult *res) {
  fprintf(stderr, "%s\n", PQerrorMessage(conn));
  PQclear(res);
  PQfinish(conn);
  exit(EXIT_FAILURE);
}

static PGconn *new (void) {
  PGconn *conn = PQconnectdb(SQL_CONNECT);
  if (PQstatus(conn) == CONNECTION_BAD) {
    fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
    PQfinish(conn);
    exit(EXIT_FAILURE);
  }
  return conn;
}

static void insert(const char *etype, const char *sleep_at, const char *wakeup_at,
                   const char *diff) {
  const char *query = "INSERT INTO events(etype, sleep_at, wakeup_at, diff) "
                      "VALUES($1,$2,$3,$4)";
  const char *args[4] = {etype, sleep_at, wakeup_at, diff};
  PGconn *conn = new ();
  PGresult *res = PQexecParams(conn, query, 4, NULL, args, NULL, NULL, 0);
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    do_exit(conn, res);

  PQclear(res);
  PQfinish(conn);
}

static char *format_date(const time_t t) {
  struct tm *tmp = localtime(&t);
  if (!tmp)
    err(EXIT_FAILURE, "failed localtime()");

  char *buf = malloc(sizeof(char) * 100);
  if (!buf)
    err(EXIT_FAILURE, "failed malloc()");

  if (strftime(buf, 100, "%a %b %d %T %Y", tmp) == 0)
    err(EXIT_FAILURE, "failed strftime()");

  return buf;
}

void sql_insert_event(const char *etype, const time_t sleep, const time_t wakeup) {
  const double rawdiff = difftime(wakeup, sleep);
  assert(rawdiff >= 0);

  char diff[128];
  if (sprintf(diff, "%f", rawdiff) < 0)
    err(EXIT_FAILURE, "failed to sprintf() double to string");

  char *sleep_at = format_date(sleep);
  char *wakeup_at = format_date(wakeup);

  insert(etype, sleep_at, wakeup_at, diff);

  free(sleep_at);
  free(wakeup_at);
}

void sql_ping(void) {
  PGconn *conn = new ();
  PQfinish(conn);
}
