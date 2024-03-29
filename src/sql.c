#include "./sql.h"
#include "./config.h"

#include <assert.h>
#include <err.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int insert(const char *etype, const char *sleep_at, const char *wakeup_at,
                  const char *diff) {
  int rc;
  sqlite3 *db;
  rc = sqlite3_open(SQL_FILE, &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  const char *query = "INSERT INTO events(etype, sleep_at, wakeup_at, diff) "
                      "VALUES(?,?,?,?);";
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failde to prepare statement: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  rc = sqlite3_bind_text(stmt, 1, etype, -1, SQLITE_STATIC);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to bind parameter: %s\n", sqlite3_errmsg(db));
    return 1;
  }
  rc = sqlite3_bind_text(stmt, 2, sleep_at, -1, SQLITE_STATIC);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to bind parameter: %s\n", sqlite3_errmsg(db));
    return 1;
  }
  rc = sqlite3_bind_text(stmt, 3, wakeup_at, -1, SQLITE_STATIC);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to bind parameter: %s\n", sqlite3_errmsg(db));
    return 1;
  }
  rc = sqlite3_bind_text(stmt, 4, diff, -1, SQLITE_STATIC);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to bind parameter: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    return 1;
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return 0;
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

int sql_oneshot(const char *query) {
  sqlite3 *db;
  int rc;
  char *zErrMsg = 0;
  rc = sqlite3_open(SQL_FILE, &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }
  rc = sqlite3_exec(db, query, NULL, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    sqlite3_free(zErrMsg);
    return 1;
  }
  sqlite3_close(db);
  return 0;
}

int sql_initdb(void) {
  int rc;
  const char *events = "create table if not exists events ("
                       "  id        integer primary key,"
                       "  etype     text    not null,"
                       "  sleep_at  integer not null,"
                       "  wakeup_at integer not null,"
                       "  diff      real    not null"
                       ");";

  rc = sql_oneshot(events);
  if (rc) {
    fprintf(stderr, "Couldn't create table events\n");
    return rc;
  }

  const char *config = "create table if not exists config ("
                       "  wakuptime integer not null,"
                       "  sleeptime integer not null"
                       ");";

  rc = sql_oneshot(config);
  if (rc) {
    fprintf(stderr, "Couldn't create table config\n");
    return rc;
  }

  return 0;
}
