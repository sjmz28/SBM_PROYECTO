
#include "Thread.h"
#include <stdio.h>
/**
	HILO DE PRUEBA.
	Muestra en las variables Direccion y Duracion los valores de la pulsacion
**/

static osThreadId_t id_Th_Thread;
static uint8_t cnt=0;


int init_Th_Thread(void);
static void Th_Thread(void *arg);

static MSGQUEUE_OBJ_COM msg_com_rx; 
static MSGQUEUE_OBJ_COM msg_com_tx;
	
int init_Th_Thread(void){ 
	init_Th_com();
	id_Th_Thread = osThreadNew(Th_Thread, NULL, NULL);
 
	if(id_Th_Thread == NULL)
		return(-1);
  
  return 0;
}


static void Th_Thread(void *argument){ // funcion del hilo

	osStatus_t status;
	// while para controlar la recepcion 
	
  while(1){
    status = osMessageQueueGet(get_id_MsgQueue_com_rx(), &msg_com_rx, NULL, 1000U);
		if(status==osOK){
			msg_com_rx=msg_com_rx; // para poder poner el breakpoint
		}
  }
	/*
	// while para controlar la transmision 
	
	msg_com_tx.SOH_type=0x01;
	msg_com_tx.CMD = 0x02; 
	msg_com_tx.LEN = 0x0A;
	msg_com_tx.payOK=1;
	
	msg_com_tx.EOT_type = 0xFE;
	while(1){
			sprintf(msg_com_tx.payload, "prueba %d", cnt);
			osMessageQueuePut(get_id_MsgQueue_com_tx(), &msg_com_tx, NULL, 500);
			cnt++;
			osDelay(3000U);
		}
  */
	
}


