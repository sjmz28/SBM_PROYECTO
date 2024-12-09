#ifndef COM_H
#define COM_H

#include "cmsis_os2.h"   
#include "Driver_USART.h"

typedef struct{
	uint16_t pito;
} MSGQUEUE_OBJ_COM_RX;

typedef struct{
	uint16_t pito;
} MSGQUEUE_OBJ_COM_TX;


int init_Th_com(void);
osThreadId_t get_id_Th_com(void);
osMessageQueueId_t get_id_MsgQueue_com_rx(void);
osMessageQueueId_t get_id_MsgQueue_com_tx(void);

#endif
