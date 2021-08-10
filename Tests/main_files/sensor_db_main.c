#include "/home/liam/Desktop/Sysprog/lab8/config.h"
#include "/home/liam/Desktop/Sysprog/lab8/sensor_db.h"

int callback(void *NotUsed, int argc, char **argv, char **azColName){
	NotUsed = 0;
	for(int i = 0; i < argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}


int main(){
	sqlite3 * db = init_connection(0);

	/*int test_insert = insert_sensor(db, 54, 7.2, 1948);
	if(test_insert != 0){
		printf("Test insert failed.\n");
	}*/
	printf("\t\t---SELECT---\n");
	int test_select = find_sensor_all(db, callback);
	if(test_select != 0){
		printf("Test select failed.\n");
	}

	printf("\t\t---SELECT BY VALUE 15.2---\n");
	int test_select_by_value1 = find_sensor_by_value(db, 15.2, callback);
	if(test_select_by_value1 != 0){
		printf("Test select failed.\n");
	}

	printf("\t\t---SELECT BY VALUE 17.2---\n");
	int test_select_by_value2 = find_sensor_by_value(db, 17.2, callback);
	if(test_select_by_value2 != 0){
		printf("Test select failed.\n");
	}

	printf("\t\t---SELECT EXCEEDS VALUE 15---\n");
	int test_select_exceeds_value1 = find_sensor_exceed_value(db, 15, callback);
	if(test_select_exceeds_value1 != 0){
		printf("Test select failed.\n");
	}

	printf("\t\t---SELECT EXCEEDS VALUE 17---\n");
	int test_select_exceeds_value2 = find_sensor_exceed_value(db, 17, callback);
	if(test_select_exceeds_value2 != 0){
		printf("Test select failed.\n");
	}

	printf("\t\t---SELECT BY TIMESTAMP 948---\n");
	int test_select_ts = find_sensor_by_timestamp(db, 948, callback);
	if(test_select_ts != 0){
		printf("Test select ts failed.\n");
	}

	printf("\t\t---SELECT AFTER TIMESTAMP 948---\n");
	int test_select_after_ts = find_sensor_after_timestamp(db, 948, callback);
	if(test_select_after_ts != 0){
		printf("Test select after ts failed.\n");
	}

	disconnect(db);
}