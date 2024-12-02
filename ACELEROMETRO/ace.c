#include "cmsis_os2.h"        //Para manejar el RTOS
#include "stm32f4xx_hal.h"    //Para manejar el HAL
#include "Driver_I2C.h"

#include "ace.h"

#define I2C 0x01
#define TIM 0x02


/**
nota: las medidas se realizarán cada segundo de ambas medidas
REGISTROS IMPORTANTES:

direccion I2C esclavo -- 0x68 ( cuando esta conectado el pin AD0 a tierra y 0x69 si esta conectado a VCC

OX  - 0x3B [15:8] 0x3C[7:0]
OY  - 0x3D [15:8] 0x3E[7:0]
OZ  - 0x3F [15:8] 0x40[7:0]

TEM - 0x41 [15:8] 0x42[7:0]

**/

static osThreadId_t id_Th_ace;
static osMessageQueueId_t id_MsgQueue_ace;

int init_Th_ace(void);
static int Init_MsgQueue_ace(void);
osThreadId_t get_id_Th_ace(void);
osMessageQueueId_t get_id_MsgQueue_ace(void);
static void init_i2c(void);

static void Th_ace(void *arg);

// variable I2C
extern ARM_DRIVER_I2C Driver_I2C1;
ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;

static int Init_MsgQueue_ace(void){
	id_MsgQueue_ace = osMessageQueueNew(16, sizeof(MSGQUEUE_OBJ_ACE), NULL);
	if(id_MsgQueue_ace == NULL)
		return(-1);
	
	return (0);
}

osMessageQueueId_t get_id_MsgQueue_ace(void){
	return id_MsgQueue_ace;
}

int init_Th_ace(void){
	id_Th_ace = osThreadNew(Th_ace, NULL, NULL);
  
	if(id_Th_ace == NULL)
		return(-1);
  
	return(Init_MsgQueue_ace());
}

static void i2c_Callback(uint32_t event){ 
    osThreadFlagsSet(get_id_Th_ace(), I2C);
}

static void init_i2c(void){
		I2Cdrv ->Initialize(i2c_Callback);     // indicamos la funcion callback para cuando se termine de realizar transacciones
		I2Cdrv ->PowerControl(ARM_POWER_FULL); // por pedir que no falte
		I2Cdrv ->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD); // velocidad del bus
	  I2Cdrv ->Control(ARM_I2C_BUS_CLEAR, 0); //Limpiamos el bus I2C de posibles comunicaciones que se hubieran quedado a medias

		// direccion del escalvo
	  I2Cdrv ->Control(ARM_I2C_OWN_ADDRESS, 0x68);
}

osThreadId_t get_id_Th_ace(void){
return id_Th_ace;
}

static void Th_ace(void *argument){ 
	
	uint8_t data[2] = {0x1C, 0x00};// Dirección del registro ACCEL_CONFIG &  Configuración para rango de ±2g
	I2Cdrv->MasterTransmit(0x68, data, 2, true); // 0x68 es la dirección I2C del MPU-6050 con AD0 a GND
	osThreadFlagsWait(I2C, osFlagsWaitAll, osWaitForever);
	
	
	uint8_t reg_pwr_mgmt_1 = 0x6B; // Dirección del registro PWR_MGMT_1
	uint8_t pwr_mgmt_value = 0x01; // Configuración para desactivar sleep y usar PLL eje X como reloj

	// Escribir en el registro PWR_MGMT_1
	I2Cdrv->MasterTransmit(0x68, &reg_pwr_mgmt_1, 1, true); 
	osThreadFlagsWait(I2C, osFlagsWaitAll, osWaitForever);

	I2Cdrv->MasterTransmit(0x68, &pwr_mgmt_value, 1, false);
	osThreadFlagsWait(I2C, osFlagsWaitAll, osWaitForever);
		
  while(1){

  }

}