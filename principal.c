#include "cmsis_os2.h"        //Para manejar el RTOS
#include "stm32f4xx_hal.h"    //Para manejar el HAL

#include "principal.h"

/*

MODULO Principal:

*/

extern uint8_t hor;                                  
extern uint8_t min;
extern uint8_t seg;

float ox_ref=0;
float oy_ref=0;
float oz_ref=1;

MSGQUEUE_OBJ_ACE msg_ace_main;
MSGQUEUE_OBJ_JOY msg_joy_main;
MSGQUEUE_OBJ_LCD msg_lcd_main;
MSGQUEUE_OBJ_LED msg_led_main;

typedef enum {
    REPOSO,     
    ACTIVO,
		PROG
} tipo_estado;
tipo_estado estado;

static osThreadId_t id_Th_principal;

int init_Th_principal(void);
static void Th_principal(void *arg);



int init_Th_principal(void){
	init_Th_ace();
	Init_Th_hora();
	init_Th_joy();
	init_Th_lcd();
	init_Th_led();
	
	id_Th_principal = osThreadNew(Th_principal, NULL, NULL);
  
	if(id_Th_principal == NULL)
		return(-1);
  
	return(0);
}

static void Th_principal(void *argument){
	tipo_estado estado=REPOSO;
	
 
  while(1){
		
		switch(estado){
		
			case REPOSO: //CHECK
				sprintf(msg_lcd_main.linea1, "      SBM 2024");
				sprintf(msg_lcd_main.linea2, "       %.2u:%.2u:%.2u ", hor, min, seg);
			  osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd_main, NULL, 0U);
			
				if((osMessageQueueGet(get_id_MsgQueue_joy(), &msg_joy_main, NULL, 100U) == osOK)){
						if((msg_joy_main.dir == MIDDLE) && (msg_joy_main.dur == 1)){
							estado = ACTIVO;
						}
				}
				break;
			
			case ACTIVO: // CHECK, Queda el buffer circular
				if((osMessageQueueGet(get_id_MsgQueue_ace(), &msg_ace_main, NULL, 100U) == osOK)){
					
					msg_led_main.led=0;
					if(msg_ace_main.oz > oz_ref) msg_led_main.led |= (1 << 2);
					if(msg_ace_main.oy > oy_ref) msg_led_main.led |= (1 << 1);
					if(msg_ace_main.ox > ox_ref) msg_led_main.led |= (1 << 0);
					osMessageQueuePut(get_id_MsgQueue_led(), &msg_led_main, NULL, 0U);
					
					sprintf(msg_lcd_main.linea1, "   ACTIVO -- T:%.1f^", msg_ace_main.temp);
					sprintf(msg_lcd_main.linea2, "   X:%.1f Y:%.1f Z:%.1f",msg_ace_main.ox, msg_ace_main.oy, msg_ace_main.oz);
			    osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd_main, NULL, 0U);
				}
				break;
			
			case PROG:
				break;
		}
		
		
		
		
	}
}


