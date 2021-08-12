#define _GNU_SOURCE

#include "main.h"

#ifdef DEBUG
# define DEBUG_PRINT(...) 				\
			do{							\
				printf("DEBUG: ");		\
				printf(__VA_ARGS__);	\
			}while(0)				
#else
# define DEBUG_PRINT(...) do {} while (0)
#endif

// Variables
// https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/#:~:text=int%20pthread_mutex_lock(pthread_mutex_t%20*mutex),object%2C%20which%20identifies%20a%20mutex.&text=The%20thread%20that%20has%20locked,the%20lock%20may%20be%20different.
pthread_mutex_t lock_fifo = PTHREAD_MUTEX_INITIALIZER;
sbuffer_t* sbuffer;
int log_id = 0;		// remove this with a getter

struct connmgr_thread_arg{
	FILE* log_fifo;
	int port; 
};

// MAIN ------------------------------------------------------------------------------//
int main(int argc, char *argv[]){
	DEBUG_PRINT("MAIN CALLED\n");
	assert(argv[1] != NULL);
	pid_t pid = fork();			// Req 1. - now 2 processes
	if(pid == 0) run_child();	// Req 1. - log process
	else{						// Req 1. - main process
		if(create_fifo() != 0) exit(EXIT_FAILURE);	// Req 7.
		FILE* log_fifo = fopen("logFifo", "w");
		if(log_fifo == NULL) exit(EXIT_FAILURE);
		sbuffer_init(&sbuffer);

			write_fifo(log_fifo, time(NULL), "MAIN PROCESS STARTED");

			connmgr_thread_arg_t* conn_argument = (connmgr_thread_arg_t*)malloc(sizeof(connmgr_thread_arg_t));
			conn_argument->log_fifo = log_fifo;
			conn_argument->port = atoi(argv[1]);
			DEBUG_PRINT("port number inserted in struct = %d\n", atoi(argv[1]));

			// https://man7.org/linux/man-pages/man3/pthread_create.3.html
			DEBUG_PRINT("Creating threads\n");
			pthread_t writer_connmgr, reader_datamgr, reader_sensor_db;
			pthread_create(&writer_connmgr, NULL, &connmgr, conn_argument);	//NULL -> default attributes & args -> single argument given to the function
			pthread_create(&reader_datamgr, NULL, &datamgr, log_fifo);
			pthread_create(&reader_sensor_db, NULL, &sensor_db, log_fifo);

			// https://man7.org/linux/man-pages/man3/pthread_join.3.html
			// if connmgr stops first: SBUFFER_FINISHED -> so the others will stop as well
			// if sensor_db stops first:	connmgr should be cancelled -> when db fails
			// datamgr can only stop when SBUFFER_FINISHED
			DEBUG_PRINT("Waiting for threads to terminate with pthread_join()\n");
			void *connmgr_exit_status, *datamgr_exit_status, *sensor_db_exit_status;
			pthread_join(reader_sensor_db, &sensor_db_exit_status);
				pthread_cancel(writer_connmgr); // gives memory leak i can't solve (yet)
			pthread_join(writer_connmgr, &connmgr_exit_status);	// additional check to get status
			pthread_join(reader_datamgr, &datamgr_exit_status);

			printf("< All threads joined.\n");

		free(conn_argument);
		sbuffer_free(&sbuffer);
		if(remove_fifo(log_fifo) != 0) printf("Error: logFifo doesn't exist anymore.");
		waitpid(pid, NULL, 0);
		printf("< End of main process.\n");
		exit(EXIT_SUCCESS);
	}
}
//------------------------------------------------------------------------------------//


// Other functions
int create_fifo(){
	DEBUG_PRINT("CREATE_FIFO CALLED\n");
	// Implementation: 	https://pubs.opengroup.org/onlinepubs/009696799/functions/mkfifo.html
	// Premissions:		https://pubs.opengroup.org/onlinepubs/7908799/xsh/sysstat.h.html
	// Check if logFifo already exists
	if(access("logFifo", F_OK) == 0) return 0;
	else{
		int status = mkfifo("logFifo", S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);	// Req 7.
		// Permissions: 	owner write | owner read | group read | others read
		return status;
	}
}
int write_fifo(FILE* log_fifo, time_t ts, char * message){	// Req 8. - <sequence number> <timestamp> <log-event info message>
	DEBUG_PRINT("WRITE_FIFO CALLED\n");
	// https://www.tutorialspoint.com/c_standard_library/c_function_fputs.htm
	char* fifo_input;
	asprintf(&fifo_input, "%d %ld %s\n", log_id, ts, message);
	pthread_mutex_lock(&lock_fifo);
		if(fputs(fifo_input, log_fifo) == EOF) exit(EXIT_FAILURE);
		fflush(log_fifo);
	pthread_mutex_unlock(&lock_fifo);
	printf("< Fifo message input: %s\n", fifo_input);
	log_id++;
	free(fifo_input);
	return 0;
}	
int remove_fifo(FILE* log_fifo){
	DEBUG_PRINT("REMOVE_FIFO CALLED\n");
	fclose(log_fifo);
	//if(access("logFifo", F_OK) == 0){
		remove("logFifo");
		return 0;
	//}
	//else return -1;
}
void read_fifo(FILE* log_fifo, FILE* gateway_log){
	DEBUG_PRINT("READ_FIFO CALLED\n");
	// https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
	char* fifo_output_result;
	char fifo_output[MAX_FIFO_CHARACTERS];
	do{
		pthread_mutex_lock(&lock_fifo);
			fifo_output_result = fgets(fifo_output, MAX_FIFO_CHARACTERS, log_fifo);
			if(fifo_output_result != NULL){
				printf("< Fifo message output: %s\n", fifo_output);
				fprintf(gateway_log, "%s", fifo_output);
			}
		pthread_mutex_unlock(&lock_fifo);
	}while(fifo_output_result != NULL);
}

void run_child(){
	DEBUG_PRINT("RUN_CHILD CALLED\n");
	// Req 1. - log process
	pid_t child_pid, parent_pid;
	child_pid = getpid();
	parent_pid = getppid();
	printf("< Child process[%d] forked from parent[%d].\n", child_pid, parent_pid);

	if(create_fifo() != 0) exit(EXIT_FAILURE);	// Req 7.
	FILE* log_fifo = fopen("logFifo", "r");
	FILE* gateway_log = fopen("gateway.log", "w");

	read_fifo(log_fifo, gateway_log);

	fclose(log_fifo);
	fclose(gateway_log);

	printf("< End of log process.\n");
	exit(EXIT_SUCCESS);
}
void* connmgr(void* conn_argument){
	// https://programmer.help/blogs/two-methods-for-immediate-cancellation-of-pthread-threads.html
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	connmgr_thread_arg_t* connmgr_args = conn_argument;
	connmgr_listen(connmgr_args->port, sbuffer, connmgr_args->log_fifo);
	printf("< Connmgr thread finished.\n");
	pthread_exit(NULL);
}
void* datamgr(){
	FILE *fp = fopen("./room_sensor.map","r");
	if(fp == NULL){
		printf("ERROR: opening room sensor map, file probably missing.\n\t> Run './file_creator' to generate these files.");
		exit(EXIT_FAILURE);
	}
	// <TEMP - replace with buffer
	FILE *fpdata = fopen("./sensor_data","r");
	if(fpdata == NULL){
		printf("ERROR: opening sensor data, file probably missing.\n\t> Run './file_creator' to generate these files.");
		exit(EXIT_FAILURE);
	}
	// TEMP/>
	datamgr_parse_sensor_files(fp, fpdata);

	datamgr_free();
	fclose(fp);
	fclose(fpdata);
	printf("< Datamgr thread finished\n");
	pthread_exit(NULL);
}
void* sensor_db(){
	sqlite3* db;
	FILE* sensor_data = fopen("sensor_data_text", "r");
	int tries;
	while(1){
		if(db == NULL){
			tries++; //REVIEW TRIES
			db = init_connection(1);
		}
		else{
			if(insert_sensor_from_file(db, sensor_data) == 0){
				// change this with insert_sensor_from_buffer
				disconnect(db);
				printf("< Sqlite3 database disconnected.\n");
				pthread_exit(NULL);
			}
			else{
				// insert EOB in sbuffer because can't connect db
			}
		}
	}
	printf("< Sensor_db thread finished.\n");
	pthread_exit(NULL);
}