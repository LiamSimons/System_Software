#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "./lib/dplist.h"
#include "config.h"
#include "datamgr.h"

int main(){
	FILE *fp = fopen("./room_sensor.map","r");
	datamgr_parse_sensor_files(fp, fp);
	print_list();
	fclose(fp);
	//datamgr_free();
}