
#include "Thread.h"

/**
	HILO DE PRUEBA.

**/

static osThreadId_t id_Th_Thread;

int init_Th_Thread(void);

static void Th_Thread(void *arg);



int init_Th_Thread(void){
	id_Th_Thread = osThreadNew(Th_Thread, NULL, NULL);
   init_Th_com_rx();
   init_Th_com_tx();
	if(id_Th_Thread == NULL)
		return(-1);
  
  return 0;
}


static void Th_Thread(void *argument){ // funcion del hilo
	
  while(1){
		
	
		
  }

}


