#ifndef COM_H
#define COM_H

#include "cmsis_os2.h"   
#include "Driver_USART.h"

typedef struct {
    uint8_t SOH_type;
    uint8_t CMD;
    uint8_t LEN;
    char payload[50]; // Asumiendo un tamaño máximo para simplificar
    uint8_t payOK;
    uint8_t EOT_type;
} MSGQUEUE_OBJ_COM;

#define FLAG_USART 0x02

int init_Th_com(void);
osMessageQueueId_t get_id_MsgQueue_com_rx(void);
osMessageQueueId_t get_id_MsgQueue_com_tx(void);

// estructura de trama
#define SOH 0x01
#define EOT 0xFE

#endif
