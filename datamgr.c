#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "./lib/dplist.h"
#include "config.h"
#include "datamgr.h"

#define NO_ERROR "c"
#define MEMORY_ERROR "b"
#define INVALID_ERROR "a"

typedef struct {
    uint16_t sensor_id;
    uint16_t room_id;
    time_t last_modified;
    double running_av;
    double temperatures[RUN_AVG_LENGTH];
} sensor_t;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CALLBACK FUNCTIONS

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element){
	return element;
}
void element_free(void ** element) {
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y){
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATAMANAGER

// list aanmaken
dplist_t *list;

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){
	list  = dpl_create(element_copy, element_free, element_compare);
	// int fscanf(FILE *stream, const char *format, ...);
	//<room ID><space><sensor ID><\n>
	uint16_t temp_room_id;
	uint16_t temp_sensor_id;
	while(fscanf(fp_sensor_map, "%hd %hd", &temp_room_id, &temp_sensor_id)>0){
		//malloc
		sensor_t *data = (sensor_t*)malloc(sizeof(sensor_t));
		ERROR_HANDLER(data==NULL, MEMORY_ERROR);
		//change 
		data->room_id = temp_room_id;
		data->sensor_id = temp_sensor_id;
		list = dpl_insert_at_index(list, data, 1000, false);
				//1000 zodat die vanachter komt
	}
	//printf("Size of list = %i\n", dpl_size(list));


	return;
}

void datamgr_free(){
	// int size = dpl_size(list);
	// while(size > 0){
	// 	dplist_node_t *dummy = dpl_get_reference_at_index(list, size-1);
	// 	sensor_data_t *sensor_dummy;
	// 	sensor_dummy = dummy->element;
	// 	free(sensor_dummy);
	// }
	dpl_free(&list, true);
	printf("Size of list ater free = %i\n", dpl_size(list));
	return;
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
	sensor_data_t *data = (sensor_data_t*)dpl_get_element_at_index(list, 0);
	return data->id;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
	sensor_data_t *data = (sensor_data_t*)dpl_get_element_at_index(list, 0);
	return data->value;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
	sensor_data_t *data = (sensor_data_t*)dpl_get_element_at_index(list, 0);
	return data->ts;
}

int datamgr_get_total_sensors(){
	return 0;
}

void print_list(){
	if(list == NULL) printf("LIST == NULL\n");
	else{
		int size = dpl_size(list);
		int i = 0;
		while(i < size){
			sensor_t *dummy;
			dummy = dpl_get_element_at_index(list, i);
			printf("SENSOR ID: %hd\tROOM ID: %hd\n", dummy->sensor_id, dummy->room_id);
			i = i + 1;
		}
	}
}