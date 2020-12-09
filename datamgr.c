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
	sensor_t * sensor = (sensor_t *)element;
	sensor_t * copy_sensor = malloc(sizeof(sensor_t));

	copy_sensor->sensor_id = sensor->sensor_id;
	copy_sensor->room_id = sensor->room_id;
	copy_sensor->last_modified = sensor->last_modified;
	copy_sensor->running_av = sensor->running_av;

	for(int i=0; i<RUN_AVG_LENGTH; i++){
		copy_sensor->temperatures[i] = sensor->temperatures[i];
	}
	return copy_sensor;
}
void element_free(void ** element) {
	sensor_t * dummy = *element;
	free(dummy);
}

int element_compare(void * x, void * y){
	sensor_t * dummy = (sensor_t*)y;
	if(*(uint16_t *)x == dummy->sensor_id){
		return 0;
	}
	else{
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATAMANAGER

// list aanmaken
dplist_t *list;

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){
	list  = dpl_create(element_copy, element_free, element_compare);
	// int fscanf(FILE *stream, const char *format, ...);
	//<room ID><space><sensor ID><\n>

	//Inserting sensors
	uint16_t temp_room_id;
	uint16_t temp_sensor_id;
	while(fscanf(fp_sensor_map, "%hd %hd", &temp_room_id, &temp_sensor_id)>0){
		//malloc
		sensor_t *data = (sensor_t*)malloc(sizeof(sensor_t));
		ERROR_HANDLER(data==NULL, MEMORY_ERROR);

		//change parameters:
		// room_id | sensor_id | last_modified | running_av | temperatures[]
		data->room_id = temp_room_id;
		data->sensor_id = temp_sensor_id;
		for(int i = 0; i<RUN_AVG_LENGTH; i++){
			data->temperatures[i] = 0;
		}

		//insert in list
		list = dpl_insert_at_index(list, data, 1000, false);
				//1000 zodat die vanachter komt
	}
	//printf("Size of list = %i\n", dpl_size(list));


	//Inserting sensor data
	double temp_temperature;
	time_t temp_time;
	int index;
	while(fread(&temp_sensor_id, sizeof(temp_sensor_id), 1, fp_sensor_data) > 0){
		//find sensor with this sensor id
		int i = 0;
		while(i<dpl_size(list)){
			if(((sensor_t *)dpl_get_element_at_index(list, i))->sensor_id == temp_sensor_id){
				index = i;
			}
			i++;
		}
		sensor_t * sensor = (sensor_t*)dpl_get_element_at_index(list, index);

		//read in the temperature
		fread(&temp_temperature, sizeof(temp_temperature), 1, fp_sensor_data);

		//Shift to the right
		double temp_running_av = 0;
		for(int i = RUN_AVG_LENGTH; i>0; i--){
			sensor->temperatures[i] = sensor->temperatures[i-1];
			temp_running_av += sensor->temperatures[i];
		}

		//Add new one at the beginning
		sensor->temperatures[0] = temp_temperature;

		//recalculate running average
		temp_running_av += temp_temperature;
		sensor->running_av = temp_running_av/RUN_AVG_LENGTH;

		//read in the time stamp
		fread(&temp_time, sizeof(temp_time), 1, fp_sensor_data);
		sensor->last_modified = temp_time;

		//error message | min or max exceeded
		//only if temperatures is full - see header file : get avg
		if(sensor->temperatures[RUN_AVG_LENGTH-1] != 0){
			if (sensor->running_av < SET_MIN_TEMP)
			{
				fprintf(stderr, "Room |%hd| is too cold for sensor |%hd|.\tRunning average = %lf.\tTime = %ld\n", sensor->room_id, sensor->sensor_id, sensor->running_av, sensor->last_modified);
			}
			else if(sensor->running_av > SET_MAX_TEMP)
			{
				fprintf(stderr, "Room |%hd| is too hot for sensor |%hd|.\tRunning average = %lf.\tTime = %ld\n", sensor->room_id, sensor->sensor_id, sensor->running_av, sensor->last_modified);
			}
		}
	}
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
	//printf("Size of list ater free = %i\n", dpl_size(list));
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
	int i = 0;
	while(i<dpl_size(list)){
		sensor_t* dummy = (sensor_t *)dpl_get_element_at_index(list, i);
		if(dummy->sensor_id == sensor_id){
			return dummy->room_id;
		}
		i++;
	}
	ERROR_HANDLER(1, INVALID_ERROR);
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
	int i = 0;
	while(i<dpl_size(list)){
		sensor_t* dummy = (sensor_t *)dpl_get_element_at_index(list, i);
		if(dummy->sensor_id == sensor_id){
			return dummy->running_av;
		}
		i++;
	}
	ERROR_HANDLER(1, INVALID_ERROR);
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
	int i = 0;
	while(i<dpl_size(list)){
		sensor_t* dummy = (sensor_t *)dpl_get_element_at_index(list, i);
		if(dummy->sensor_id == sensor_id){
			return dummy->last_modified;
		}
		i++;
	}
	ERROR_HANDLER(1, INVALID_ERROR);
}

int datamgr_get_total_sensors(){
	return dpl_size(list);
}

void print_list(){
	if(list == NULL) printf("LIST == NULL\n");
	else{
		int size = dpl_size(list);
		int i = 0;
		while(i < size){
			sensor_t *dummy;
			dummy = dpl_get_element_at_index(list, i);
			printf("SENSOR ID: %hd\tROOM ID: %hd\tRunning average = %lf\n", dummy->sensor_id, dummy->room_id, dummy->running_av);
			i = i + 1;
		}
	}
}