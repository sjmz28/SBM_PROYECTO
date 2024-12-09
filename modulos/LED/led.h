#ifndef __LED_H
#define __LED_H

#include "cmsis_os2.h"

typedef struct{
	uint8_t led; // bit 1 - LED 1; bit 2 - LED 2; bit 3 - LED 3
} MSGQUEUE_OBJ_LED;

int init_Th_led(void);
osMessageQueueId_t get_id_MsgQueue_led(void);

#endif
