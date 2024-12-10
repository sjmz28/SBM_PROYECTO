#include "cmsis_os2.h"        //Para manejar el RTOS
#include "stm32f4xx_hal.h"    //Para manejar el HAL

#include "principal.h"

/*

MODULO Principal:

*/

extern uint8_t hor;                                  
extern uint8_t min;
extern uint8_t seg;
extern uint8_t hab_reloj;

static uint8_t aux_hor;
static uint8_t aux_min;
static uint8_t aux_seg;

static	uint8_t cnt_prg;
static	uint8_t cnt_hora;
static	uint8_t cnt_ace;

float aux_ox_ref;
float aux_oy_ref;
float aux_oz_ref;

float ox_ref=0;
float oy_ref=0;
float oz_ref=1;

MSGQUEUE_OBJ_ACE msg_ace_main;
MSGQUEUE_OBJ_JOY msg_joy_main;
MSGQUEUE_OBJ_LCD msg_lcd_main;

buf_medidas buffer_medidas;

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
	
	cnt_prg=0;
	cnt_hora=0;
	cnt_ace=0;
	

	
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
				sprintf(msg_lcd_main.linea1, " + + SBM 2024 + +");
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
					
					sprintf(msg_lcd_main.linea1, "   ACTIVO -- T:%.1f ^", msg_ace_main.temp);
					sprintf(msg_lcd_main.linea2, "   X:%.1f Y:%.1f Z:%.1f",msg_ace_main.ox, msg_ace_main.oy, msg_ace_main.oz);
			    osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd_main, NULL, 0U);
					
					
					if(msg_ace_main.oz > oz_ref){
					osThreadFlagsSet(get_id_Th_led(), LED1);
					} else{
					osThreadFlagsSet(get_id_Th_led(), nLED1);
					}
					
					if(msg_ace_main.oy > oy_ref){
					osThreadFlagsSet(get_id_Th_led(), LED2);
					} else{
					osThreadFlagsSet(get_id_Th_led(), nLED2);
					}
					
					if(msg_ace_main.ox > ox_ref){
					osThreadFlagsSet(get_id_Th_led(), LED3);
					} else{
					osThreadFlagsSet(get_id_Th_led(), nLED3);
					}  
					
					
				}
				if((osMessageQueueGet(get_id_MsgQueue_joy(), &msg_joy_main, NULL, 100U) == osOK)){
						if((msg_joy_main.dir == MIDDLE) && (msg_joy_main.dur == 1)){
							
							hab_reloj=0;
							aux_hor=hor;
							aux_min=min;
							aux_seg=seg;
							
							aux_ox_ref=ox_ref;
							aux_oy_ref=oy_ref;
							aux_oz_ref=oz_ref;
							
							estado = PROG;
						}
				}
				break;
			
			case PROG:
				
			if(!cnt_prg){
				sprintf(msg_lcd_main.linea1, "       ---P&D---");
				sprintf(msg_lcd_main.linea2, "       %.2u:%.2u:%.2u ", aux_hor, aux_min, aux_seg);
			  osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd_main, NULL, 0U);
			}else{
				sprintf(msg_lcd_main.linea1, "       ---P&D---");
				sprintf(msg_lcd_main.linea2, "   X:%.1f Y:%.1f Z:%.1f",aux_ox_ref, aux_oy_ref, aux_oz_ref);
			  osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd_main, NULL, 0U);
			}

			
			
			if((osMessageQueueGet(get_id_MsgQueue_joy(), &msg_joy_main, NULL, 100U) == osOK)){

/////////////////////////////////Boton MIDDLE /////////////////////////////////////				
				if((msg_joy_main.dir == MIDDLE) && (msg_joy_main.dur == 0)){ 
					if(!cnt_prg){
					hor=aux_hor;
					min=aux_min;
					seg=aux_seg;
					}else{
					ox_ref=aux_ox_ref;
					oy_ref=aux_oy_ref;
					oz_ref=aux_oz_ref;
					}
					cnt_prg = cnt_prg==0 ? 1 : 0;	
					}
/////////////////////////////////////////////////////////////////////////////////////////
					
					/////// HORA ////////
		 if(cnt_prg==0){	
///////////////////////////// Boton RIGHT(hora) ////////////////////////////////////////////				
		
				if((msg_joy_main.dir == RIGHT) && (msg_joy_main.dur == 0)){ 
					
					cnt_hora += cnt_hora!=3;
					cnt_hora *= cnt_hora!=3;
					
					}
			
//////////////////////////////////////////////////////////////////////////////////////////////
				
///////////////////////////// Boton LEFT(hora) ////////////////////////////////////////////				
			
				if((msg_joy_main.dir == LEFT) && (msg_joy_main.dur == 0)){ 
							
					if(cnt_hora!=0){
					cnt_hora--;
					}else{
					cnt_hora=2;
					}
					
					}
				
//////////////////////////////////////////////////////////////////////////////////////////////
					
//////////////////////////// Boton UP(hora) //////////////////////////////////////////////////				
			
				if(cnt_hora==0){
					
						if((msg_joy_main.dir == UP) && (msg_joy_main.dur == 0)){ 
							
							aux_hor += aux_hor != 24;
							aux_hor *= aux_hor != 24;
							
						}	
					}	
				
				if (cnt_hora==1){
						
						if((msg_joy_main.dir == UP) && (msg_joy_main.dur == 0)){ 
							
							aux_min += aux_min != 60;
							aux_min *= aux_min != 60;
							
						}	
					}	
				if (cnt_hora==2){
						
						if((msg_joy_main.dir == UP) && (msg_joy_main.dur == 0)){ 
							
							aux_seg += aux_seg != 60;
							aux_seg *= aux_seg != 60;
							
						}	
					}
									
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////// Boton DOWN (hora) //////////////////////////////////////////////////				
			
				if(cnt_hora==0){
					
						if((msg_joy_main.dir == DOWN) && (msg_joy_main.dur == 0)){
							
				  if(aux_hor!=0){
					aux_hor--;
					}else{
				  aux_hor=23;
					}
							
							
						}	
					}	else if (cnt_hora==1){
						
						if((msg_joy_main.dir == DOWN) && (msg_joy_main.dur == 0)){ 
							
				  if(aux_min!=0){
					aux_min--;
					}else{
					aux_min=59;
					}
							
						}	
					}	else if (cnt_hora==2){
						
						if((msg_joy_main.dir == DOWN) && (msg_joy_main.dur == 0)){ 
							
				  if(aux_seg!=0){
					aux_seg--;
					}else{
					aux_seg=59;
					}
							
						}	
					}	
									
//////////////////////////////////////////////////////////////////////////////////////////////////////					
				}
				//////////////// HORA ////////////////////////////
				
				//////////////// ACE ////////////////////////////
				
			if(cnt_prg==1){		
				
/////////////////////////////// Boton RIGHT (ace) //////////////////////////////////////////////////////
				if((msg_joy_main.dir == RIGHT) && (msg_joy_main.dur == 0)){ 
							
					cnt_ace += cnt_ace!=3;
					cnt_ace *= cnt_ace!=3;
							
				}
////////////////////////////////////////////////////////////////////////////////////////////////////////
				
/////////////////////////////// Boton LEFT (ace) //////////////////////////////////////////////////////
				if((msg_joy_main.dir == LEFT) && (msg_joy_main.dur == 0)){ 
							
					if(cnt_ace!=0){
					cnt_ace--;
					}else{
					cnt_ace=2;
					}
							
				}
////////////////////////////////////////////////////////////////////////////////////////////////////////
				
//////////////////////////////// Boton UP (ace) ////////////////////////////////////////////////////////
							
				if(cnt_ace==0){
					
						if((msg_joy_main.dir == UP) && (msg_joy_main.dur == 0)){ 
							
							aux_ox_ref = (aux_ox_ref<2) ? aux_ox_ref+0.1f : -2; 
							
						}	
					}
				
				if (cnt_ace==1){
						
						if((msg_joy_main.dir == UP) && (msg_joy_main.dur == 0)){ 
							
						 aux_oy_ref = (aux_oy_ref<2) ? aux_oy_ref+0.1f : -2; 
							
						}	
					}
				
				if (cnt_ace==2){
						
						if((msg_joy_main.dir == UP) && (msg_joy_main.dur == 0)){ 
							
						aux_oz_ref = (aux_oz_ref<2) ? aux_oz_ref+0.1f : -2; 
							
						}	
					}
					
/////////////////////////////////////////////////////////////////////////////////////////////////////
				
//////////////////////////////// Boton DOWN (ace) ////////////////////////////////////////////////////////
				

				if(cnt_ace==0){
					
						if((msg_joy_main.dir == DOWN) && (msg_joy_main.dur == 0)){ 
							
							aux_ox_ref = (aux_ox_ref>-2) ? aux_ox_ref-0.1f : 2; 
							
						}	
					}	
				
				if (cnt_ace==1){
						
						if((msg_joy_main.dir == DOWN) && (msg_joy_main.dur == 0)){ 
							
						 aux_oy_ref = (aux_oy_ref>-2) ? aux_oy_ref-0.1f : -2; 
							
						}	
					}	 
				
				if (cnt_ace==2){
						
						if((msg_joy_main.dir == DOWN) && (msg_joy_main.dur == 0)){ 
							
						aux_oz_ref = (aux_oz_ref>-2) ? aux_oz_ref-0.1f : -2; 
							
						}	
					}
				
/////////////////////////////////////////////////////////////////////////////////////////////////////
				}
				/////////////////////////////////////////////////
				
/////////////////////////////////// CAMBIO DE ESTADO /////////////////////////////////////////////////					
						if((msg_joy_main.dir == MIDDLE) && (msg_joy_main.dur == 1)){ //cambiar de estado
							estado = REPOSO;
							hab_reloj=1;
						}
//////////////////////////////////////////////////////////////////////////////////////////////////////
		}
			break;
		
		
	}
}
}
