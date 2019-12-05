#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_pwr.h"

void Error_Handler(void);


//ID definition
#define ID 0

//debugging definitions
#define TEST_PWN_NOT_INPUT 1


//structs
typedef struct pinPort
{
	GPIO_TypeDef* PORT;
	uint16_t PIN;
}pinPort;

//definitions
extern const pinPort DIO3;
extern const pinPort DIO4;
extern const pinPort DIO5;
extern const pinPort DIO6;
extern const pinPort DIO15;

extern const pinPort SEL0;
const pinPort SEL1;
const pinPort U5IN0;
const pinPort U5IN1;
const pinPort U5MULTI;
const pinPort U6IN0;
const pinPort U6IN1;
const pinPort U6MULTI;
const pinPort U7IN0;
const pinPort U7IN1;
const pinPort U7MULTI;


//flash address definitions //don't use flash_read function, it is not needed if thing are defined as pointers
#define DIGITAL_IN_POS 0 //bytes: interrupt (falling edge)->PWM DC, interrupt (rising edge)->PWM DC, interrupt (falling edge)->power switch, interrupt (rising edge)->power switch, interrupt->can id high, interrupt->can id low, interrupt EN, EN
//TODO: add PWM DC (also to configuration) after PWM is set up; set interrupt to toggle power switches after power switches and interrupts are set up; set possibility for interrupt to send can message once can is set up; basic interrupt stuff, standard setup from enabling
#define DIGITAL_IN (*(uint32_t*)(FLASH_PAGE_63+0x4*DIGITAL_IN_POS))

//flash page definitions
#define FLASH_PAGE_0 0x8000000
#define FLASH_PAGE_1 FLASH_PAGE_0+0x800
#define FLASH_PAGE_2 FLASH_PAGE_1+0x800
#define FLASH_PAGE_3 FLASH_PAGE_2+0x800
#define FLASH_PAGE_4 FLASH_PAGE_3+0x800
#define FLASH_PAGE_5 FLASH_PAGE_4+0x800
#define FLASH_PAGE_6 FLASH_PAGE_5+0x800
#define FLASH_PAGE_7 FLASH_PAGE_6+0x800
#define FLASH_PAGE_8 FLASH_PAGE_7+0x800
#define FLASH_PAGE_9 FLASH_PAGE_8+0x800
#define FLASH_PAGE_10 FLASH_PAGE_9+0x800
#define FLASH_PAGE_11 FLASH_PAGE_10+0x800
#define FLASH_PAGE_12 FLASH_PAGE_11+0x800
#define FLASH_PAGE_13 FLASH_PAGE_12+0x800
#define FLASH_PAGE_14 FLASH_PAGE_13+0x800
#define FLASH_PAGE_15 FLASH_PAGE_14+0x800
#define FLASH_PAGE_16 FLASH_PAGE_15+0x800
#define FLASH_PAGE_17 FLASH_PAGE_16+0x800
#define FLASH_PAGE_18 FLASH_PAGE_17+0x800
#define FLASH_PAGE_19 FLASH_PAGE_18+0x800
#define FLASH_PAGE_20 FLASH_PAGE_19+0x800
#define FLASH_PAGE_21 FLASH_PAGE_20+0x800
#define FLASH_PAGE_22 FLASH_PAGE_21+0x800
#define FLASH_PAGE_23 FLASH_PAGE_22+0x800
#define FLASH_PAGE_24 FLASH_PAGE_23+0x800
#define FLASH_PAGE_25 FLASH_PAGE_24+0x800
#define FLASH_PAGE_26 FLASH_PAGE_25+0x800
#define FLASH_PAGE_27 FLASH_PAGE_26+0x800
#define FLASH_PAGE_28 FLASH_PAGE_27+0x800
#define FLASH_PAGE_29 FLASH_PAGE_28+0x800
#define FLASH_PAGE_30 FLASH_PAGE_29+0x800
#define FLASH_PAGE_31 FLASH_PAGE_30+0x800
#define FLASH_PAGE_32 FLASH_PAGE_31+0x800
#define FLASH_PAGE_33 FLASH_PAGE_32+0x800
#define FLASH_PAGE_34 FLASH_PAGE_33+0x800
#define FLASH_PAGE_35 FLASH_PAGE_34+0x800
#define FLASH_PAGE_36 FLASH_PAGE_35+0x800
#define FLASH_PAGE_37 FLASH_PAGE_36+0x800
#define FLASH_PAGE_38 FLASH_PAGE_37+0x800
#define FLASH_PAGE_39 FLASH_PAGE_38+0x800
#define FLASH_PAGE_40 FLASH_PAGE_39+0x800
#define FLASH_PAGE_41 FLASH_PAGE_40+0x800
#define FLASH_PAGE_42 FLASH_PAGE_41+0x800
#define FLASH_PAGE_43 FLASH_PAGE_42+0x800
#define FLASH_PAGE_44 FLASH_PAGE_43+0x800
#define FLASH_PAGE_45 FLASH_PAGE_44+0x800
#define FLASH_PAGE_46 FLASH_PAGE_45+0x800
#define FLASH_PAGE_47 FLASH_PAGE_46+0x800
#define FLASH_PAGE_48 FLASH_PAGE_47+0x800
#define FLASH_PAGE_49 FLASH_PAGE_48+0x800
#define FLASH_PAGE_50 FLASH_PAGE_49+0x800
#define FLASH_PAGE_51 FLASH_PAGE_50+0x800
#define FLASH_PAGE_52 FLASH_PAGE_51+0x800
#define FLASH_PAGE_53 FLASH_PAGE_52+0x800
#define FLASH_PAGE_54 FLASH_PAGE_53+0x800
#define FLASH_PAGE_55 FLASH_PAGE_54+0x800
#define FLASH_PAGE_56 FLASH_PAGE_55+0x800
#define FLASH_PAGE_57 FLASH_PAGE_56+0x800
#define FLASH_PAGE_58 FLASH_PAGE_57+0x800
#define FLASH_PAGE_59 FLASH_PAGE_58+0x800
#define FLASH_PAGE_60 FLASH_PAGE_59+0x800
#define FLASH_PAGE_61 FLASH_PAGE_60+0x800
#define FLASH_PAGE_62 FLASH_PAGE_61+0x800
#define FLASH_PAGE_63 FLASH_PAGE_62+0x800


#ifdef __cplusplus
}
#endif

#endif
