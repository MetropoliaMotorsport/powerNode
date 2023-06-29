#ifndef CONFIG_H_
#define CONFIG_H_

#define HPF2023

//ID definition
#define ID 35

//debugging definitions, only used for ID 0
#define TEST_PWN_NOT_INPUT	0
#define TEST_PWM_INPUT		0

//function prototypes
void Config_Setup(void);

void Config_0(void);
void Config_33(void);
void Config_34(void);
void Config_35(void);
void Config_36(void);
void Config_37(void);


void Config_Write_Flash(void);
void Config_Read_Flash(void);

void Flash_Write(uint32_t, uint32_t, uint32_t[512], int);
uint32_t Flash_Read(uint32_t);

#endif /* CONFIG_H_ */
