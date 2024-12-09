#include "cmsis_os2.h"        //Para manejar el RTOS
#include "stm32f4xx_hal.h"    //Para manejar el HAL

#include "led.h"


static uint32_t flags;

static osThreadId_t id_Th_led;

int init_Th_led(void);
//osMessageQueueId_t id_MsgQueue_led;

static void Th_led(void *arg);
static void led_init(void);
//static MSGQUEUE_OBJ_LED msg;

//static int Init_MsgQueue_Thread(void){
//	id_MsgQueue_led = osMessageQueueNew(16, sizeof(MSGQUEUE_OBJ_LED), NULL);
//	if(id_MsgQueue_led == NULL)
//		return(-1);
	
//	return (0);
//}

//osMessageQueueId_t get_id_MsgQueue_led(void){
//	return id_MsgQueue_led;
//}

int init_Th_led(void){
  led_init();
	id_Th_led = osThreadNew(Th_led, NULL, NULL);
  
	if(id_Th_led == NULL){
		return(-1);
	}
	
  return 0;
}

osThreadId_t get_id_Th_led(void){
 return id_Th_led;
}

static void Th_led(void *argument){ // funcion del hilo
	
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
 
  while(1){
	flags = osThreadFlagsWait(LED1 | LED2 | LED3, osFlagsWaitAny, osWaitForever);
		
		if(flags&LED1){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	} //else
  	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		
		if(flags&LED2){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	} else 
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);

	
		if(flags&LED3){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	}else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	
	}
}

static void led_init(void){  //Inicializar parametros el LED
  
  static GPIO_InitTypeDef GPIO_InitStruct;
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0| GPIO_PIN_7 | GPIO_PIN_14, GPIO_PIN_RESET);
  
}
