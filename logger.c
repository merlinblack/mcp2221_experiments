#include <linux/limits.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "i2c.h"
#include "options.h"
#include "sensors.h"

// Options from command line
typedef struct {
  int bus;
  bool verbose;
  char db_filename[PATH_MAX];
} cmdline_options;

void get_logger_options(int argc, char* argv[], cmdline_options* options)
{
  // defaults
  options->bus = 1;
  options->verbose = false;
  options->db_filename[0] = 0;
  strncat(options->db_filename, "measurements.db", PATH_MAX - 1);

  while (true) {
    int opt = getopt(argc, argv, "b:f:v");

    if (opt == -1) {
      return;
    }

    switch (opt) {
      case 'b':
        options->bus = atoi(optarg);
        break;
      case 'f':
        options->db_filename[0] = 0;
        strncat(options->db_filename, optarg, PATH_MAX - 1);
        break;
      case 'v':
        options->verbose = true;
        break;
    }
  }
}

int getDB(const char* filename, sqlite3** db)
{
  int ret;

  ret = sqlite3_open(filename, db);

  if (ret) {
    fprintf(stderr, "Issue in opening db: %s", sqlite3_errmsg(*db));
    return ret;
  }

  sqlite3_stmt* stmt;

  const char* sql =
      "create table if not exists measurements ("
      "id integer primary key,"
      "timestamp bigint,"
      "temperature float,"
      "pressure float,"
      "humidity float,"
      "tvoc integer,"
      "location varchar(255)"
      ")";

  ret = sqlite3_prepare_v2(*db, sql, -1, &stmt, nullptr);

  if (ret) {
    fprintf(stderr, "Issue in preparing statement`: %s", sqlite3_errmsg(*db));
    sqlite3_close(*db);
    return ret;
  }

  ret = sqlite3_step(stmt);
  if (ret != SQLITE_DONE) {
    fprintf(stderr, "Issue running statement`: %s", sqlite3_errmsg(*db));
    sqlite3_close(*db);
    return ret;
  }

  sqlite3_finalize(stmt);

  return SQLITE_OK;
}

int write_measurements(sqlite3* db,
                       float temperature,
                       float pressure,
                       float humidity,
                       uint32_t tvoc,
                       time_t timestamp)
{
  sqlite3_stmt* stmt;
  int ret;

  const char* sql =
      "insert into measurements"
      "(temperature, pressure, humidity, tvoc, timestamp, location)"
      "values (?,?,?,?,?,?)";

  ret = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  if (ret) {
    return ret;
  }

  ret = sqlite3_bind_double(stmt, 1, temperature);
  if (ret) {
    return ret;
  }
  ret = sqlite3_bind_double(stmt, 2, pressure);
  if (ret) {
    return ret;
  }
  ret = sqlite3_bind_double(stmt, 3, humidity);
  if (ret) {
    return ret;
  }
  ret = sqlite3_bind_int(stmt, 4, tvoc);
  if (ret) {
    return ret;
  }
  ret = sqlite3_bind_int(stmt, 5, timestamp);
  if (ret) {
    return ret;
  }
  ret = sqlite3_bind_text(stmt, 6, "Alisons place, Kingwood", -1, SQLITE_STATIC);
  if (ret) {
    return ret;
  }

  ret = sqlite3_step(stmt);
  if (ret != SQLITE_DONE) {
    return ret;
  }

  sqlite3_finalize(stmt);

  return SQLITE_OK;
}

int main(int argc, char* argv[])
{
  cmdline_options options;

  get_logger_options(argc, argv, &options);

  if (options.verbose) {
    printf("DB file: %s\nI2C Bus: %d\n", options.db_filename, options.bus);
    cli_opts.verbose = true;
  }

  sqlite3* db;

  int ret = getDB(options.db_filename, &db);

  if (ret != SQLITE_OK) {
    return EXIT_FAILURE;
  }

  int i2c = i2c_open(options.bus, -1);

  float temperature;
  float temperature2;
  float pressure;
  float humidity;
  bool ready;
  uint32_t tvoc;

  time_t timestamp = time(nullptr);

  if (get_measurement_bmp280(i2c, &temperature, &pressure)) {
    sqlite3_close(db);
    return EXIT_FAILURE;
  }

  pressure /= 100.0;

  i2c_select(i2c, AHT10_ADDRESS);
  if (get_measurement_aht10(i2c, &temperature2, &humidity)) {
    sqlite3_close(db);
    return EXIT_FAILURE;
  }

  i2c_select(i2c, AGS10_ADDRESS);
  if (get_measurement_ags10(i2c, &ready, &tvoc)) {
    sqlite3_close(db);
    return EXIT_FAILURE;
  }

  ret = write_measurements(db, temperature, pressure, humidity, tvoc, timestamp);
  if (ret) {
    fprintf(stderr, "Issue inserting data: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return EXIT_FAILURE;
  }

  sqlite3_close(db);

  if (options.verbose) {
    printf(
        "temperature: %0.2f\n"
        "   humidity: %0.2f\n"
        "   pressure: %0.2f\n"
        "       tvoc: %u\n",
        temperature, humidity, pressure, tvoc);
  }

  return EXIT_SUCCESS;
}
