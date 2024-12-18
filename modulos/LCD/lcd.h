#ifndef __LCD_H
#define __LCD_H

#include <stdio.h>
#include <string.h>
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"        //Para manejar el RTOS



typedef struct{
	char linea1[32];
	char linea2[32];
} MSGQUEUE_OBJ_LCD;

int init_Th_lcd(void);
osMessageQueueId_t get_id_MsgQueue_lcd(void);

static void LCD_Reset(void);
static void LCD_init(void);
static void LCD_update(void);
static void dataToBuffer(char data_L1[32], char data_L2[32]);

#endif
