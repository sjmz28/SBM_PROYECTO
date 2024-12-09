#ifndef COM_H
#define COM_H

#include "cmsis_os2.h"   
#include "Driver_USART.h"

typedef struct{
		uint8_t SOH_byte;
    uint8_t CMD;
    uint8_t LEN;
    char payload[50]; // Asumiendo un tamaño máximo para simplificar
		uint8_t payOK;
    uint8_t EOT_byte;
} MSGQUEUE_OBJ_COM;


int init_Th_com_rx(void);
int init_Th_com_tx(void);
osThreadId_t get_id_Th_com_rx(void);
osThreadId_t get_id_Th_com_tx(void);
osMessageQueueId_t get_id_MsgQueue_com_rx(void);
osMessageQueueId_t get_id_MsgQueue_com_tx(void);

#endif
