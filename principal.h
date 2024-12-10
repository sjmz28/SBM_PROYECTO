#ifndef __PRINCIPAL_H
#define __PRINCIPAL_H

#include "cmsis_os2.h"

#include "ace.h"
#include "Hora.h"
#include "joy.h"
#include "lcd.h"
#include "led.h"

typedef struct{
  char mesure[36];
}mesure_t;

typedef struct{
  mesure_t medidas[10];
  uint8_t ini;
  uint8_t fin;
  uint8_t cnt;
}buf_medidas;

int init_Th_principal(void);

#endif
