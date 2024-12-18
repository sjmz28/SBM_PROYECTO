
#include "Thread.h"

/**
	HILO DE PRUEBA.

**/
uint8_t leds;

static osThreadId_t id_Th_Thread;

int init_Th_Thread(void);

static void Th_Thread(void *arg);

static osTimerId_t timer;

int init_Th_Thread(void){
	id_Th_Thread = osThreadNew(Th_Thread, NULL, NULL);
  
	if(id_Th_Thread == NULL)
		return(-1);
  
  return 0;
}

static void timer_Callback(void* argument){  

	leds += leds!=20;
	leds *= leds<20;
	
	
}

static void Th_Thread(void *argument){ // funcion del hilo
  leds=0;
	
	timer = osTimerNew(timer_Callback, osTimerPeriodic, (void *)0, NULL);
	
	osTimerStart(timer,1000U);
	
  while(1){
		
		if(leds>3){
		osThreadFlagsSet(get_id_Th_led(), LED1);
		} else{
		osThreadFlagsSet(get_id_Th_led(), nLED1);
		}
		
		
		if(leds>8){
		osThreadFlagsSet(get_id_Th_led(), LED2);
		} else {
	  osThreadFlagsSet(get_id_Th_led(), nLED2);
		}
		
	  if(leds>14){
		osThreadFlagsSet(get_id_Th_led(), LED3);
		} else {
	  osThreadFlagsSet(get_id_Th_led(), nLED3);
		}
		
  }

}


