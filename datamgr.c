#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "./lib/dplist.h"
#include "config.h"
#include "datamgr.h"	



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CALLBACK FUNCTIONS
typedef struct {
    int id;
    char* name;
} my_element_t;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name,"%s",((my_element_t*)element)->name);
    //assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

void element_free(void ** element) {
    free((((my_element_t*)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}
// list aanmaken
dplist_t *list;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATAMANAGER

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){
	list  = dpl_create(element_copy, element_free, element_compare);
	sensor_data_t *data = (sensor_data_t*)malloc(sizeof(sensor_data_t));
	list = dpl_insert_at_index(list, data, 0, false);
	return;
}

void datamgr_free(){
	return;
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
	sensor_data_t *data = dpl_get_element_at_index(list, 0);
	return data->id;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
	sensor_data_t *data = dpl_get_element_at_index(list, 0);
	return data->value;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
	sensor_data_t *data = dpl_get_element_at_index(list, 0);
	return data->ts;
}

int datamgr_get_total_sensors(){
	return 0;
}