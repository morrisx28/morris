#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vl53l5cx_api.h"

char *device = "/dev/tof";//"/dev/i2c-10";///dev/tof"
VL53L5CX_Configuration 	Dev;
VL53L5CX_Configuration 	Dev1;
VL53L5CX_Configuration 	Dev2;
uint16_t i2c_address = 0x52;
uint16_t i2c_address1 = 0x56;
uint16_t i2c_address2 = 0x6A;

uint8_t setDeviceAddr(int tag){

	uint8_t status, isAlive;

	printf("*** VL53L5CX  setDeviceAddr! TAG : [%d] ***)\n",	tag);


	/* Initialize channel com */
	if(tag == 1){

		Dev1.platform.address = i2c_address;

		status = vl53l5cx_comms_init(&Dev1.platform, device, i2c_address);

		if(status)
		{
			printf("VL53L5CX comms init dev failed\n");
			return status;
		}

		/* (Optional) Check if there is a VL53L5CX sensor connected */
		status = vl53l5cx_is_alive(&Dev1, &isAlive);
		if(!isAlive || status)
		{
			printf("VL53L5CX not detected at requested address\n");
			return status;
		}

		printf("Start to vl53l5cx_init.\n");
		/* (Mandatory) Init VL53L5CX sensor */
		status = vl53l5cx_init(&Dev1);
		if(status)
		{
			printf("VL53L5CX ULD Loading Dev1 failed\n");
			return status;
		}

	}else if(tag == 2){
		Dev2.platform.address = i2c_address;

		status = vl53l5cx_comms_init(&Dev2.platform, device, i2c_address);

		if(status)
		{
			printf("VL53L5CX comms init dev failed\n");
			return status;
		}

		/* (Optional) Check if there is a VL53L5CX sensor connected */
		status = vl53l5cx_is_alive(&Dev2, &isAlive);
		if(!isAlive || status)
		{
			printf("VL53L5CX not detected at requested address\n");
			return status;
		}

		printf("Start to vl53l5cx_init.\n");
		/* (Mandatory) Init VL53L5CX sensor */
		status = vl53l5cx_init(&Dev2);
		if(status)
		{
			printf("VL53L5CX ULD Loading Dev2 failed\n");
			return status;
		}
	}

	printf("VL53L5CX vl53l5cx_init OK)\n");

	if(tag == 1){
		status = vl53l5cx_set_i2c_address(&Dev1, i2c_address1);
		if(status)
		{
			printf("VL53L5CX set i2c Address[%d] failed\n", tag);
			vl53l5cx_comms_close(&Dev1.platform);

			return status;
		}
		vl53l5cx_comms_close(&Dev1.platform);

	}else if(tag == 2){
		status = vl53l5cx_set_i2c_address(&Dev2, i2c_address2);

		if(status)
		{
			printf("VL53L5CX set i2c Address[%d] failed\n", tag);
			vl53l5cx_comms_close(&Dev2.platform);

			return status;
		}
		vl53l5cx_comms_close(&Dev2.platform);
	}
	return status;
}

static uint8_t fetchDistance(void);
static uint8_t singleTest(void);

int exampleVL53(void)
{

	printf("Start to exampleVL53...\n");

	/*********************************/
	/*   VL53L5CX ranging variables  */
	/*********************************/

	uint8_t 				status, isAlive;
	//VL53L5CX_ResultsData 	Results;		/* Results data from VL53L5CX */

	char choice[20];


	/*********************************/
	/*   Power on sensor and init    */
	/*********************************/

	while(1){
		printf("Your choice ?\n ");
		if( scanf("%s", choice) ){};
		if (strcmp(choice, "set1") == 0) {

			setDeviceAddr(1);

		}else if (strcmp(choice, "set2") == 0) {

			setDeviceAddr(2);

		}else if (strcmp(choice, "open") == 0) {

			uint8_t 				status;
			/* Initialize channel com */
			status = vl53l5cx_comms_openBus(&Dev.platform, device);
			if(status)
			{
				printf("VL53L5CX open failed\n");
				return -1;
			}

		}else if (strcmp(choice, "close") == 0) {

			vl53l5cx_comms_close(&Dev.platform);

		}else if (strcmp(choice, "test") == 0) {
		
			singleTest();
		}
		else if (strcmp(choice, "i1") == 0) {
		
			Dev.platform.address = i2c_address1;

			vl53l5cx_comms_CheckSpeakToSlave(&Dev.platform, device, i2c_address1);


			/* (Optional) Check if there is a VL53L5CX sensor connected */
			status = vl53l5cx_is_alive(&Dev, &isAlive);
			if(!isAlive || status)
			{
				printf("VL53L5CX not detected at requested address1\n");
				return status;
			}
			printf("Start to vl53l5cx_start_ranging.\n");
			
			status = vl53l5cx_start_ranging(&Dev);
			if(status){
				printf("VL53L5CX ULD vl53l5cx_start_ranging failed, error=[%d]\n", status);
				return -1;
			}
			printf("vl53l5cx_start_ranging Dev OK!\n");

		}else if (strcmp(choice, "i2") == 0) {
		
			Dev.platform.address = i2c_address2;
			vl53l5cx_comms_CheckSpeakToSlave(&Dev.platform, device, i2c_address2);

			/* (Optional) Check if there is a VL53L5CX sensor connected */
			status = vl53l5cx_is_alive(&Dev, &isAlive);
			if(!isAlive || status)
			{
				printf("VL53L5CX not detected at requested address2\n");
				return status;
			}
			printf("Start to vl53l5cx_start_ranging.\n");
			
			status = vl53l5cx_start_ranging(&Dev);
			if(status){
				printf("VL53L5CX ULD vl53l5cx_start_ranging failed, error=[%d]\n", status);
				return -1;
			}
			printf("vl53l5cx_start_ranging Dev 2 OK!\n");

		}else if (strcmp(choice, "1") == 0) {
				printf("Starting Fetch 1 distance.\n");
				Dev.platform.address = i2c_address1;
				fetchDistance();
		}
		else if (strcmp(choice, "2") == 0) {
				printf("Starting Fetch 2 distance.\n");
				Dev.platform.address = i2c_address2;
				fetchDistance();

		}else if (strcmp(choice, "q") == 0) {
				printf("Quit examplevl53()!\n");
				status = vl53l5cx_stop_ranging(&Dev);
				vl53l5cx_comms_close(&Dev.platform);
				return 1;
		}else if(strcmp(choice, "setResolution11") == 0){
			Dev.platform.address = i2c_address1;
			status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_4X4);
			if(status){
				printf("vl53l5cx_set_resolution1 failed, error=[%d]\n", status);
				return -1;
			}
		}else if(strcmp(choice, "setResolution12") == 0){
			Dev.platform.address = i2c_address1;
			status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
			if(status){
				printf("vl53l5cx_set_resolution1 failed, error=[%d]\n", status);
				return -1;
			}
		}else if(strcmp(choice, "setResolution21") == 0){
			Dev.platform.address = i2c_address2;
			status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_4X4);
			if(status){
				printf("vl53l5cx_set_resolution2 failed, error=[%d]\n", status);
				return -1;
			}
		}else if(strcmp(choice, "setResolution22") == 0){
			Dev.platform.address = i2c_address2;
			status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
			if(status){
				printf("vl53l5cx_set_resolution2 failed, error=[%d]\n", status);
				return -1;
			}
		}else if(strcmp(choice, "dumpResolution") == 0){
			uint8_t	p_resolution;	


			Dev.platform.address = i2c_address1;
			status = vl53l5cx_get_resolution(&Dev, &p_resolution);
			if(status){
				printf("vl53l5cx_get_resolution1 failed, error=[%d]\n", status);
				return -1;
			}
			printf("vl53l5cx_get_resolution1 =[%d]\n", p_resolution);


			Dev.platform.address = i2c_address2;
			status = vl53l5cx_get_resolution(&Dev, &p_resolution);
			if(status){
				printf("vl53l5cx_get_resolution2 failed, error=[%d]\n", status);
				return -1;
			}
			printf("vl53l5cx_get_resolution2 =[%d]\n", p_resolution);

		}else if(strcmp(choice, "q") == 0){
				status = vl53l5cx_stop_ranging(&Dev);
				vl53l5cx_comms_close(&Dev.platform);

		}
		else{
				printf("Invalid command.\n");

		}

	}//while loop



	printf("End of ULD demo\n");
	return status;
}

//must setup address first.
static uint8_t fetchDistance(void){
	uint8_t 				status, isReady, i;
	VL53L5CX_ResultsData 	Results;		/* Results data from VL53L5CX */
	int loop = 0;
	while(loop < 3){
		status = vl53l5cx_check_data_ready(&Dev, &isReady);
		if(status){
			printf("vl53l5cx_check_data_ready failed, error=[%d]\n", status);
			return -1;
		}
		if(isReady)
		{
			status = vl53l5cx_get_ranging_data(&Dev, &Results);
			if(status){
				printf("vl53l5cx_get_ranging_data failed, error=[%d]\n", status);
				return -1;
			}
			/* As the sensor is set in 4x4 mode by default, we have a total 
			* of 16 zones to print. For this example, only the data of first zone are 
			* print */
			printf("Print data num : %3u\n", Dev.streamcount);
			for(i = 8; i < 10; i++)
			{
				printf("Zone : %3d, Status : %3u, Distance : %4d mm\n",
					i,
					Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
					Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
			}
			loop++;
		}
		WaitMs(&Dev.platform, 5);
	}//end while
	return 0;
}

static uint8_t singleTest(void){
	uint8_t 				status, isAlive;
	//VL53L5CX_ResultsData 	Results;		/* Results data from VL53L5CX */
	Dev.platform.address = i2c_address;

	status = vl53l5cx_comms_init(&Dev.platform, device, i2c_address);

	if(status)
	{
		printf("VL53L5CX comms init dev failed\n");
		return status;
	}

	/* (Optional) Check if there is a VL53L5CX sensor connected */
#if(1)	
	status = vl53l5cx_is_alive(&Dev, &isAlive);
	if(!isAlive || status)
	{
		printf("VL53L5CX not detected at requested address\n");
		return status;
	}
#endif
	printf("Start to vl53l5cx_init.\n");
	/* (Mandatory) Init VL53L5CX sensor */
	status = vl53l5cx_init(&Dev);
	if(status)
	{
		printf("VL53L5CX ULD Loading Dev failed\n");
		return status;
	}
	printf("vl53l5cx_init OK.\n");


	status = vl53l5cx_set_ranging_frequency_hz(&Dev, 60);
	if (status) {
		printf("vl53l5cx set frequency fail \r\n");
		return status;
	}

	printf("Start to vl53l5cx_start_ranging.\n");
	
	status = vl53l5cx_start_ranging(&Dev);
	if(status){
		printf("VL53L5CX ULD vl53l5cx_start_ranging failed, error=[%d]\n", status);
		return -1;
	}
	printf("vl53l5cx_start_ranging Dev OK!\n");

	return fetchDistance();

}//singleTest
