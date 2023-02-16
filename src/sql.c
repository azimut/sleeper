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

static void insert(const char *sleep_at, const char *wakeup_at, const char *diff) {
  PGconn *conn = new ();
  PGresult *res =
      PQexecParams(conn,
                   "INSERT INTO events(sleep_at, wakeup_at, diff) "
                   "VALUES($1,$2,$3)",
                   2, NULL, (const char *[]){sleep_at, wakeup_at, diff}, NULL, NULL, 0);
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    do_exit(conn, res);

  PQclear(res);
  PQfinish(conn);
}

void sql_insert_event(const time_t sleep, const time_t wakeup) {
  const char *sleep_at = ctime(&sleep);
  if (!sleep_at)
    err(EXIT_FAILURE, "failed to ctime() sleep");

  const char *wakeup_at = ctime(&wakeup);
  if (!wakeup_at)
    err(EXIT_FAILURE, "failed to ctime() wakeup");

  const double rawdiff = difftime(wakeup, sleep);
  assert(rawdiff > 0);

  char diff[128];
  if (sprintf(diff, "%f", rawdiff) < 0)
    err(EXIT_FAILURE, "failed to sprintf() double to string");

  insert(sleep_at, wakeup_at, diff);
}

void sql_ping(void) {
  PGconn *conn = new ();
  PQfinish(conn);
}
