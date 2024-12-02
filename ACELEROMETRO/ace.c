#include "cmsis_os2.h"        //Para manejar el RTOS
#include "stm32f4xx_hal.h"    //Para manejar el HAL

#include "ace.h"

static osThreadId_t id_Th_ace;
static osMessageQueueId_t id_MsgQueue_ace;

int init_Th_ace(void);
static int Init_MsgQueue_ace(void);
osThreadId_t get_id_Th_ace(void);
osMessageQueueId_t get_id_MsgQueue_ace(void);

static void Th_ace(void *arg);

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
	id_Th_ace = osThreadNew(Th_ace, NULL, NULL);
  
	if(id_Th_ace == NULL)
		return(-1);
  
	return(Init_MsgQueue_ace());
}

osThreadId_t get_id_Th_ace(void){
return id_Th_ace;
}

static void Th_ace(void *argument){ 
  
  while(1){

  }

}