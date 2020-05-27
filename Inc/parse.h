#ifndef PARSE_H_
#define PARSE_H_

//function prototypes
uint32_t Parse_Current(uint32_t, uint32_t, uint32_t);
uint32_t Parse_Voltage(uint32_t, uint32_t);
uint32_t Parse_Temperature(uint32_t);

uint32_t Calculate_PWM_DC(uint32_t);
uint32_t Calculate_PWM_Freq(uint32_t);

#endif
