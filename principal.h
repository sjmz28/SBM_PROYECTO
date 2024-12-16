#ifndef __PRINCIPAL_H
#define __PRINCIPAL_H

#include "cmsis_os2.h"

#include "ace.h"
#include "Hora.h"
#include "joy.h"
#include "lcd.h"
#include "led.h"
#include "com.h"
// COMANDOS
#define HORA 0x20
#define AX 0x25
#define AY 0x26
#define AZ 0x27
#define MEDIDAS 0x55
#define DELETE 0x60

#define MEDIDA_LEN 52
typedef struct{
  char mesure[MEDIDA_LEN];
}mesure_t;

typedef struct{
  mesure_t medidas[10];
  uint8_t ini;
  uint8_t fin;
  uint8_t cnt;
}buf_medidas;

int init_Th_principal(void);



#endif
