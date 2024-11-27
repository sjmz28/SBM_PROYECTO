
#include "Thread.h"

/**
	HILO DE PRUEBA.
	Muestra en las variables Direccion y Duracion los valores de la pulsacion
**/

static osThreadId_t id_Th_Thread;

int init_Th_Thread(void);

static void Th_Thread(void *arg);

dir_t direccion;
uint8_t duracion;


int init_Th_Thread(void){
	id_Th_Thread = osThreadNew(Th_Thread, NULL, NULL);
  
	if(id_Th_Thread == NULL)
		return(-1);
  
  return 0;
}

static void Th_Thread(void *argument){ // funcion del hilo
  MSGQUEUE_OBJ_JOY msg_joy;
	osStatus_t status;
  while(1){
     status = osMessageQueueGet(get_id_MsgQueue_joy(), &msg_joy, NULL, 100);
		if(status==osOK){
			direccion = msg_joy.dir;
			duracion  = msg_joy.dur;
		}
  }

}


