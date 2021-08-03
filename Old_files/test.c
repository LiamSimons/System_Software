
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "./lib/dplist.h"
#include "config.h"
#include "datamgr.h"	


int main()
{
	FILE *fp = fopen("./room_sensor.map","r");
	// while(!feof(fp))
	// {
	// 	int sensorID;
	// 	int roomID; 
	// 	fscanf(fp, "%d",&roomID);
	// 	fscanf(fp, "%d",&sensorID);
	// 	printf("roomID = %d and sensorID = %d\n", roomID, sensorID);
	// }

	FILE *fpdata = fopen("./sensor_data","r");
	// while(!feof(fpdata))
	// {
	// 	uint16_t sensorID;
	// 	double temp; 
	// 	time_t ts; 
	// 	fread(&sensorID, sizeof(uint16_t), 1, fpdata);
	// 	fread(&temp, sizeof(double), 1, fpdata);
	// 	fread(&ts, sizeof(time_t), 1, fpdata);
	// 	printf("sensorID = %d, temp = %g, time = %ld\n", sensorID, temp, ts);
	// }

	datamgr_parse_sensor_files(fp, fpdata);
	uint16_t roomID = datamgr_get_room_id(132);
	printf("--------------------------------------------------\n");
	printf("roomID of sensor with sensorID:132 is %d\n",roomID);
	double AVG = datamgr_get_avg(49);
	printf("with a average temperature of %g\n",AVG);
	printf("--------------------------------------------------\n");
	printf("total amount of sensors should be: %d\n",datamgr_get_total_sensors());
	datamgr_free();
	fclose(fp);
	fclose(fpdata);
	return 0;
}