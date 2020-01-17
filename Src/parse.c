#include "main.h"
#include "parse.h"

//external handlers
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim8;


uint32_t Parse_Current(uint32_t raw)
{
	uint32_t calculated=raw/10; //calculate current in 100's of uAs
	//TODO: put actual calculation here instead of made up numbers

	return calculated;
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

uint32_t Calculate_PWM_DC(uint32_t channel)
{
	uint32_t DC=0;
	uint32_t T=0, off=0, on=0;

	if (!(PWM_In_EN>>channel)&1)
	{
		Set_Error(WARN_PWM_CHANNEL_UNINITIALIZED);
		return 0;
	}

	switch(channel)
	{
	case 0:
		T=HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2);
		off=HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
		DC=(((T-off)*100)/T); //note integer division always rounds down
		break;
	case 3:
		T=HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_1);
		on=HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_2);
		DC=((on*100)/T);
		break;
	case 4:
		T=HAL_TIM_ReadCapturedValue(&htim8, TIM_CHANNEL_1);
		on=HAL_TIM_ReadCapturedValue(&htim8, TIM_CHANNEL_2);
		DC=((on*100)/T);
		break;
	default:
		Set_Error(WARN_PWM_INVALID_CHANNEL);
		return 0;
	}


	return DC; //DC ranges from 0-100
}

uint32_t Calculate_PWM_Freq(uint32_t channel)
{
	uint32_t frequency=0;
	uint32_t T=0;

	if (!(PWM_In_EN>>channel)&1)
	{
		Set_Error(WARN_PWM_CHANNEL_UNINITIALIZED);
		return 0;
	}

	switch(channel)
	{
	case 0:
		T=HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2);
		break;
	case 3:
		T=HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_1);
		break;
	case 4:
		T=HAL_TIM_ReadCapturedValue(&htim8, TIM_CHANNEL_1);
		break;
	default:
		Set_Error(WARN_PWM_INVALID_CHANNEL);
		return 0;
	}
	frequency = (((HAL_RCC_GetHCLKFreq()*10)/(T*(PWM_Prescalers[channel]+1)))+5)/10; //calculate frequency in .1 Hz, add .5 Hz, divide by 10 to get rounded value in Hz

	//if frequency is higher than 255 Hz, two bytes must be used to send the frequency, frequency will almost surely not be over 65kHz
	return frequency;
}

