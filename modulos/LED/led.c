#include "cmsis_os2.h"        //Para manejar el RTOS
#include "stm32f4xx_hal.h"    //Para manejar el HAL

#include "led.h"


static uint32_t flags;

static osThreadId_t id_Th_led;

int init_Th_led(void);


static void Th_led(void *arg);
static void led_init(void);


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
 
  while(1){
	flags = osThreadFlagsWait(LED1 | LED2 | LED3 | nLED1 | nLED2 | nLED3, osFlagsWaitAny, osWaitForever);
		
		if(flags&LED1){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	} 
		if(flags&LED2){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	}
		if(flags&LED3){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	}
		if(flags&nLED1){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	} 
		if(flags&nLED2){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	}
		if(flags&nLED3){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	}
		
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
