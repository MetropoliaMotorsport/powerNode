#ifndef CANCONFIG_H_
#define CANCONFIG_H_

//function prototypes
void Set_Can_Bytes(uint32_t*, uint32_t);

//data ID definitions
#define MESS_U5I0		1 //1 byte in 100's of mA or 2 bytes in 100's of uA
#define MESS_U5I1		2
#define MESS_U6I0		3
#define MESS_U6I1		4
#define MESS_U7I0		5
#define MESS_U7I1		6
#define MESS_U5T		7 //1 byte in °C
#define MESS_U6T		8
#define MESS_U7T		9
#define MESS_U5V		10 //1 byte in V or 2 bytes in mV
#define MESS_U6V		11
#define MESS_U7V		12
#define MESS_DI			13 //1 byte always with up to 5 bits used, bits referring to pwm outputs or floating inputs may be something
#define MESS_PWM0_Freq	14 //1 or 2 bytes in Hz
#define MESS_PWM0_DC	15 //1 byte in %
#define MESS_PWM3_Freq	16
#define MESS_PWM3_DC	17
#define MESS_PWM4_Freq	18
#define MESS_PWM4_DC	19
#define MESS_POWER		20 //always 1 byte //TODO: this after getting the long switch cleaner
//TODO: power state

#endif
