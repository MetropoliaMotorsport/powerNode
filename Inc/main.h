#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

//includes
#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_pwr.h"
#include "config.h"
#include "commands.h"
#include "canconfig.h"
#include "parse.h"

void Error_Handler(void);


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
extern const pinPort SEL1;
extern const pinPort U5IN0;
extern const pinPort U5IN1;
extern const pinPort U5MULTI;
extern const pinPort U6IN0;
extern const pinPort U6IN1;
extern const pinPort U6MULTI;
extern const pinPort U7IN0;
extern const pinPort U7IN1;
extern const pinPort U7MULTI;

#define I_ROLLING_AVERAGE 4
#define T_ROLLING_AVERAGE 4
#define V_ROLLING_AVERAGE 1

//can id definitions
#define CANID_CONFIG 0x700
#define CANID_SYNC 0x80
#define CANID_ERROR 0x600

//config commands
#define SWITCH_POWER	1
#define CHANGE_DC		2
#define something		255


//global configuration variables
extern uint8_t Digital_In_EN; //byte: xxx[DIO15][DI6][DIO5][DIO4][DIO3]
extern uint8_t Digital_In_Interrupt_EN; //TODO
extern uint8_t Digital_In_Interrupt_Can_Rising; //TODO
extern uint8_t Digital_In_Interrupt_Can_Falling; //TODO
extern uint8_t Digital_In_Interrupt_Power_Rising; //TODO
extern uint8_t Digital_In_Interrupt_Power_Falling; //TODO
extern uint8_t Digital_In_Interrupt_PWM_Rising; //TODO
extern uint8_t Digital_In_Interrupt_PWM_Falling; //TODO
extern uint16_t Can_IDs[8];
extern uint8_t Can_DLCs[8];
extern uint8_t Can_Config_Bytes[8][8];
extern uint8_t Can_Config_Datas[8][8];

//global variables
extern uint8_t CANTxData[8];

extern uint32_t U5I0_real;
extern uint32_t U5I1_real;
extern uint32_t U5T_real;
extern uint32_t U5V_real;
extern uint32_t U6I0_real;
extern uint32_t U6I1_real;
extern uint32_t U6T_real;
extern uint32_t U6V_real;
extern uint32_t U7I0_real;
extern uint32_t U7I1_real;
extern uint32_t U7T_real;
extern uint32_t U7V_real;


//more details about what is stored in each word can be found in main before the while(1) loop
//flash address definitions //don't use flash_read function, it is not needed if thing are defined as pointers
#define DIGITAL_IN_0_POS	0
//TODO: add PWM DC (also to configuration) after PWM is set up; set interrupt to toggle power switches after power switches and interrupts are set up; set possibility for interrupt to send can message once can is set up; basic interrupt stuff, standard setup from enabling
#define DIGITAL_IN_0		(*(uint32_t*)(FLASH_PAGE_63+0x4*DIGITAL_IN_0_POS))
#define DIGITAL_IN_1_POS	(DIGITAL_IN_0_POS+1)
#define DIGITAL_IN_1		(*(uint32_t*)(FLASH_PAGE_63+0x4*DIGITAL_IN_1_POS))
#define CAN_ID_0_POS		(DIGITAL_IN_1_POS+1)
#define CAN_ID_0			(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_0_POS))
#define CAN_ID_1_POS		(CAN_ID_0_POS+1)
#define CAN_ID_1			(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_1_POS))
#define CAN_ID_2_POS		(CAN_ID_1_POS+1)
#define CAN_ID_2			(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_2_POS))
#define CAN_ID_3_POS		(CAN_ID_2_POS+1)
#define CAN_ID_3			(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_3_POS))
#define CAN_ID_4_POS		(CAN_ID_3_POS+1)
#define CAN_ID_4			(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_4_POS))
#define CAN_ID_5_POS		(CAN_ID_4_POS+1)
#define CAN_ID_5			(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_5_POS))
#define CAN_ID_6_POS		(CAN_ID_5_POS+1)
#define CAN_ID_6			(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_6_POS))
#define CAN_ID_7_POS		(CAN_ID_6_POS+1)
#define CAN_ID_7			(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_7_POS))

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
