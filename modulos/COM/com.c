
#include "com.h"
#define FLAG_USART 0x02

// estructura de trama
#define SOH 0x01
#define EOT 0xFE

extern ARM_DRIVER_USART Driver_USART3;
static ARM_DRIVER_USART *USARTdrv = &Driver_USART3;

/**
 MODULO COM-PC

**/
// HILOS
static osThreadId_t id_Th_com_rx;
static osThreadId_t id_Th_com_tx;


osThreadId_t get_id_Th_com_rx(void);
static void Th_com_rx(void *arg);

osThreadId_t get_id_Th_com_tx(void);
static void Th_com_tx(void *arg);

int init_Th_com_rx(void);
int init_Th_com_tx(void);



//COLAS
static int init_MsgQueue_com_rx(void);
static osMessageQueueId_t id_MsgQueue_com_rx;
osMessageQueueId_t get_id_MsgQueue_com_rx(void);

static int init_MsgQueue_com_tx(void);
static osMessageQueueId_t id_MsgQueue_com_tx;
osMessageQueueId_t get_id_MsgQueue_com_tx(void);

//USART
static void init_USART(void);
static void CallBack_USART3(uint32_t event);

static MSGQUEUE_OBJ_COM msg_com_rx;
static MSGQUEUE_OBJ_COM msg_com_tx;

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
		osThreadFlagsSet(get_id_Th_com_rx(),FLAG_USART);
    
	if (event &  mask)
		osThreadFlagsSet(get_id_Th_com_tx(),FLAG_USART);
	
}


/* COLAS DE RECEPCION Y DE ENVIO */
static int init_MsgQueue_com_rx(void){
	id_MsgQueue_com_rx = osMessageQueueNew(16, sizeof(MSGQUEUE_OBJ_COM), NULL);
	if(id_MsgQueue_com_rx == NULL)
		return(-1);
	return (0);
}
osMessageQueueId_t get_id_MsgQueue_com_rx(void){
return id_MsgQueue_com_rx;
}

static int init_MsgQueue_com_tx(void){
	id_MsgQueue_com_tx = osMessageQueueNew(16, sizeof(MSGQUEUE_OBJ_COM), NULL);
	if(id_MsgQueue_com_tx == NULL)
		return(-1);
	return (0);
}
osMessageQueueId_t get_id_MsgQueue_com_tx(void){
return id_MsgQueue_com_tx;
}


/*   HILO DE RECEPCION  */

int init_Th_com_rx(void){
	id_Th_com_rx = osThreadNew(Th_com_rx, NULL, NULL);
  init_USART();
	if(id_Th_com_rx == NULL)
		return(-1);
  
  return init_MsgQueue_com_rx();
}

osThreadId_t get_id_Th_com_rx(void){
	return id_Th_com_rx;
}

static void Th_com_rx(void *argument){ // funcion del hilo
	uint8_t byte;
	uint8_t i=0;
	
	msg_com_rx.SOH_byte=0;
	msg_com_rx.CMD=0;
	msg_com_rx.LEN=0;
	msg_com_rx.payOK=0;
	msg_com_rx.EOT_byte=0;
	
  while(1){
		USARTdrv->Receive(&byte, 1);

		osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever); // mirar el osWaitForever
		
		if(byte == SOH){	// si recivo una incicializacion 
			if(msg_com_rx.SOH_byte == 0 && msg_com_rx.CMD == 0 && msg_com_rx.LEN ==0 &&  msg_com_rx.payOK==0 && msg_com_rx.EOT_byte==0){ // y no estoy inicializado
          msg_com_rx.SOH_byte = SOH;   																																														//me inicializo
			} 
		}
		else if(msg_com_rx.SOH_byte != 0 && msg_com_rx.CMD == 0 && msg_com_rx.LEN ==0 &&  msg_com_rx.payOK==0 && msg_com_rx.EOT_byte==0){ // si tengo solo el SOH
			msg_com_rx.CMD=byte;																																																		 // sera que el siguiente es el CMD
		}
		else if(msg_com_rx.SOH_byte != 0 && msg_com_rx.CMD != 0 && msg_com_rx.LEN ==0 &&  msg_com_rx.payOK==0 && msg_com_rx.EOT_byte==0){ // sitengo SOH, CMD y nada mas
			msg_com_rx.LEN=byte;																																																		 // sera que el siguiente es el LEN
		}
		else if(msg_com_rx.SOH_byte != 0 && msg_com_rx.CMD != 0 && msg_com_rx.LEN !=0 &&  msg_com_rx.payOK==0 && msg_com_rx.EOT_byte==0){ // si tengo SOH, CMD, LEN y nada mas, es que me viene el PAYLOAD
			if(byte!=EOT){ 																																																				// mientras no reciba un fin de trama sigo reciviendo payload
				msg_com_rx.payload[i]=byte;
				i++;
			}
			else{																																																								// ya ha terminado con el payload, finalizo la trama
				msg_com_rx.payOK=1;
				msg_com_rx.EOT_byte=EOT;
				osMessageQueuePut(get_id_MsgQueue_com_rx(), &msg_com_rx, NULL, 0U);
				// reseteamos la variable
				msg_com_rx.SOH_byte=0;
				msg_com_rx.CMD=0;
				msg_com_rx.LEN=0;
				msg_com_rx.payOK=0;
				msg_com_rx.EOT_byte=0;
			}
		}
  }

}

/*   HILO DE TRANSMISION  */


int init_Th_com_tx(void){
	id_Th_com_tx = osThreadNew(Th_com_tx, NULL, NULL);
  
	if(id_Th_com_tx == NULL)
		return(-1);
  
  return init_MsgQueue_com_tx();
}

osThreadId_t get_id_Th_com_tx(void){
	return id_Th_com_tx;
}

static void Th_com_tx(void *argument){ 
		int i;
		while(1){
			if((osMessageQueueGet(get_id_MsgQueue_com_tx(), &msg_com_tx, NULL, 100U)==osOK)){
				USARTdrv->Send(&msg_com_tx.SOH_byte,1);
				osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
				
				USARTdrv->Send(&msg_com_tx.CMD,1);
				osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
				
				USARTdrv->Send(&msg_com_tx.LEN,1);
				osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
				
				for(i=0; i<=msg_com_tx.LEN-3 ; i++){
					USARTdrv->Send(&msg_com_tx.payload[i],1);
					osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
				}
				
				USARTdrv->Send(&msg_com_tx.EOT_byte,1);
				osThreadFlagsWait(FLAG_USART, osFlagsWaitAny, osWaitForever);
				
			}
		}
}




