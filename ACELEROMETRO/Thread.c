
#include "Thread.h"

/**
	HILO DE PRUEBA.
	va a esta enviando lineas de texto para representar 
**/

static osThreadId_t id_Th_Thread;

static MSGQUEUE_OBJ_ACE msg_ace;
int init_Th_Thread(void);
 
static void Th_Thread(void *arg);

static osTimerId_t timer;
static MSGQUEUE_OBJ_ACE msg_ace;

static uint8_t cnt;

static int16_t accel_x;
static int16_t accel_y;
static int16_t accel_z;
static int16_t temp_raw;

int init_Th_Thread(void){
	id_Th_Thread = osThreadNew(Th_Thread, NULL, NULL);
  
	if(id_Th_Thread == NULL)
		return(-1);
  
  return 0;
}


static void Th_Thread(void *argument){ // funcion del hilo

  MSGQUEUE_OBJ_ACE msg_ace;
	osStatus_t status;
  while(1){
     status = osMessageQueueGet(get_id_MsgQueue_ace(), &msg_ace, NULL, 100);
		if(status==osOK){
			accel_x = msg_ace.ox;
			accel_y  = msg_ace.oy;
			accel_z  = msg_ace.oz;
			temp_raw  = msg_ace.temp;
		}
  }

}


