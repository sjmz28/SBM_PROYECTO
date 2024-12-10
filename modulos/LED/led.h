#ifndef __LED_H
#define __LED_H

#include "cmsis_os2.h"

#define LED1 0x0010
#define nLED1 0x0020
#define LED2 0x0040
#define nLED2 0x0080
#define LED3 0x0100
#define nLED3 0x0200


int init_Th_led(void);
osThreadId_t get_id_Th_led(void);

#endif
