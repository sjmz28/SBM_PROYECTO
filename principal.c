#include "cmsis_os2.h"        //Para manejar el RTOS
#include "stm32f4xx_hal.h"    //Para manejar el HAL

#include "principal.h"

/*

MODULO Principal:

*/

static void agregarMedida(buf_medidas* buf);
static void procesarComandosRS232(void);

static uint8_t i;
static uint8_t j;

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

float ox_ref=1;
float oy_ref=1;
float oz_ref=1;

MSGQUEUE_OBJ_ACE msg_ace_main;
MSGQUEUE_OBJ_JOY msg_joy_main;
MSGQUEUE_OBJ_LCD msg_lcd_main;
MSGQUEUE_OBJ_COM msg_com_rx_main;
MSGQUEUE_OBJ_COM msg_com_tx_main;

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
	init_Th_com();
	
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
				} 
				else{
					osThreadFlagsSet(get_id_Th_led(), nLED1);
				}	
				if(msg_ace_main.oy > oy_ref){
					osThreadFlagsSet(get_id_Th_led(), LED2);
				} 
				else{
					osThreadFlagsSet(get_id_Th_led(), nLED2);
				}	
				if(msg_ace_main.ox > ox_ref){
					osThreadFlagsSet(get_id_Th_led(), LED3);
				} 
				else{
					osThreadFlagsSet(get_id_Th_led(), nLED3);
				}  	
			  agregarMedida(&buffer_medidas);
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
				//*  MANEJO DE LA HORA Y ACE MEDIANTE JOY*//
				if(!cnt_prg){
					sprintf(msg_lcd_main.linea1, "      ---P&D---");
					sprintf(msg_lcd_main.linea2, "      %.2u:%.2u:%.2u ", aux_hor, aux_min, aux_seg);
					osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd_main, NULL, 0U);
				}else{
					sprintf(msg_lcd_main.linea1, "      ---P&D---");
					sprintf(msg_lcd_main.linea2, "   X:%.1f Y:%.1f Z:%.1f",aux_ox_ref, aux_oy_ref, aux_oz_ref);
					osMessageQueuePut(get_id_MsgQueue_lcd(), &msg_lcd_main, NULL, 0U);
				}
				
				if((osMessageQueueGet(get_id_MsgQueue_joy(), &msg_joy_main, NULL, 100U) == osOK)){
					if((msg_joy_main.dir == MIDDLE) && (msg_joy_main.dur == 0)){ 
						if(!cnt_prg){
							hor=aux_hor;
							min=aux_min;
							seg=aux_seg;
						}
						else{
							ox_ref=aux_ox_ref;
							oy_ref=aux_oy_ref;
							oz_ref=aux_oz_ref;
						}
						cnt_prg = cnt_prg==0 ? 1 : 0;	
					}

							/** HORA **/
					 if(cnt_prg==0){	
							if((msg_joy_main.dir == RIGHT) && (msg_joy_main.dur == 0)){ 
								cnt_hora += cnt_hora!=3;
								cnt_hora *= cnt_hora!=3;
							}
							if((msg_joy_main.dir == LEFT) && (msg_joy_main.dur == 0)){ 
								if(cnt_hora!=0){
									cnt_hora--;
								}
								else{
									cnt_hora=2;
								}	
							}
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
							if(cnt_hora==0){
								if((msg_joy_main.dir == DOWN) && (msg_joy_main.dur == 0)){
									if(aux_hor!=0){
										aux_hor--;
									}
									else{
										aux_hor=23;
									}
								}	
							}	
							else if (cnt_hora==1){	
								if((msg_joy_main.dir == DOWN) && (msg_joy_main.dur == 0)){ 
									if(aux_min!=0){
										aux_min--;
									}
									else{
										aux_min=59;
									}
								}	
							}	
							else if (cnt_hora==2){	
								if((msg_joy_main.dir == DOWN) && (msg_joy_main.dur == 0)){ 							
									if(aux_seg!=0){
									aux_seg--;
									}else{
									aux_seg=59;
									}
								}	
							}						
						}
						/** ACE **/
						
					if(cnt_prg==1){		
						if((msg_joy_main.dir == RIGHT) && (msg_joy_main.dur == 0)){ 
							cnt_ace += cnt_ace!=3;
							cnt_ace *= cnt_ace!=3;
						}
						if((msg_joy_main.dir == LEFT) && (msg_joy_main.dur == 0)){ 
							if(cnt_ace!=0){
								cnt_ace--;
							}
							else{
								cnt_ace=2;
							}		
						}				
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
					}		
					if((msg_joy_main.dir == MIDDLE) && (msg_joy_main.dur == 1)){ //cambiar de estado
						estado = REPOSO;
						hab_reloj=1;
					}
				}
				
				//* MANEJO DE LA HORA Y ACE MEDIANTE COM-PC *//
				procesarComandosRS232();
			break;
		}
	}
}

static void agregarMedida(buf_medidas* buf){
	
	if(buf->cnt == 10){
    buf->ini = (buf->ini +1)%10;
    buf->cnt--;
  }
  
  sprintf(buf->medidas[buf->fin].mesure, "%.2u:%.2u:%.2u--Tm:%.1f-Ax:%.1f-Ay:%.1f-Az:%.1f%%\n\r", hor, min, seg, msg_ace_main.temp, msg_ace_main.ox, msg_ace_main.oy,msg_ace_main.oz);
  buf->fin = (buf->fin + 1)%10;
  buf->cnt++;
	
}
static void clean_buffer(void){
  for(i = 0; i < buffer_medidas.cnt; i++){
    for(j = 0; j < 36; j++){
    buffer_medidas.medidas[i].mesure[j] = 0x00;
  }
}
}

static void procesarComandosRS232(void){
	uint8_t h, m, s, cnt_aux;
	float aux;
	if (osMessageQueueGet(get_id_MsgQueue_com_rx(), &msg_com_rx_main, NULL, 100U) == osOK) {
		switch(msg_com_rx_main.CMD){
			case HORA:
				if(sscanf(msg_com_rx_main.payload, "%hhu:%hhu:%hhu", &h, &m, &s) == 3){
					hor = h;
					min = m;
					seg = s;
					msg_com_tx_main.SOH_type = SOH;
					msg_com_tx_main.CMD = 0xDF;
					msg_com_tx_main.LEN= 0x0C; //*enunciado
					sprintf(msg_com_tx_main.payload, "%d:%d:%d\n\r",h,m,s);
					msg_com_tx_main.EOT_type = EOT;
					osMessageQueuePut(get_id_MsgQueue_com_tx(), &msg_com_tx_main, NULL, 0U);
				}
				break;
			case AX:
				if(sscanf(msg_com_rx_main.payload, "%f", &aux) == 1){
					if(aux>= 2 && aux<=2){
						ox_ref=aux;
						msg_com_tx_main.SOH_type = SOH;
						msg_com_tx_main.CMD = 0xDA;
						msg_com_tx_main.LEN= 0x08; 
						sprintf(msg_com_tx_main.payload, "%.1f", aux);
						msg_com_tx_main.EOT_type = EOT;
						osMessageQueuePut(get_id_MsgQueue_com_tx(), &msg_com_tx_main, NULL, 0U);
					}
				}
				break;
			case AY:
				if(sscanf(msg_com_rx_main.payload, "%f", &aux) == 1){
					if(aux>= 2 && aux<=2){
						oy_ref=aux;
						msg_com_tx_main.SOH_type = SOH;
						msg_com_tx_main.CMD = 0xD9;
						msg_com_tx_main.LEN= 0x08; 
						sprintf(msg_com_tx_main.payload, "%.1f", aux);
						msg_com_tx_main.EOT_type = EOT;
						osMessageQueuePut(get_id_MsgQueue_com_tx(), &msg_com_tx_main, NULL, 0U);
					}
				}
				break;
			case AZ:
				if(sscanf(msg_com_rx_main.payload, "%f", &aux) == 1){
					if(aux>= 2 && aux<=2){
						oz_ref=aux;
						msg_com_tx_main.SOH_type = SOH;
						msg_com_tx_main.CMD = 0xD8;
						msg_com_tx_main.LEN= 0x08; 
						sprintf(msg_com_tx_main.payload, "%.1f", aux);
						msg_com_tx_main.EOT_type = EOT;
						osMessageQueuePut(get_id_MsgQueue_com_tx(), &msg_com_tx_main, NULL, 0U);
						while(cnt_aux < buffer_medidas.cnt){
							memcpy(msg_com_tx_main.payload, buffer_medidas.medidas[cnt_aux].mesure, 36);
							cnt_aux++;
							msg_com_tx_main.EOT_type = EOT;
							osMessageQueuePut(get_id_MsgQueue_com_tx(), &msg_com_tx_main, NULL, 0U);
						 }
					}
				}
				break;
			case MEDIDAS:
				msg_com_tx_main.SOH_type = SOH;
				msg_com_tx_main.CMD=0xD9;
				msg_com_tx_main.LEN=0x27;
				while(cnt_aux < buffer_medidas.cnt){
					memcpy(msg_com_tx_main.payload, buffer_medidas.medidas[cnt_aux].mesure, MEDIDA_LEN);
					cnt_aux++;
					msg_com_tx_main.EOT_type=EOT;
					osMessageQueuePut(get_id_MsgQueue_com_tx(), &msg_com_tx_main, NULL, 0U);
				}
				break;
			case DELETE:
				clean_buffer();
				msg_com_tx_main.SOH_type = SOH;
				msg_com_tx_main.CMD=0x9F;
				memset(msg_com_tx_main.payload, 0, sizeof(msg_com_tx_main.payload));
				msg_com_tx_main.LEN=0x04;
				msg_com_tx_main.EOT_type=EOT;
				osMessageQueuePut(get_id_MsgQueue_com_tx(), &msg_com_tx_main, NULL, 0U);
				break;
		
		}
	}
	



}