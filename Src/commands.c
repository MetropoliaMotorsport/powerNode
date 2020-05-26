#include "main.h"
#include "commands.h"

//externs
extern FDCAN_HandleTypeDef hfdcan;


uint32_t ack_k=0;
uint32_t blnk_k=0;
extern pinPort LED;

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
		//blinking at .5 Hz means something has gone wrong here, this should only happen during configuration so it is considered acceptable infinite loop location
		blnk_k++;
		if(blnk_k>340000000)
		{
			blnk_k=0;
			HAL_GPIO_TogglePin(LED.PORT, LED.PIN);
		}
	}

	if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan, &TxHeader, CANTxData) != HAL_OK)
	{
		Set_Error(ERR_SEND_FAILED);
		return;
	}

	ack_k++;
}

extern TIM_HandleTypeDef htim15;

void Save_Config()
{
	Config_Write_Flash();

	Config_Read_Flash();

	Acknowledge(SAVE_CONFIGS);
}


const pinPort *switches[] = {&U5IN0, &U5IN1, &U6IN0, &U6IN1, &U7IN0, &U7IN1};
uint8_t *actives[] = {&U5I0_active, &U5I1_active, &U6I0_active, &U6I1_active, &U7I0_active, &U7I1_active};
uint8_t *errors[] = {&U5I0_error, &U5I1_error, &U6I0_error, &U6I1_error, &U7I0_error, &U7I1_error};

void Switch_Power(uint8_t enableSwitching, uint8_t newState)
{
	for(uint32_t i=0; i<5; i++)
	{
		if (((1<<i) & enableSwitching) && errors[i])
		{
			HAL_GPIO_WritePin(switches[i]->PORT, switches[i]->PIN, ((1<<i)&newState)>>i);
			*actives[i]=((1<<i)&newState)>>i;
		}
	}

	Acknowledge(SWITCH_POWER);
}

void Switch_DC(uint8_t channelEN, uint8_t newDC[8])
{
	uint32_t pos=3; //start at pos 3 and use a length 8 array so that we can just pass rx data
	for(uint32_t i=0; i<5; i++)
	{
		if ((1<<i) & channelEN)
		{
			Write_PWM(i, newDC[pos]);
			pos++;
		}
	}

	Acknowledge(CHANGE_DC);
}

extern uint8_t CanBuffer[31];
extern uint8_t CanMessagesToSend;
extern uint8_t CanBufferReadPos;
extern uint8_t CanBufferWritePos;

void Buffer_Can_Message(uint8_t message)
{
	if(CanBuffer[CanBufferWritePos]!=255)
	{
		Set_Error(ERR_CAN_BUFFER_FULL);
	}
	//overwrite unsent messages
	CanBuffer[CanBufferWritePos]=message;

	if(CanBufferWritePos>=30)
	{
		CanBufferWritePos=0;
	}
	else
	{
		CanBufferWritePos++;
	}
	CanMessagesToSend++;
}

extern uint32_t sample_temperature;
extern uint32_t sample_voltage;

void Sample_Temperature_Voltage(uint8_t temperature_samples, uint8_t voltage_samples)
{
	sample_temperature+=temperature_samples;
	if(temperature_samples){ Set_Error(ERROR_READ_TEMP); } //temperature sensors not working properly
	sample_voltage+=voltage_samples;
}

void Clear_Error(uint8_t error)
{
	switch(error)
	{
	case 0:
		U5I0_error=0;
		break;
	case 1:
		U5I1_error=0;
		break;
	case 2:
		U6I0_error=0;
		break;
	case 3:
		U6I1_error=0;
		break;
	case 4:
		U7I0_error=0;
		break;
	case 5:
		U7I1_error=0;
		break;
	default:
		Set_Error(ERR_CLEAR_INVALID_ERROR);
		break;
	}

	Acknowledge(CLEAR_ERROR);
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

void Config_Can_Sync_Delay(uint16_t newDelay)
{
	Can_Sync_Delay=newDelay;

	Acknowledge(CONFIG_CAN_SYNC_DELAY);
}

void Config_Temperature_Voltage_Reading(uint16_t interval, uint8_t tempBurst, uint8_t voltBurst)
{
	if(tempBurst==255)
	{
		sample_temperature=255;
		SampleTemperatureBurst=0;
	}
	else
	{
		sample_temperature=0;
		SampleTemperatureBurst=tempBurst;
	}

	if(voltBurst==255)
	{
		sample_voltage=255;
		SampleVoltageBurst=0;
	}
	else
	{
		sample_voltage=0;
		SampleVoltageBurst=voltBurst;
	}

	SampleTemperatureVoltagePeriod=interval;

	Acknowledge(CONFIG_CAN_TV_READING);
}

void Config_Default_DC(uint8_t channelEN, uint8_t newDC[8])
{
	uint32_t pos=3; //start at pos 3 and use a length 8 array so that we can just pass rx data
	for(uint32_t i=0; i<5; i++)
	{
		if ((1<<i) & channelEN)
		{
			PWM_Pulses[i]=newDC[pos];
			pos++;
		}
	}

	Acknowledge(CONFIG_DEFAULT_DC);
}

void Config_PWM_Prescalers(uint8_t channelEN, uint8_t newPrescalers[8])
{
	uint32_t pos=3; //start at pos 3 and use a length 8 array so that we can just pass rx data
	for(uint32_t i=0; i<5; i++)
	{
		if ((1<<i) & channelEN)
		{
			PWM_Prescalers[i]=newPrescalers[pos];
			pos++;
		}
	}

	Acknowledge(CONFIG_PWM_PRESCALERS);
}

void Config_DIO_Pins(uint8_t EN, uint8_t new_Din_EN, uint8_t new_PWM_Out_EN, uint8_t new_PWM_In_EN)
{
	for(uint32_t i=0; i<5; i++)
	{
		if ((EN>>i)&1)
		{
			Digital_In_EN &= ~(1<<i);
			PWM_Out_EN &= ~(1<<i);
			PWM_In_EN &= ~(1<<i);

			if ((new_Din_EN>>i)&1)
			{
				if (i != 1) //disable PB4 as input
				{
					Digital_In_EN |= (1<<i);
				}
			}
			else if ((new_PWM_Out_EN>>i)&1)
			{
				if (1)
				{
					PWM_Out_EN |= (1<<i);
				}
			}
			else if ((new_PWM_In_EN>>i)&1)
			{
				if ( (i!=1) && (i!=2) ) //disable PB4 as input, PB5 doesn't have functionality as PWM input
				{
					PWM_In_EN |= (1<<i);
				}
			}
		}
	}

	Acknowledge(CONFIG_DIO);
}

void Config_Interrupt_Power(uint8_t gpio, uint8_t Power_High_Falling, uint8_t Power_High_Rising, uint8_t Power_Low_Falling, uint8_t Power_Low_Rising)
{

	Digital_In_Interrupt_Power_High_Falling[gpio]=Power_High_Falling;
	Digital_In_Interrupt_Power_High_Rising[gpio]=Power_High_Rising;
	Digital_In_Interrupt_Power_Low_Falling[gpio]=Power_Low_Falling;
	Digital_In_Interrupt_Power_Low_Rising[gpio]=Power_Low_Rising;

	Acknowledge(CONFIG_INTERRUPT_POWER);
}

void Config_Interrupt_Can(uint8_t gpio, uint8_t Can_Falling, uint8_t Can_Rising)
{
	Digital_In_Interrupt_Can_Falling[gpio]=Can_Falling;
	Digital_In_Interrupt_Can_Rising[gpio]=Can_Rising;

	Acknowledge(CONFIG_INTERRUPT_CAN);
}
