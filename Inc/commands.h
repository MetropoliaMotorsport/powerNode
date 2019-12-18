#ifndef COMMANDS_H_
#define COMMANDS_H_

//function prototypes
void Acknowledge(uint8_t);
void Save_Config();

void Switch_Power(uint8_t, uint8_t);

void Config_Message(uint8_t, uint8_t, uint16_t);
void Config_Switch_Defaults(uint8_t, uint8_t);
void Config_Can_Sync(uint8_t, uint8_t);

#endif
