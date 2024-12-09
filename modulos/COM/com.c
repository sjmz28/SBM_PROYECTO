
#include "com.h"

/**
 MODULO COM-PC

**/

static osThreadId_t id_Th_com;

int init_Th_com(void);
osThreadId_t get_id_Th_com(void);


static int init_MsgQueue_com_rx(void);
static osMessageQueueId_t id_MsgQueue_com_rx;
osMessageQueueId_t get_id_MsgQueue_com_rx(void);

static int init_MsgQueue_com_tx(void);
static osMessageQueueId_t id_MsgQueue_com_tx;
osMessageQueueId_t get_id_MsgQueue_com_tx(void);


static void Th_com(void *arg);
/* HILOS DE RECEPCION Y DE ENVIO */
static int init_MsgQueue_com_rx(void){
	id_MsgQueue_com_rx = osMessageQueueNew(16, sizeof(MSGQUEUE_OBJ_COM_RX), NULL);
	if(id_MsgQueue_com_rx == NULL)
		return(-1);
	return (0);
}
osMessageQueueId_t get_id_MsgQueue_com_rx(void){
return id_MsgQueue_com_rx;
}

static int init_MsgQueue_com_tx(void){
	id_MsgQueue_com_tx = osMessageQueueNew(16, sizeof(MSGQUEUE_OBJ_COM_TX), NULL);
	if(id_MsgQueue_com_tx == NULL)
		return(-1);
	return (0);
}
osMessageQueueId_t get_id_MsgQueue_com_tx(void){
return id_MsgQueue_com_tx;
}

int init_Th_com(void){
	id_Th_com = osThreadNew(Th_com, NULL, NULL);
  
	if(id_Th_com == NULL)
		return(-1);
  
  return 0;
}


static void Th_com(void *argument){ // funcion del hilo
	
  while(1){
		
	
		
  }

}


