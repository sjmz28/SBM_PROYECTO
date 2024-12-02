#ifndef __ACE_H
#define __ACE_H

#include "cmsis_os2.h"

typedef struct{
	float ox;
	float oy;	
	float oz;
	float temp;
	
} MSGQUEUE_OBJ_ACE;

int init_Th_Thread(void);
osMessageQueueId_t get_id_MsgQueue_ace(void);

#endif
