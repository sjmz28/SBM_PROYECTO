
#include "Thread.h"

/**
	HILO DE PRUEBA.
	va a esta enviando lineas de texto para representar 
**/

static osThreadId_t id_Th_Thread;

int init_Th_Thread(void);
 
static void Th_Thread(void *arg);

static MSGQUEUE_OBJ_ACE msg_ace;


int init_Th_Thread(void){
	id_Th_Thread = osThreadNew(Th_Thread, NULL, NULL);
  
	if(id_Th_Thread == NULL)
		return(-1);
  
  return 0;
}


static void Th_Thread(void *argument){ // funcion del hilo
	osStatus_t status;
	while(1){
		status = osMessageQueueGet(get_id_MsgQueue_ace(), &msg_ace, NULL, 100);
		if(status==osOK){
				msg_ace=msg_ace; // para poder poner el breakpoint
		}
  }



}




