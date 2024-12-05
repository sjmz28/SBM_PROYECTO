
#include "Thread.h"

/**
	HILO DE PRUEBA.
	va a esta enviando lineas de texto para representar 
**/

static osThreadId_t id_Th_Thread;

static MSGQUEUE_OBJ_ACE msg_lcd;
int init_Th_Thread(void);
 
static void Th_Thread(void *arg);

static osTimerId_t timer;
static MSGQUEUE_OBJ_ACE msg_lcd;

static uint8_t cnt;

int init_Th_Thread(void){
	id_Th_Thread = osThreadNew(Th_Thread, NULL, NULL);
  
	if(id_Th_Thread == NULL)
		return(-1);
  
  return 0;
}


static void Th_Thread(void *argument){ // funcion del hilo

	while(1){

	}

}


