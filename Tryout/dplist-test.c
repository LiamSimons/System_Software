#include <stdio.h>
#include <stdlib.h>
#include "./lib/dplist.h"
#include "config.h"

void* copy_element(void *element){
	sensor_data_t* copy = (sensor_data_t*)malloc(sizeof(sensor_data_t));
	copy->id = ((sensor_data_t*)element)->id;
	copy->value = ((sensor_data_t*)element)->value;
	copy->ts = ((sensor_data_t*)element)->ts;
	return (void*)copy;
}

void free_element(void **element){
	free(*element);
}

int compare_element(void *x, void *y){
	return 0;
}

int main(){
	printf("MAINNNN\n");
	dplist_t* list = dpl_create(copy_element, free_element, compare_element);
	sensor_data_t* data = (sensor_data_t*)malloc(sizeof(sensor_data_t));
	data->id = 1;
	data->value = 99;
	data->ts = time(NULL);
	list = dpl_insert_at_index(list, data, 0, true);
	sensor_data_t* dummy = dpl_get_element_at_index(list, 0);
	printf("DUMMY ID =\t%d\n", dummy->id);
	printf("DUMMY VALUE =\t%f\n", dummy->value);
	printf("DUMMY TS =\t%ld\n", dummy->ts);
	dpl_free(&list, true);
	free(data);
	return 0;
}