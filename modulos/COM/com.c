#include "com.h"

extern ARM_DRIVER_USART Driver_USART3;
static ARM_DRIVER_USART *USARTdrv = &Driver_USART3;

osThreadId_t id_Th_com_rx;
osThreadId_t id_Th_com_tx;

osMessageQueueId_t id_MsgQueue_com_tx;
osMessageQueueId_t id_MsgQueue_com_rx;

osMessageQueueId_t get_id_MsgQueue_com_rx(void);
osMessageQueueId_t get_id_MsgQueue_com_tx(void);

MSGQUEUE_OBJ_COM msg_com_RX;
MSGQUEUE_OBJ_COM msg_com_TX;

/*---------FUNCIONES------------*/
static void init_USART(void);
static void CallBack_USART3(uint32_t event);
void Th_com_rx (void *argument); 
void Th_com_tx (void *argument); 
static int Init_MsgQueue_com(void);

// USART //
static void init_USART(void){
		USARTdrv->Initialize(CallBack_USART3); 
	USARTdrv->PowerControl(ARM_POWER_FULL);
	USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                    ARM_USART_DATA_BITS_8 |
                    ARM_USART_PARITY_NONE |
                    ARM_USART_STOP_BITS_1 |
                    ARM_USART_FLOW_CONTROL_NONE, 115200);
	
/* Enable Receiver and Transmitter lines */
	USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
  USARTdrv->Control (ARM_USART_CONTROL_RX, 1);
}
	
static void CallBack_USART3(uint32_t event){
	  uint32_t mask;
  mask = 
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_TX_COMPLETE       ;
  
	if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) 
    /* Success: Wakeup Thread */
		osThreadFlagsSet(id_Th_com_rx,FLAG_USART);
    
	if (event &  mask)
			osThreadFlagsSet(id_Th_com_tx,FLAG_USART);
	
}
// COLAS //
static int Init_MsgQueue_com(void){
	id_MsgQueue_com_rx = osMessageQueueNew(16, sizeof(MSGQUEUE_OBJ_COM), NULL);
  id_MsgQueue_com_tx = osMessageQueueNew(16, sizeof(MSGQUEUE_OBJ_COM), NULL);
	if(id_MsgQueue_com_rx == NULL || id_MsgQueue_com_tx == NULL){
		return(-1);
	}else{
		return (0);
	}
}
osMessageQueueId_t get_id_MsgQueue_com_rx(void){
	return id_MsgQueue_com_rx;
}

osMessageQueueId_t get_id_MsgQueue_com_tx(void){
	return id_MsgQueue_com_tx;
}
// HILOS // 
int init_Th_com (void){
  init_USART();

	id_Th_com_rx = osThreadNew(Th_com_rx, NULL, NULL);
	id_Th_com_tx = osThreadNew(Th_com_tx, NULL, NULL);
	
  if (id_Th_com_rx == NULL || id_Th_com_rx == NULL) {
    return(-1);
  }
	
  return(Init_MsgQueue_com());

}
// HILO RECEPCION //
void Th_com_rx(void* argument){
  uint8_t byte;
  static uint16_t cnt = 0;
  msg_com_RX.CMD=0;
  msg_com_RX.EOT_type=0;
  msg_com_RX.LEN=0;
  msg_com_RX.SOH_type = 0;
  msg_com_RX.payOK=0;
	while(1){
		USARTdrv->Receive(&byte, 1);
		osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
    if(byte==SOH){
        if(msg_com_RX.SOH_type == 0 && msg_com_RX.EOT_type == 0 && msg_com_RX.LEN ==0 && msg_com_RX.CMD ==0 && msg_com_RX.payOK==0){
          msg_com_RX.SOH_type= SOH;    
        } 
    }
    else if(msg_com_RX.SOH_type !=0  && msg_com_RX.EOT_type == 0 && msg_com_RX.LEN ==0 && msg_com_RX.CMD ==0 && msg_com_RX.payOK==0){
          msg_com_RX.CMD= byte;
    }
    else if(msg_com_RX.SOH_type !=0  && msg_com_RX.EOT_type == 0 && msg_com_RX.LEN ==0 && msg_com_RX.CMD !=0 && msg_com_RX.payOK==0){
          msg_com_RX.LEN= byte;
    }
    else if(msg_com_RX.SOH_type !=0  && msg_com_RX.EOT_type == 0 && msg_com_RX.LEN !=0 && msg_com_RX.CMD !=0 && msg_com_RX.payOK==0){
			 if(byte!=EOT){
					msg_com_RX.payload[cnt] = byte;
					cnt++;
			 }
			 else{
					msg_com_RX.payOK=1;
					cnt=0;
					msg_com_RX.EOT_type= EOT;
					osMessageQueuePut(get_id_MsgQueue_com_rx(), &msg_com_RX, NULL, 0U);
					for(int i = 0; i <msg_com_RX.LEN-3; i++){
						msg_com_RX.payload[i] = 0x00;
					}
					msg_com_RX.CMD=0;
					msg_com_RX.EOT_type=0;
					msg_com_RX.LEN=0;
					msg_com_RX.SOH_type = 0;
					msg_com_RX.payOK=0;
						
			 }
     }
	}
}

// HILO TRANSMISION //
void Th_com_tx(void* argument){
  while(1){
    if((osMessageQueueGet(get_id_MsgQueue_com_tx(), &msg_com_TX, NULL, 100U) == osOK)){ // trama nueva
      uint8_t cnt=0;
      USARTdrv->Send(&msg_com_TX.SOH_type, 1);
      osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
      USARTdrv->Send(&msg_com_TX.CMD, 1);
      osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
      USARTdrv->Send(&msg_com_TX.LEN, 1);
      osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
      USARTdrv->Send(&msg_com_TX.LEN, 1);
      osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
      while(cnt<=(msg_com_TX.LEN-3)){
         USARTdrv->Send(&msg_com_TX.payload[cnt], 1);
         osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
         cnt++;
      }
      USARTdrv->Send(&msg_com_TX.EOT_type, 1);
      osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
    
		}
	}
}























