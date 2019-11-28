#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_pwr.h"

void Error_Handler(void);


//definitions
typedef struct pinPort
{
	GPIO_TypeDef* PORT;
	uint16_t PIN;
}pinPort;

extern const pinPort DIO3;
extern const pinPort DIO4;
extern const pinPort DIO5;
extern const pinPort DIO6;
extern const pinPort DIO15;


#ifdef __cplusplus
}
#endif

#endif
