#ifndef CONFIG_H_
#define CONFIG_H_

//ID definition
#define ID 0

//debugging definitions
#define TEST_PWN_NOT_INPUT 1

//function prototypes
void Config_Setup(void);

void Config_0(void);
void Config_1(void);

void Config_Write_Flash(void);
void Config_Read_Flash(void);

void Flash_Write(uint32_t, uint32_t, uint32_t[512], int);
uint32_t Flash_Read(uint32_t);

#endif /* CONFIG_H_ */
