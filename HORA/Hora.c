#include "cmsis_os2.h" 
#include <stdio.h>// CMSIS RTOS header file
#include "Hora.h"

osThreadId_t id_Th_hora;                        // thread id
static uint32_t flag;

 uint8_t seg;
 uint8_t min;
 uint8_t hor;
 uint8_t hab_reloj;
 
static uint8_t hab_min=0;
static uint8_t hab_hor=0;
static uint8_t hab_reset=0;

 
 osTimerId_t timer_seg;             // timer de 1 segundo para la hora 
 
 
void Th_hora (void *argument);                   // thread function
 
int Init_Th_hora (void) {
 
  id_Th_hora = osThreadNew(Th_hora, NULL, NULL);
  if (id_Th_hora == NULL) {
    return(-1);
  }
 
  return(0);
}

static char linea1[32], linea2[32];

osThreadId_t get_id_Th_Hora(void){
 return id_Th_hora;
}

static void timer_seg_Callback(void *argument){
osThreadFlagsSet(get_id_Th_Hora(), SEG);
}



void Th_hora (void *argument) {
	
	hab_reloj=1;
	seg=0;
	min=0;
	hor=0;
		
	timer_seg = osTimerNew(timer_seg_Callback, osTimerPeriodic, (void *)0, NULL);
	
	osTimerStart(timer_seg, 1000U);
 
  while (1) {
   
		flag = osThreadFlagsWait(SEG, osFlagsWaitAny, osWaitForever);
		
	////////////////////// Reset del reloj ////////////////////////////////////////////////////////				
//			if(hab_reset==1){
//		
//			min=0;
//			hor=0;
//			hab_reset=0;
//				
//			}
	////////////////////////////////////////////////////////////////////////////////////////////////
		
	/////////////////////////// SEGUNDOS /////////////////////////////////		
		if(flag&SEG){
		
		if(hab_reloj){	
			
			seg++;
			min += seg == 60;
			hor += min == 60;
			
			hor *= hor != 24;
			min *= min != 60;
			seg *= seg != 60;
			
		
//	////////////////////// Contador de segundos con habilitacion de fin de cuenta ////////////////			
//			if(seg<59){
//				seg++;
//			} else { 
//				seg=0;
//				hab_min=1;
//			}
//	///////////////////////////////////////////////////////////////////////////////////////////////

//	////////////////////// Contador de minutos con habilitacion de fin de cuenta ////////////////				
//			if(hab_min==1){
//				
//				hab_min=0;
//				
//				if(min<59){
//					min++;
//				} else { 
//					min=0;
//					hab_hor=1;
//				}
//			}
//	////////////////////////////////////////////////////////////////////////////////////////////////			

//	////////////////////// Contador de horas con habilitacion de fin de cuenta ////////////////				
//			if(hab_hor==1){
//				
//				hab_hor=0;
//				hab_reset = (hor==23) ? 1 : 0;
//				hor = (hor<23) ? hor+1 : 0;
//			}
	////////////////////////////////////////////////////////////////////////////////////////////
      }	
		}
	}
}
