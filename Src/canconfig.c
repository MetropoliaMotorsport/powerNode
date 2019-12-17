#include "main.h"
#include "canconfig.h"


void Set_Can_Bytes(uint32_t *pos, uint32_t message)
{
//Can_Config_Bytes[message][*pos];

	uint32_t transmit=0;

	switch(Can_Config_Datas[message][*pos])
	{
	case MESS_U5I0:
		transmit=0;
		switch(Can_Config_Bytes[message][*pos])
		{
		case 1: //TODO: function to parse raw values to real values
			CANTxData[*pos] = transmit>>8 & 0xFF;
			*pos+=1;
		case 2:
			CANTxData[*pos] = transmit>>8 & 0xFF;
			CANTxData[*pos+1] = transmit>>0 & 0xFF;
			*pos+=2;
		default:
			//TODO: warning for wrong length message
			break;
		}
		break;

	case MESS_U5I1:

		break;

	case MESS_U6I0:

		break;

	case MESS_U6I1:

		break;

	case MESS_U7I0:

		break;

	case MESS_U7I1:

		break;

	case MESS_U5T:

		break;

	case MESS_U6T:

		break;

	case MESS_U7T:

		break;

	case MESS_U5V:
		transmit=Parse_Voltage(U5V_raw_average, U5GNDV_raw_average, Can_Config_Bytes[message][*pos]);
		break;

	case MESS_U6V:
		transmit=Parse_Voltage(U6V_raw_average, U6GNDV_raw_average, Can_Config_Bytes[message][*pos]);
		break;

	case MESS_U7V:
		transmit=Parse_Voltage(U7V_raw_average, U7GNDV_raw_average, Can_Config_Bytes[message][*pos]);
		break;

	default:
		//TODO: warning from requested undefined message
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
