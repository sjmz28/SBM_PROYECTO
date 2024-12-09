@ -1,6 +1,8 @@
#include "cmsis_os2.h"        //Para manejar el RTOS
#include "stm32f4xx_hal.h"    //Para manejar el HAL
#include "Driver_I2C.h"
#include <stdio.h>
#include <math.h>

#include "ace.h"

@ -45,6 +47,16 @@ static int16_t accel_x;
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


@ -126,21 +138,42 @@ static void Th_ace(void *argument){
	 accel_z = (int16_t)((accel_data[4] << 8) | accel_data[5]); // Z

	// Convertir a unidades f?sicas en 'g' (para ?2g)
	 msg_ace.ox = accel_x / 16384.0f;
	 msg_ace.oy = accel_y / 16384.0f;
	 msg_ace.oz = accel_z / 16384.0f;
	 ox = accel_x / 16384.0f;
	 oy = accel_y / 16384.0f;
	 oz = accel_z / 16384.0f;
			
	// Combinar los bytes en un valor de 16 bits con signo
	 temp_raw = (int16_t)((accel_data[6] << 8) | accel_data[7]);

	// Convertir el valor crudo a grados Celsius
	 msg_ace.temp = (temp_raw / 340.0f) + 36.53f;
	 temp = (temp_raw / 340.0f) + 36.53f;
		
	// Enviar por cola
	
	osMessageQueuePut(get_id_MsgQueue_ace(), &msg_ace, 0U, 0U);

	/* Para que no se sature la cola con valores iguales, voy a comparar en cada lectura el valor
			leido con el valor acual, de manera que solo se guarda en la cola los valores nuevos, ademas, 
			dado que varia mucho hasta los decimales (posiblemente por la calidad de mis cables) los paso
			con un decimal de resolucion que es lo que se va observar 
	*/
	ox   =  floor(ox * 10)  / 10.0;
	oy   =  floor(oy * 10)  / 10.0;
	oz   =  floor(oz * 10) / 10.0;
	temp =  floor(temp * 10) / 10.0;
	
  
	if(fabs(ox-ox_actual)>=0.1 || fabs(oy-oy_actual)>=0.1 || fabs(oz-oz_actual)>=0.1 || fabs(temp-temp_actual)>=0.1){
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
