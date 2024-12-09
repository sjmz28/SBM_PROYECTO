
#include "Thread.h"

/**
	HILO DE PRUEBA.

**/

static osThreadId_t id_Th_Thread;

int init_Th_Thread(void);

static void Th_Thread(void *arg);

static osTimerId_t timer;
static MSGQUEUE_OBJ_LED msg_led;

int init_Th_Thread(void){
	id_Th_Thread = osThreadNew(Th_Thread, NULL, NULL);
  
	if(id_Th_Thread == NULL)
		return(-1);
  
  return 0;
}

static void timer_Callback(void* argument){  
	  msg_led.led++;
}

static void Th_Thread(void *argument){ // funcion del hilo
	osStatus_t status;
	msg_led.led=0;
	timer = osTimerNew(timer_Callback, osTimerPeriodic,  (void*)0, NULL); 
	osTimerStart(timer, 1000U);
  while(1){
		
		status = osMessageQueuePut(get_id_MsgQueue_led(), &msg_led, NULL, 500);
		
  }

}


