/*#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
//#include "./lib/dplist.h"
#include "config.h"*/
#include "sensor_db.h"

#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list 


DBCONN *init_connection(char clear_up_flag){
	char *dbname = TO_STRING(DB_NAME);
	char *tablename = TO_STRING(TABLE_NAME);
	char *err_msg = 0;

	//tutorial
	sqlite3 *db;
	//sqlite3_stmt *res;

	int rc = sqlite3_open(dbname, &db);

	if(rc != SQLITE_OK){
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return NULL;
	}

	/*rc = sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);

	if(rc != SQLITE_OK){
		fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return NULL;
	}

	rc = sqlite3_step(res);

	if(rc == SQLITE_ROW){
		printf("%s\n", sqlite3_column_text(res, 0));
	}

	sqlite3_finalize(res);*/
	if(clear_up_flag == 1){
		char sql[300];

		snprintf(sql, 300, "DROP TABLE IF EXISTS %s;"
				 "CREATE TABLE %s(id INTEGER PRIMARY KEY AUTOINCREMENT, sensor_id INT, sensor_value DECIMAL(4.2), timestamp TIMESTAMP);",
				 tablename, tablename);
		rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

		if(rc != SQLITE_OK){
			fprintf(stderr, "SQL error clean: %s\n", err_msg);

			sqlite3_free(err_msg);
			sqlite3_close(db);

			return NULL;
		}
	}
	else{
		char sql2[300];

		snprintf(sql2, 300, 
				 "CREATE TABLE IF NOT EXISTS %s(id INTEGER PRIMARY KEY AUTOINCREMENT, sensor_id INT, sensor_value DECIMAL(4.2), timestamp TIMESTAMP);",
				 tablename);
		rc = sqlite3_exec(db, sql2, 0, 0, &err_msg);

		if(rc != SQLITE_OK){
			fprintf(stderr, "SQL error: %s\n", err_msg);

			sqlite3_free(err_msg);
			sqlite3_close(db);

			return NULL;
		}
	}
	return db;
}

void disconnect(DBCONN *conn){
	sqlite3_close(conn);
	return;
}

int insert_sensor(DBCONN *conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
	char *err_msg = 0;
	char sql[300];
	char *tablename = TO_STRING(TABLE_NAME);
	snprintf(sql, 300, "INSERT INTO %s(sensor_id, sensor_value, timestamp) VALUES(%hu, %f, %ld)", tablename, id, value, ts);
	int rc = sqlite3_exec(conn, sql, 0, 0, &err_msg);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return 1;
	}
	return 0;
}

int insert_sensor_from_file(DBCONN *conn, FILE *sensor_data){
	//variables
	uint16_t temp_sensor_id;
	float temp_value;
	time_t temp_ts;

	//read
	while(fread(&temp_sensor_id, sizeof(temp_sensor_id), 1, sensor_data) > 0){
		fread(&temp_value, sizeof(temp_value), 1, sensor_data);
		fread(&temp_ts, sizeof(temp_ts), 1, sensor_data);
		int test = insert_sensor(conn, temp_sensor_id, temp_value, temp_ts);
		if(test != 0){
			return 1;
		}
	}
	return 0;
}

int find_sensor_all(DBCONN *conn, callback_t f){
	char *err_msg = 0;
	char sql[300];
	char *tablename = TO_STRING(TABLE_NAME);
	snprintf(sql, 300, "SELECT * FROM %s", tablename);
	int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return 1;
	}
	return 0;
}

int find_sensor_by_value(DBCONN *conn, sensor_value_t value, callback_t f){
	char *err_msg = 0;
	char sql[300];
	char *tablename = TO_STRING(TABLE_NAME);
	snprintf(sql, 300, "SELECT * FROM %s WHERE sensor_value = %f", tablename, value);
	int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return 1;
	}
	return 0;
}

int find_sensor_exceed_value(DBCONN *conn, sensor_value_t value, callback_t f){
	char *err_msg = 0;
	char sql[300];
	char *tablename = TO_STRING(TABLE_NAME);
	snprintf(sql, 300, "SELECT * FROM %s WHERE sensor_value > %f", tablename, value);
	int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return 1;
	}
	return 0;
}

int find_sensor_by_timestamp(DBCONN *conn, sensor_ts_t ts, callback_t f){
	char *err_msg = 0;
	char sql[300];
	char *tablename = TO_STRING(TABLE_NAME);
	snprintf(sql, 300, "SELECT * FROM %s WHERE timestamp = %ld", tablename, ts);
	int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return 1;
	}
	return 0;
}

int find_sensor_after_timestamp(DBCONN *conn, sensor_ts_t ts, callback_t f){
	char *err_msg = 0;
	char sql[300];
	char *tablename = TO_STRING(TABLE_NAME);
	snprintf(sql, 300, "SELECT * FROM %s WHERE timestamp > %ld", tablename, ts);
	int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return 1;
	}
	return 0;
}
