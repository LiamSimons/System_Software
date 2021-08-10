#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "/home/liam/Desktop/Sysprog/lab8/lib/dplist.h"
#include "/home/liam/Desktop/Sysprog/lab8/config.h"
#include "/home/liam/Desktop/Sysprog/lab8/datamgr.h"

int main(){
	// opening files
	FILE *fp = fopen("./room_sensor.map","r");
	if(fp == NULL){
		printf("ERROR: opening room sensor map, file probably missing.\n\t> Run './file_creator' to generate these files.");
		exit(EXIT_FAILURE);
	}
	FILE *fpdata = fopen("./sensor_data","r");
	if(fpdata == NULL){
		printf("ERROR: opening sensor data, file probably missing.\n\t> Run './file_creator' to generate these files.");
		exit(EXIT_FAILURE);
	}

	// test parse
	datamgr_parse_sensor_files(fp, fpdata);
	printf("After parse\n");
	print_list();

	// test get_room_id
	printf("\nRoom ID of sensor 15 = %d\n", datamgr_get_room_id(15));
	//printf("\nRoom ID of sensor 16 = %d\n", datamgr_get_room_id(16));

	// test get avg
	printf("Average of sensor 112 = %f\n", datamgr_get_avg(112));

	// test last modified
	printf("The last modified time of sensor 142 = %ld\n", datamgr_get_last_modified(142));
	
	// test get total sensors
	printf("Total amount of sensors = %i\n", datamgr_get_total_sensors());

	// freeing and closing
	datamgr_free();
	fclose(fp);
	fclose(fpdata);
}


// parse seems to be working

// get room id from sensor id seems to be working

// get avg seems to be working

//