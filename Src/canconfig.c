#include "main.h"
#include "canconfig.h"


void Set_Can_Bytes(uint32_t *pos, uint32_t message)
{
	//Can_Config_Bytes[message][*pos];

	uint32_t transmit=0;

	switch(Can_Config_Datas[message][*pos])
	{
	case MESS_U5I0:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U5I0_real/1000); //transmit number in 100s of mA
			break;
		case 2:
			transmit=U5I0_real; //transmit number in 100s of uA
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U5I1:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U5I0_real/1000); //transmit number in 100s of mA
			break;
		case 2:
			transmit=U5I1_real; //transmit number in 100s of uA
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U6I0:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U6I0_real/1000); //transmit number in 100s of mA
			break;
		case 2:
			transmit=U6I0_real; //transmit number in 100s of uA
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U6I1:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U6I1_real/1000); //transmit number in 100s of mA
			break;
		case 2:
			transmit=U6I1_real; //transmit number in 100s of uA
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U7I0:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U7I0_real/1000); //transmit number in 100s of mA
			break;
		case 2:
			transmit=U7I0_real; //transmit number in 100s of uA
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U7I1:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U7I1_real/1000); //transmit number in 100s of mA
			break;
		case 2:
			transmit=U7I1_real; //transmit number in 100s of uA
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U5T:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U5T_real); //transmit number in °C
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U6T:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U6T_real); //transmit number in °C
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U7T:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U7T_real); //transmit number in °C
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U5V:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U5V_real/1000); //transmit number in V
			break;
		case 2:
			transmit=U5V_real; //transmit number in mV
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U6V:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U6V_real/1000); //transmit number in V
			break;
		case 2:
			transmit=U6V_real; //transmit number in mV
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_U7V:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			transmit=(U7V_real/1000); //transmit number in V
			break;
		case 2:
			transmit=U7V_real; //transmit number in mV
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	case MESS_DI:
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1:
			//DIO4 still not working, but we send whatever it says anyway
			transmit=(HAL_GPIO_ReadPin(DIO3.PORT, DIO3.PIN)<<0)|(HAL_GPIO_ReadPin(DIO4.PORT, DIO4.PIN)<<1)|(HAL_GPIO_ReadPin(DIO5.PORT, DIO5.PIN)<<2)|(HAL_GPIO_ReadPin(DIO6.PORT, DIO6.PIN)<<3)|(HAL_GPIO_ReadPin(DIO15.PORT, DIO15.PIN)<<4);
			break;
		default:
			Set_Error(ERR_MESS_INVALID_BYTES);
			break;
		}
		break;

	//for these we have the same transmit regardless of how many bytes it will use; this means it is easy to overflow if one byte is used, but we want to send smaller frequencies (up to 255Hz) on one byte
	//this also means there is no warning for wrong number of bytes though
	case MESS_PWM0_Freq:
		transmit=Calculate_PWM_Freq(0);
		break;
	case MESS_PWM0_DC:
		transmit=Calculate_PWM_DC(0);
		break;
	case MESS_PWM3_Freq:
		transmit=Calculate_PWM_Freq(3);
		break;
	case MESS_PWM3_DC:
		transmit=Calculate_PWM_DC(3);
		break;
	case MESS_PWM4_Freq:
		transmit=Calculate_PWM_Freq(4);
		break;
	case MESS_PWM4_DC:
		transmit=Calculate_PWM_DC(4);
		break;

	case MESS_POWER:
		transmit=((U5I0_active&0x1)<<0)+((U5I1_active&0x1)<<1)+((U6I0_active&0x1)<<2)+((U6I1_active&0x1)<<3)+((U7I0_active&0x1)<<4)+((U7I1_active&0x1)<<5);
		break;

	default:
		Set_Error(ERR_MESS_UNDEFINED);
		break;
	}

	uint32_t temp_pos = *pos;
	for(uint32_t i=0; i<Can_Config_Bytes[message][temp_pos]; i++)
	{
		//fill the least significant byte first
		CANTxData[temp_pos+(Can_Config_Bytes[message][temp_pos]-i-1)] = transmit>>(i*8) & 0xFF;

		*pos+=1;
	}

}
