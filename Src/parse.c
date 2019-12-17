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

uint32_t Parse_Voltage(uint32_t raw, uint32_t raw_ground)
{
	//voltage should be raw*4 after voltage divider, but component is not working with tolerances, voltage divider is 130 and 82 which is 106/41, voltage is raw*3.3/(2^10), V to mV is *1000;
	//approximate value from raw->Vcc based on measured values is (95*raw/2)-1645; note that this formula is not taken from reality, it is probably approximating a logarithimic formula but datasheet doesn't give enough details; is low enough precision that overflow is not a problem to consider at all
	uint32_t calculated=(((95*raw)/2)-1645)+((raw_ground*825)/256); //calculate current in mV
	//TODO: need to add offset from ground voltage in to this
	//TODO: check that this function works properly for the second board assembled as well sometime

	return calculated;
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

