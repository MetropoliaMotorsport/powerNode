#ifndef COMMANDS_H_
#define COMMANDS_H_


//config commands
#define SWITCH_POWER				1
#define CHANGE_DC					2
#define CAN_MESS0					3
#define CAN_MESS1					4
#define CAN_MESS2					5
#define CAN_MESS3					6
#define CAN_MESS4					7
#define CAN_MESS5					8
#define CAN_MESS6					9
#define CAN_MESS7					10
#define SAMPLE_TEMP_VOLT			11 //TODO
#define SAVE_CONFIGS				128
#define CONFIG_MESSAGE				129
#define CONFIG_SWITCHES_DEFAULT		130
#define CONFIG_CAN_SYNC				131
#define CONFIG_CAN_TIMED			132
#define CONFIG_CAN_INTERVAL			133
#define CONFIG_CAN_SYNC_DELAY		134
#define CONFIG_CAN_TV_READING		135
#define CONFIG_DEFAULT_DC			136
#define CONFIG_PWM_PRESCALERS		137
#define CONFIG_DIO					138

//TODO: configure EN for DIn, PWMOut, and PWMIn


//function prototypes
void Acknowledge(uint8_t);
void Save_Config();

void Switch_Power(uint8_t, uint8_t);
void Switch_DC(uint8_t, uint8_t[8]);
void Buffer_Can_Message(uint8_t);

void Config_Message(uint8_t, uint8_t, uint16_t);
void Config_Switch_Defaults(uint8_t, uint8_t);
void Config_Can_Sync(uint8_t, uint8_t);
void Config_Can_Timed(uint8_t, uint8_t);
void Config_Can_Interval(uint16_t);
void Config_Can_Sync_Delay(uint16_t);
void Config_Temperature_Voltage_Reading(uint16_t, uint8_t, uint8_t);
void Config_Default_DC(uint8_t, uint8_t[8]);
void Config_PWM_Prescalers(uint8_t, uint8_t[8]);
void Config_DIO_Pins(uint8_t, uint8_t, uint8_t, uint8_t);

#endif
