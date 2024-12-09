#include "cmsis_os2.h"        //Para manejar el RTOS
#include "stm32f4xx_hal.h"    //Para manejar el HAL
#include "Driver_I2C.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "ace.h"

#define I2C 0x01
#define TIM 0x02


/**
nota: las medidas se realizarán cada segundo de ambas medidas
REGISTROS IMPORTANTES:

direccion I2C esclavo -- 0x68 ( cuando esta conectado el pin AD0 a tierra y 0x69 si esta conectado a VCC

OX  - 0x3B [15:8] 0x3C[7:0]
OY  - 0x3D [15:8] 0x3E[7:0]
OZ  - 0x3F [15:8] 0x40[7:0]

TEM - 0x41 [15:8] 0x42[7:0]

**/

static osThreadId_t id_Th_ace;
static osMessageQueueId_t id_MsgQueue_ace;

int init_Th_ace(void);
static int Init_MsgQueue_ace(void);
osThreadId_t get_id_Th_ace(void);
osMessageQueueId_t get_id_MsgQueue_ace(void);
static void init_i2c(void);

static void Th_ace(void *arg);

static MSGQUEUE_OBJ_ACE msg_ace;

// variable I2C
extern ARM_DRIVER_I2C Driver_I2C1;
ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;

osTimerId_t tim_1seg;

static int16_t accel_x;
static int16_t accel_y;
static int16_t accel_z;

static float ox=0;
static float oy=0;
static float oz=0;
static float temp=0;

static float ox_actual=0;
static float oy_actual=0;
static float oz_actual=0;
static float temp_actual=0;



static int16_t temp_raw;


static int Init_MsgQueue_ace(void){
	id_MsgQueue_ace = osMessageQueueNew(16, sizeof(MSGQUEUE_OBJ_ACE), NULL);
	if(id_MsgQueue_ace == NULL)
		return(-1);
	
	return (0);
}

osMessageQueueId_t get_id_MsgQueue_ace(void){
	return id_MsgQueue_ace;
}

int init_Th_ace(void){
	init_i2c();
	id_Th_ace = osThreadNew(Th_ace, NULL, NULL);
  
	if(id_Th_ace == NULL)
		return(-1);
  
	return(Init_MsgQueue_ace());
}

static void i2c_Callback(uint32_t event){ 
    osThreadFlagsSet(get_id_Th_ace(), I2C);
}

static void init_i2c(void){
		I2Cdrv ->Initialize(i2c_Callback);     // indicamos la funcion callback para cuando se termine de realizar transacciones
		I2Cdrv ->PowerControl(ARM_POWER_FULL); // por pedir que no falte
		I2Cdrv ->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD); // velocidad del bus
	  I2Cdrv ->Control(ARM_I2C_BUS_CLEAR, 0); //Limpiamos el bus I2C de posibles comunicaciones que se hubieran quedado a medias

		// direccion del escalvo
	  I2Cdrv ->Control(ARM_I2C_OWN_ADDRESS, 0x68);
}

osThreadId_t get_id_Th_ace(void){
return id_Th_ace;
}

static void tim_1seg_Callback(void* argument){  
		 osThreadFlagsSet(get_id_Th_ace(), TIM);
}

static void Th_ace(void *argument){ 
	
	uint8_t reg_pwr_mgmt_1[2] = {0x6B,0x00}; // Dirección del registro PWR_MGMT_1
	uint8_t data[2] = {0x1C, 0x00};// Dirección del registro ACCEL_CONFIG &  Configuración para rango de ±2g
	
	uint8_t dir_x = 0x3B;
	uint8_t accel_data[8];
	
	
	tim_1seg = osTimerNew(tim_1seg_Callback, osTimerOnce, (void*)0, NULL); 
	
	I2Cdrv->MasterTransmit(0x68, reg_pwr_mgmt_1, 2, true); 
	osThreadFlagsWait(I2C, osFlagsWaitAll, osWaitForever);

	I2Cdrv->MasterTransmit(0x68, data, 2, false);  
	osThreadFlagsWait(I2C, osFlagsWaitAll, osWaitForever);
		
		
  while(1){
		
	I2Cdrv->MasterTransmit(0x68, &dir_x, 1, true);
	osThreadFlagsWait(I2C, osFlagsWaitAll, osWaitForever);
	
		
	I2Cdrv->MasterReceive(0x68, accel_data, 8, false);	
	osThreadFlagsWait(I2C, osFlagsWaitAll, osWaitForever);

		
	// Convertir los valores de cada eje a 16 bits
	 accel_x = (int16_t)((accel_data[0] << 8) | accel_data[1]); // X
	 accel_y = (int16_t)((accel_data[2] << 8) | accel_data[3]); // Y
	 accel_z = (int16_t)((accel_data[4] << 8) | accel_data[5]); // Z

	// Convertir a unidades físicas en 'g' (para ±2g)
	 ox = accel_x / 16384.0f;
	 oy = accel_y / 16384.0f;
	 oz = accel_z / 16384.0f;
			
	// Combinar los bytes en un valor de 16 bits con signo
	 temp_raw = (int16_t)((accel_data[6] << 8) | accel_data[7]);

	// Convertir el valor crudo a grados Celsius
	 temp = (temp_raw / 340.0f) + 36.53f;
		
	// Pasarlo a entero
	ox= floor(ox*10)/10;
	oy= floor(oy*10)/10;
	oz= floor(oz*10)/10;
	temp= floor(temp*10)/10;

	
	/* Para que no se sature la cola con valores iguales, voy a comparar en cada lectura el valor
			leido con el valor acual, de manera que solo se guarda en la cola los valores nuevos, ademas, 
			dado que varia mucho hasta los decimales (posiblemente por la calidad de mis cables) los paso
			con un decimal de resolucion que es lo que se va observar 
	*/
  
	if(fabs(ox-ox_actual)>=0.1 || fabs(oy-oy_actual)>=0.1  || fabs(oz-oz_actual)>=0.1  || fabs(temp-temp_actual)>=0.1 ){
		msg_ace.ox=ox;
		msg_ace.oy=oy;
		msg_ace.oz=oz;
		msg_ace.temp=temp;
		
		ox_actual=ox;
		oy_actual=oy;
		oz_actual=oz;
		temp_actual=temp;
		
		osMessageQueuePut(get_id_MsgQueue_ace(), &msg_ace, 0U, 0U);
	}

	// timer de un segundo 
	osTimerStart(tim_1seg, 1000U);
  osThreadFlagsWait(TIM, osFlagsWaitAll, osWaitForever);
	osMessageQueuePut(get_id_MsgQueue_ace(), &msg_ace, 0U, 0U);

	
	}

}
