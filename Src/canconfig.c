#include "main.h"
#include "canconfig.h"


void Set_Can_Bytes(uint32_t *pos, uint32_t message)
{
	switch(/*Can_Config_Datas[message][*pos]*/message)
	{
	case 1:
		CANTxData[*pos] = 0xF;
		*pos+=1;
		break;
	default:
		CANTxData[*pos] = 0;
		CANTxData[*pos+1] = 0xFF;
		*pos+=2;
		break;
	}

}
