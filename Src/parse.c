#include "main.h"
#include "parse.h"


uint32_t Parse_Current(uint32_t raw, uint32_t bytes)
{
	uint32_t calculated=raw/10; //calculate current in 100's of uAs
	//TODO: put actual calculation here instead of made up numbers


	switch(bytes)
	{
	case 1:
		return (calculated/1000); //return value in 100's of mAs
	case 2:
		return (calculated); //return value in 100's of uAs
	default:
		//TODO: add a warning here
		return 0;
	}
}

uint32_t Parse_Voltage(uint32_t raw, uint32_t bytes)
{
	//voltage is vcc/4, voltage divider is 130 and 82 which is 106/41, voltage is raw*3.3/(2^10), V to mV is *1000; total is 43725/20992; is no overflow possibility
	uint32_t calculated=((raw*43725)/20992); //calculate current in mV
	//TODO: test this code
	//TODO: need to add offset from ground voltage in to this?

	switch(bytes)
	{
	case 1:
		return (calculated/1000); //return value in V
	case 2:
		return (calculated); //return value in mV
	default:
		//TODO: add a warning here
		return 0;
	}
}

uint32_t Parse_Temperatuer(uint32_t raw, uint32_t bytes)
{
	uint32_t calculated=raw/10; //calculate current in °C
	//TODO: put actual calculation here instead of made up numbers


	switch(bytes)
	{
	case 1:
		return (calculated/1000); //return value in °C
	default:
		//TODO: add a warning here
		return 0;
	}
}

