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
#include "checks.h"

void Error_Handler(void);

//function prototypes
void Can_Send(uint8_t);
void Can_Sync(void);
void Set_Error(uint32_t);
void Send_Error(void);
void Write_PWM(uint32_t, uint16_t);

//external functions
void HAL_TIM_MspPostInit(TIM_HandleTypeDef*);


//warning and error codes
#define ERR_CAN_BUFFER_FULL			1
#define ERR_CAN_FIFO_FULL			2
#define ERR_MESSAGE_DISABLED		3
#define ERR_DLC_0					4
#define ERR_DLC_LONG				5
#define ERR_SEND_FAILED				6
#define ERR_RECIEVE_FAILED			7
#define ERR_INVALID_COMMAND			8
#define ERR_COMMAND_SHORT			9
#define ERR_RECIEVED_INVALID_ID 	10
#define ERR_CANOFFLINE				11

#define ERR_MODIFY_INVALID_MESSAGE	33
#define ERR_MODIFY_INVALID_THING	34
#define ERR_CLEAR_INVALID_ERROR		35

#define ERR_MESS_INVALID_BYTES		97
#define ERR_MESS_UNDEFINED			98

#define U5I0_SWITCH_OFF				129 //PO0
#define U5I1_SWITCH_OFF				130 //PO1
#define U6I0_SWITCH_OFF				131 //PO2
#define U6I1_SWITCH_OFF				132 //PO3
#define U7I0_SWITCH_OFF				133 //PO4
#define U7I1_SWITCH_OFF				134 //PO5

#define WARN_UNDERVOLT_U5				193
#define WARN_OVERVOLT_U5				194
#define WARN_UNDERTEMP_U5				195
#define WARN_OVERTEMP_U5				196
#define WARN_UNDERCURR_U5I0				197
#define WARN_OVERCURR_U5I0				198
#define WARN_UNDERCURR_U5I1				199
#define WARN_OVERCURR_U5I1				200
#define ERROR_OVERCURR_TRIP_U5_0		201 //PO0
#define ERROR_OVERCURR_TRIP_U5_1		202 //PO1
#define WARN_UNDERVOLT_U6				203
#define WARN_OVERVOLT_U6				204
#define WARN_UNDERTEMP_U6				205
#define WARN_OVERTEMP_U6				206
#define WARN_UNDERCURR_U6I0				207
#define WARN_OVERCURR_U6I0				208
#define WARN_UNDERCURR_U6I1				209
#define WARN_OVERCURR_U6I1				210
#define ERROR_OVERCURR_TRIP_U6_0		211 //PO2
#define ERROR_OVERCURR_TRIP_U6_1		212 //PO3
#define WARN_UNDERVOLT_U7				213
#define WARN_OVERVOLT_U7				214
#define WARN_UNDERTEMP_U7				215
#define WARN_OVERTEMP_U7				216
#define WARN_UNDERCURR_U7I0				217
#define WARN_OVERCURR_U7I0				218
#define WARN_UNDERCURR_U7I1				219
#define WARN_OVERCURR_U7I1				220
#define ERROR_OVERCURR_TRIP_U7_0		221 //PO4
#define ERROR_OVERCURR_TRIP_U7_1		222 //PO5

#define ERROR_READ_TEMP					224
#define WARN_TEMP_MEASURE_OVERFLOW		225
#define WARN_VOLT_MEASURE_OVERFLOW		226

#define WARN_PWM_INVALID_CHANNEL		257
#define WARN_PWM_CHANNEL_UNINITIALIZED	258
#define WARN_UNDEFINED_GPIO				259
#define WARN_PWM_NOT_ENABLED			260

//Set_Error(something);


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

#define ERROR_PERIOD_100US	10000 //send error every second


//can id definitions
#define CANID_SYNC 0x80
#define CANID_CONFIG 0x602
#define CANID_ERROR 0x600
#define CANID_ACK	0x601


//global configuration variables
extern uint8_t Digital_In_EN; //byte: [x][x][x][DIO15][DI6][DIO5][DIO4][DIO3]
extern uint8_t Digital_In_Interrupt_EN;
extern uint8_t Digital_In_Interrupt_Can_Rising[5];
extern uint8_t Digital_In_Interrupt_Can_Falling[5];
extern uint8_t Digital_In_Interrupt_Power_High_Rising[5];
extern uint8_t Digital_In_Interrupt_Power_High_Falling[5];
extern uint8_t Digital_In_Interrupt_Power_Low_Rising[5];
extern uint8_t Digital_In_Interrupt_Power_Low_Falling[5];
extern uint8_t Digital_In_Interrupt_PWM_Rising[5]; //TODO
extern uint8_t Digital_In_Interrupt_PWM_Falling[5]; //TODO

extern uint8_t PWM_Out_EN;
extern uint16_t PWM_Prescalers[5];
extern uint16_t PWM_Pulses[5];

extern uint8_t PWM_In_EN;

extern uint8_t Default_Switch_State;

extern uint16_t Can_IDs[8];
extern uint8_t Can_DLCs[8];
extern uint8_t Can_Config_Bytes[8][8];
extern uint8_t Can_Config_Datas[8][8];
extern uint8_t Can_Sync_Enable;
extern uint8_t Can_Timed_Enable;
extern uint16_t Can_Interval;
extern uint16_t Can_Sync_Delay;

extern uint16_t warn_undervoltage_U5;
extern uint16_t warn_overvoltage_U5;
extern uint16_t warn_undertemperature_U5;
extern uint16_t warn_overtemperature_U5;
extern uint16_t warn_undercurrent_U5I0;
extern uint16_t warn_overcurrent_U5I0;
extern uint16_t warn_undercurrent_U5I1;
extern uint16_t warn_overcurrent_U5I1;
extern uint16_t cutoff_overcurrent_U5I0;
extern uint16_t cutoff_overcurrent_U5I1;
extern uint16_t warn_undervoltage_U6;
extern uint16_t warn_overvoltage_U6;
extern uint16_t warn_undertemperature_U6;
extern uint16_t warn_overtemperature_U6;
extern uint16_t warn_undercurrent_U6I0;
extern uint16_t warn_overcurrent_U6I0;
extern uint16_t warn_undercurrent_U6I1;
extern uint16_t warn_overcurrent_U6I1;
extern uint16_t cutoff_overcurrent_U6I0;
extern uint16_t cutoff_overcurrent_U6I1;
extern uint16_t warn_undervoltage_U7;
extern uint16_t warn_overvoltage_U7;
extern uint16_t warn_undertemperature_U7;
extern uint16_t warn_overtemperature_U7;
extern uint16_t warn_undercurrent_U7I0;
extern uint16_t warn_overcurrent_U7I0;
extern uint16_t warn_undercurrent_U7I1;
extern uint16_t warn_overcurrent_U7I1;
extern uint16_t cutoff_overcurrent_U7I0;
extern uint16_t cutoff_overcurrent_U7I1;

extern uint32_t sample_temperature;
extern uint32_t sample_voltage;
extern uint16_t SampleTemperatureVoltagePeriod;
extern uint8_t SampleTemperatureBurst;
extern uint8_t SampleVoltageBurst;

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

extern uint8_t U5I0_active;
extern uint8_t U5I1_active;
extern uint8_t U6I0_active;
extern uint8_t U6I1_active;
extern uint8_t U7I0_active;
extern uint8_t U7I1_active;

extern uint8_t U5I0_error;
extern uint8_t U5I1_error;
extern uint8_t U6I0_error;
extern uint8_t U6I1_error;
extern uint8_t U7I0_error;
extern uint8_t U7I1_error;


//more details about what is stored in each word can be found in config.c
//flash address definitions
#define DIGITAL_IN_0_POS					0
//TODO: add PWM DC (also to configuration) after PWM is set up; set interrupt to toggle power switches after power switches and interrupts are set up; set possibility for interrupt to send can message once can is set up; basic interrupt stuff, standard setup from enabling
#define DIGITAL_IN_0						(*(uint32_t*)(FLASH_PAGE_63+0x4*DIGITAL_IN_0_POS))
#define DIGITAL_IN_1_POS					(DIGITAL_IN_0_POS+1)
#define DIGITAL_IN_1						(*(uint32_t*)(FLASH_PAGE_63+0x4*DIGITAL_IN_1_POS))
#define CAN_ID_0_POS						(DIGITAL_IN_1_POS+10)
#define CAN_ID_0							(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_0_POS))
#define CAN_ID_1_POS						(CAN_ID_0_POS+1)
#define CAN_ID_1							(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_1_POS))
#define CAN_ID_2_POS						(CAN_ID_1_POS+1)
#define CAN_ID_2							(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_2_POS))
#define CAN_ID_3_POS						(CAN_ID_2_POS+1)
#define CAN_ID_3							(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_3_POS))
#define CAN_ID_4_POS						(CAN_ID_3_POS+1)
#define CAN_ID_4							(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_4_POS))
#define CAN_ID_5_POS						(CAN_ID_4_POS+1)
#define CAN_ID_5							(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_5_POS))
#define CAN_ID_6_POS						(CAN_ID_5_POS+1)
#define CAN_ID_6							(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_6_POS))
#define CAN_ID_7_POS						(CAN_ID_6_POS+1)
#define CAN_ID_7							(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_ID_7_POS))
#define CAN_BYTES_1ST_POS					(CAN_ID_7_POS+1)
#define CAN_BYTES_1ST						(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_BYTES_1ST_POS))
#define CAN_DATAS_1ST_POS					(CAN_BYTES_1ST_POS+16)
#define CAN_DATAS_1ST						(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_DATAS_1ST_POS))
#define DEFAULT_SWITCH_STATE_POS			(CAN_DATAS_1ST_POS+16)
#define DEFAULT_SWITCH_STATE				(*(uint32_t*)(FLASH_PAGE_63+0x4*DEFAULT_SWITCH_STATE_POS))
#define CAN_SEND_EN_POS						(DEFAULT_SWITCH_STATE_POS+1)
#define CAN_SEND_EN							(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_SEND_EN_POS))
#define CAN_SYNC_DELAY_POS					(CAN_SEND_EN_POS+1)
#define CAN_SYNC_DELAY						(*(uint32_t*)(FLASH_PAGE_63+0x4*CAN_SYNC_DELAY_POS))
#define U5_VOLTAGE_WARNING_LIMIT_POS		(CAN_SYNC_DELAY_POS+1)
#define U5_VOLTAGE_WARNING_LIMIT			(*(uint32_t*)(FLASH_PAGE_63+0x4*U5_VOLTAGE_WARNING_LIMIT_POS))
#define U5_TEMPERATURE_WARNING_LIMIT_POS	(U5_VOLTAGE_WARNING_LIMIT_POS+1)
#define U5_TEMPERATURE_WARNING_LIMIT		(*(uint32_t*)(FLASH_PAGE_63+0x4*U5_TEMPERATURE_WARNING_LIMIT_POS))
#define U5_CURRENT_WARNING_LIMIT_I0_POS		(U5_TEMPERATURE_WARNING_LIMIT_POS+1)
#define U5_CURRENT_WARNING_LIMIT_I0			(*(uint32_t*)(FLASH_PAGE_63+0x4*U5_CURRENT_WARNING_LIMIT_I0_POS))
#define U5_CURRENT_WARNING_LIMIT_I1_POS		(U5_CURRENT_WARNING_LIMIT_I0_POS+1)
#define U5_CURRENT_WARNING_LIMIT_I1			(*(uint32_t*)(FLASH_PAGE_63+0x4*U5_CURRENT_WARNING_LIMIT_I1_POS))
#define U6_VOLTAGE_WARNING_LIMIT_POS		(U5_CURRENT_WARNING_LIMIT_I1_POS+1)
#define U6_VOLTAGE_WARNING_LIMIT			(*(uint32_t*)(FLASH_PAGE_63+0x4*U6_VOLTAGE_WARNING_LIMIT_POS))
#define U6_TEMPERATURE_WARNING_LIMIT_POS	(U6_VOLTAGE_WARNING_LIMIT_POS+1)
#define U6_TEMPERATURE_WARNING_LIMIT		(*(uint32_t*)(FLASH_PAGE_63+0x4*U6_TEMPERATURE_WARNING_LIMIT_POS))
#define U6_CURRENT_WARNING_LIMIT_I0_POS		(U6_TEMPERATURE_WARNING_LIMIT_POS+1)
#define U6_CURRENT_WARNING_LIMIT_I0			(*(uint32_t*)(FLASH_PAGE_63+0x4*U6_CURRENT_WARNING_LIMIT_I0_POS))
#define U6_CURRENT_WARNING_LIMIT_I1_POS		(U6_CURRENT_WARNING_LIMIT_I0_POS+1)
#define U6_CURRENT_WARNING_LIMIT_I1			(*(uint32_t*)(FLASH_PAGE_63+0x4*U6_CURRENT_WARNING_LIMIT_I1_POS))
#define U7_VOLTAGE_WARNING_LIMIT_POS		(U6_CURRENT_WARNING_LIMIT_I1_POS+1)
#define U7_VOLTAGE_WARNING_LIMIT			(*(uint32_t*)(FLASH_PAGE_63+0x4*U7_VOLTAGE_WARNING_LIMIT_POS))
#define U7_TEMPERATURE_WARNING_LIMIT_POS	(U7_VOLTAGE_WARNING_LIMIT_POS+1)
#define U7_TEMPERATURE_WARNING_LIMIT		(*(uint32_t*)(FLASH_PAGE_63+0x4*U7_TEMPERATURE_WARNING_LIMIT_POS))
#define U7_CURRENT_WARNING_LIMIT_I0_POS		(U7_TEMPERATURE_WARNING_LIMIT_POS+1)
#define U7_CURRENT_WARNING_LIMIT_I0			(*(uint32_t*)(FLASH_PAGE_63+0x4*U7_CURRENT_WARNING_LIMIT_I0_POS))
#define U7_CURRENT_WARNING_LIMIT_I1_POS		(U7_CURRENT_WARNING_LIMIT_I0_POS+1)
#define U7_CURRENT_WARNING_LIMIT_I1			(*(uint32_t*)(FLASH_PAGE_63+0x4*U7_CURRENT_WARNING_LIMIT_I1_POS))
#define TV_BURST_POS						(U7_CURRENT_WARNING_LIMIT_I1_POS+1)
#define TV_BURST							(*(uint32_t*)(FLASH_PAGE_63+0x4*TV_BURST_POS))
#define TV_BURST_TIMING_POS					(TV_BURST_POS+1)
#define TV_BURST_TIMING						(*(uint32_t*)(FLASH_PAGE_63+0x4*TV_BURST_TIMING_POS))
#define PWM_0_POS							(TV_BURST_TIMING_POS+1)
#define PWM_0								(*(uint32_t*)(FLASH_PAGE_63+0x4*PWM_0_POS))
#define PWM_1_POS							(PWM_0_POS+1)
#define PWM_1								(*(uint32_t*)(FLASH_PAGE_63+0x4*PWM_1_POS))
#define PWM_2_POS							(PWM_1_POS+1)
#define PWM_2								(*(uint32_t*)(FLASH_PAGE_63+0x4*PWM_2_POS))
#define PWM_3_POS							(PWM_2_POS+1)
#define PWM_3								(*(uint32_t*)(FLASH_PAGE_63+0x4*PWM_3_POS))
#define PWM_4_POS							(PWM_3_POS+1)
#define PWM_4								(*(uint32_t*)(FLASH_PAGE_63+0x4*PWM_4_POS))
#define PWM_EN_POS							(PWM_4_POS+1)
#define PWM_EN								(*(uint32_t*)(FLASH_PAGE_63+0x4*PWM_EN_POS))
#define U5_CURRENT_SHUTOFF_POS				(PWM_EN_POS+1)
#define U5_CURRENT_SHUTOFF					(*(uint32_t*)(FLASH_PAGE_63+0x4*U5_CURRENT_SHUTOFF_POS))
#define U6_CURRENT_SHUTOFF_POS				(U5_CURRENT_SHUTOFF_POS+1)
#define U6_CURRENT_SHUTOFF					(*(uint32_t*)(FLASH_PAGE_63+0x4*U6_CURRENT_SHUTOFF_POS))
#define U7_CURRENT_SHUTOFF_POS				(U6_CURRENT_SHUTOFF_POS+1)
#define U7_CURRENT_SHUTOFF					(*(uint32_t*)(FLASH_PAGE_63+0x4*U7_CURRENT_SHUTOFF_POS))


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
