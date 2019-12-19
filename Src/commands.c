#include "main.h"
#include "commands.h"

//externs
extern FDCAN_HandleTypeDef hfdcan;


uint32_t ack_k=0;

void Acknowledge(uint8_t cmd)
{
	FDCAN_TxHeaderTypeDef TxHeader;

	TxHeader.Identifier = CANID_ACK;
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;

	CANTxData[0]=ID;
	CANTxData[1]=cmd;
	CANTxData[2]=(ack_k>>24)*0xFF;
	CANTxData[3]=(ack_k>>16)*0xFF;
	CANTxData[4]=(ack_k>>8)*0xFF;
	CANTxData[5]=ack_k&0xFF;
	CANTxData[6]=cmd;
	CANTxData[7]=0xFF;

	TxHeader.IdType = FDCAN_STANDARD_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader.MessageMarker = 0;

	while(HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan) < 1)
	{
		//TODO: think if this is the best way to handle this
		//Error_Handler();
		//return;
	}

	if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan, &TxHeader, CANTxData) != HAL_OK)
	{
		Set_Error(ERR_SEND_FAILED);
		return;
	}

	ack_k++;
}

void Save_Config()
{
	Config_Write_Flash();

	Config_Read_Flash();

	Acknowledge(SAVE_CONFIGS);
}


const pinPort *switches[] = {&U5IN0, &U5IN1, &U6IN0, &U6IN1, &U7IN0, &U7IN1};

void Switch_Power(uint8_t enableSwitching, uint8_t newState)
{
	for(uint32_t i=0; i<6; i++)
	{
		if ((1<<i) & enableSwitching)
		{
			HAL_GPIO_WritePin(switches[i]->PORT, switches[i]->PIN, ((1<<i)&newState)>>i);
		}
	}

	Acknowledge(SWITCH_POWER);
}


void Config_Message(uint8_t message, uint8_t change, uint16_t data)
{
	if(message>7)
	{
		Set_Error(ERR_MODIFY_INVALID_MESSAGE);
		return;
	}

	if(change<8)
	{
		Can_Config_Bytes[message][change]=data;
	}
	else if(change<16)
	{
		Can_Config_Datas[message][change-8]=data;
	}
	else if(change==16)
	{
		Can_IDs[message]=data;
	}
	else if(change==17)
	{
		Can_DLCs[message]=data;
	}
	else
	{
		Set_Error(ERR_MODIFY_INVALID_THING);
		return;
	}

	Acknowledge(CONFIG_MESSAGE);
}

void Config_Switch_Defaults(uint8_t enableChanges, uint8_t newState)
{
	for(uint32_t i=0; i<6; i++)
	{
		if (((enableChanges>>i)&0b1))
		{
			if(((newState>>i)&0b1))
			{
				Default_Switch_State|=(1<<i);
			}
			else
			{
				Default_Switch_State&=~(1<<i);
			}
		}
	}

	Acknowledge(CONFIG_SWITCHES_DEFAULT);
}

void Config_Can_Sync(uint8_t enableChanges, uint8_t newState)
{
	for(uint32_t i=0; i<8; i++)
	{
		if (((enableChanges>>i)&0b1))
		{
			if(((newState>>i)&0b1))
			{
				Can_Sync_Enable|=(1<<i);
			}
			else
			{
				Can_Sync_Enable&=~(1<<i);
			}
		}
	}

	Acknowledge(CONFIG_CAN_SYNC);
}

void Config_Can_Timed(uint8_t enableChanges, uint8_t newState)
{
	for(uint32_t i=0; i<8; i++)
	{
		if (((enableChanges>>i)&0b1))
		{
			if(((newState>>i)&0b1))
			{
				Can_Timed_Enable|=(1<<i);
			}
			else
			{
				Can_Timed_Enable&=~(1<<i);
			}
		}
	}

	Acknowledge(CONFIG_CAN_TIMED);
}

void Config_Can_Interval(uint16_t newInterval)
{
	Can_Interval=newInterval;

	Acknowledge(CONFIG_CAN_INTERVAL);
}
