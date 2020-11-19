#include <stdlib.h>
#include <stdio.h>
#include "./lib/dplist.h"
#include "config.h"
#include "datamgr.h"

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){
	dplist_t *list = dpl_create();
	return;
}

void datamgr_free(){
	return;
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
	return;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
	return;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
	return;
}

int datamgr_get_total_sensors(){
	return;
}
