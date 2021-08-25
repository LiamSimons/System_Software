#define _GNU_SOURCE
#include "sensor_db.h"

#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list 

#ifdef DEBUG
# define DEBUG_PRINT(...) 				\
			do{							\
				printf("DEBUG: ");		\
				printf(__VA_ARGS__);	\
			}while(0)				
#else
# define DEBUG_PRINT(...) do {} while (0)
#endif

DBCONN *init_connection(char clear_up_flag, FILE* log_fifo){
	// return NULL; // TESTING AND DEBUGGING: program terminates when database connection fails
	char *dbname = TO_STRING(DB_NAME);
	char *tablename = TO_STRING(TABLE_NAME);
	char *err_msg = 0;
	char* log;

	// Tutorial from: https://zetcode.com/db/sqlitec/
	sqlite3 *db;
	
	int rc = sqlite3_open(dbname, &db);

	if(rc != SQLITE_OK){
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		asprintf(&log, "Sensor_db: Unable to connect to SQL server: %s\n", sqlite3_errmsg(db)); //Req 9.3.d
		write_fifo(log_fifo, time(NULL), log);
		sqlite3_close(db);
		return NULL;
	}
	printf("< Database connection established.\n");
	asprintf(&log, "Sensor_db: Connection to SQL server established."); // Req 9.3.a
	write_fifo(log_fifo, time(NULL), log);
	free(log);
	if(clear_up_flag == 1){
		char sql[300];

		snprintf(sql, 300, "DROP TABLE IF EXISTS %s;"
				 "CREATE TABLE %s(id INTEGER PRIMARY KEY AUTOINCREMENT, sensor_id INT, sensor_value DECIMAL(4.2), sensor_ts TIMESTAMP);",
				 tablename, tablename);
		rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
		asprintf(&log, "Sensor_db: New table <%s> created.", tablename); // Req 9.3.b
		write_fifo(log_fifo, time(NULL), log);
		free(log);

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
				 "CREATE TABLE IF NOT EXISTS %s(id INTEGER PRIMARY KEY AUTOINCREMENT, sensor_id INT, sensor_value DECIMAL(4.2), sensor_ts TIMESTAMP);",
				 tablename);
		rc = sqlite3_exec(db, sql2, 0, 0, &err_msg);
		asprintf(&log, "Sensor_db: New table <%s> created.", tablename); // Req 9.3.b
		write_fifo(log_fifo, time(NULL), log);
		free(log);

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
	snprintf(sql, 300, "INSERT INTO %s(sensor_id, sensor_value, sensor_ts) VALUES(%hu, %f, %ld)", tablename, id, value, ts);
	int rc = sqlite3_exec(conn, sql, 0, 0, &err_msg);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return 1;
	}
	printf("\t> Sensor_db: Inserted new sensor measurement.\n");
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
// ADDED: continuous function to insert sensors from the sbuffer 
int insert_sensor_from_buffer(DBCONN *conn, sbuffer_t* sbuffer, int reader_thread, FILE* log_fifo){
	int sbuffer_status; 		// Checks what the sbuffer_read returns
	int insert_sensor_status;	// Checks the status of the inser_sensor return
	sensor_data_t* data = (sensor_data_t*)malloc(sizeof(sensor_data_t));	// to store what is read from the buffer
	while(1){
		sbuffer_status = sbuffer_read(sbuffer, data, reader_thread);
		DEBUG_PRINT("Sensor_db: read data from buffer.\n");
		if(sbuffer_status == SBUFFER_FINISHED){
			printf("< SENSOR_DB: Sbuffer finished, closing the database...\n");
			//disconnect(conn);
			break;
		}
		if(sbuffer_status == SBUFFER_SUCCESS){
			// Req 6. - attempt fails -> try again after 5 sec; max 3 attempts
			int attempts = 0;
			int success = 0;
			while((attempts < MAX_ATTEMPTS) && !success){
				insert_sensor_status = insert_sensor(conn, data->id, data->value, data->ts);	// no need to check if data is null because if it was the buffer wouldn't return SBUFFER_SUCCESS, there would be a segmentation fault stated when trying to print the data
				if(insert_sensor_status){
					attempts++;
					printf("\t> Inserting sensor from sbuffer failed, connection to SQL database lost.");
					// Write log
					char* log;
					asprintf(&log, "Sbuffer: Inserting sensor from sbuffer failed, connection to SQL server lost.\n");	// Req 9.3.c
					write_fifo(log, time(NULL), log_fifo);
					free(log);
					sleep(5);
				}
				else success = 1;
				if(attempts == 0 && success == 0){
					free(data);
					disconnect(conn);
					return -1;
				}
			}
		}
	}
	free(data);
	// disconnect happens in main
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
	snprintf(sql, 300, "SELECT * FROM %s WHERE sensor_ts = %ld", tablename, ts);
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
	snprintf(sql, 300, "SELECT * FROM %s WHERE sensor_ts > %ld", tablename, ts);
	int rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		return 1;
	}
	return 0;
}
