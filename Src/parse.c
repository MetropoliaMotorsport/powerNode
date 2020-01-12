#include "main.h"
#include "parse.h"

//external handlers
extern TIM_HandleTypeDef htim2;


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
		//TODO: don't define number of bytes here
		return 0;
	}
}

uint32_t Parse_Voltage(uint32_t raw, uint32_t raw_ground)
{
	//voltage should be raw*4 after voltage divider, but component is not working with tolerances, voltage divider is 130 and 82 which is 106/41, voltage is raw*3.3/(2^10), V to mV is *1000;
	//approximate value from raw->Vcc based on measured values is (95*raw/2)-1645; note that this formula is not taken from reality, it is probably approximating a logarithimic formula but datasheet doesn't give enough details; is low enough precision that overflow is not a problem to consider at all
	uint32_t calculated=(((95*raw)/2)-1645)+((raw_ground*825)/256); //calculate current in mV
	//TODO: check that this function works properly for the second board assembled as well sometime

	return calculated;
}

uint32_t Parse_Temperature(uint32_t raw)
{
	uint32_t calculated=401-(raw*3791/5000); //calculate temperature in °C
	//TODO: get some sort of line for this and check what it looks like in reality, datasheet only gives value for 1k ohm resistor

	return calculated;
}

/*
		volatile uint32_t a = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2);
		volatile uint32_t b = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);*/

uint32_t Calculate_PWM_DC(uint32_t channel)
{
	uint32_t DC=0;
	uint32_t T=0, off=0;

	if (!(PWM_In_EN>>channel)&1)
	{
		//TODO: error message for uninitialized channel
		return 0;
	}

	switch(channel)
	{
	case 0:
		T=HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2);
		off=HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
		break;
	case 3:

		break;
	case 4:

		break;
	default:
		//TODO: error message here for trying to calculate PWM of invalid PWM channel
		return 0;
	}

	DC=(((T-off)*100)/T); //note integer division always rounds down

	return DC; //DC ranges from 0-100, so we don't need to
}

uint32_t Calculate_PWM_Freq(uint32_t channel)
{
	uint32_t frequency=0;
	uint32_t T=0;

	if (!(PWM_In_EN>>channel)&1)
	{
		//TODO: error message for uninitialized channel
		return 0;
	}

	switch(channel)
	{
	case 0:
		T=HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2);
		break;
	case 3:

		break;
	case 4:

		break;
	default:
		//TODO: error message here for trying to calculate PWM of invalid PWM channel
		return 0;
	}
	frequency = (((HAL_RCC_GetHCLKFreq()*10)/(T*1000))+5)/10; //calculate frequency in .1 Hz, add .5 Hz, divide by 10 to get rounded value in Hz

	//frequency can exist over too many orders of magnitude, and might want to be sent in one byte or multiple bytes, so we give option for a divider
	return frequency; //TODO: divider
}

