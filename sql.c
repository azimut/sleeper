#include "./sql.h"
#include "./config.h"

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

static void insert(time_t time, const char *event) {
  const char *date = ctime(&time);
  if (!date)
    err(EXIT_FAILURE, "failed to ctime()");
  const char *args[2] = {event, date};

  PGconn *conn = new ();
  PGresult *res =
      PQexecParams(conn, "INSERT INTO events(etype, created_at) VALUES($1,$2)",
                   2, NULL, args, NULL, NULL, 0);
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    do_exit(conn, res);

  PQclear(res);
  PQfinish(conn);
}

void sql_insert_sleep(time_t time) { insert(time, "sleep"); }
void sql_insert_awake(time_t time) { insert(time, "awake"); }

void sql_ping(void) {
  PGconn *conn = new ();
  PQfinish(conn);
}
