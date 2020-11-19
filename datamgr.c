#include <stdlib.h>
#include <stdio.h>
#include "./lib/dplist.h"
#include "config.h"
#include "datamgr.h"

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
    assert(copy != NULL);
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

// DATAMANAGER

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){
	dplist_t *list = dpl_create(element_copy, element_free, element_compare);
	return;
}

void datamgr_free(){
	return;
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
	return NULL;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
	return NULL;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
	return NULL;
}

int datamgr_get_total_sensors(){
	return NULL;
}
