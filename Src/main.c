#include "main.h"


//static function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_FDCAN_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);

//type handlers
FDCAN_HandleTypeDef hfdcan;
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

//definitions
const pinPort LED = { .PORT=GPIOA, .PIN=GPIO_PIN_8 };
const pinPort DIO3 = { .PORT=GPIOB, .PIN=GPIO_PIN_3 };
const pinPort DIO4 = { .PORT=GPIOB, .PIN=GPIO_PIN_4 }; //DON'T USE THIS PIN AS AN INPUT AS MICROCONTROLLER SEEMS TO HAVE COUPLING TO PA10 WITH IT
const pinPort DIO5 = { .PORT=GPIOB, .PIN=GPIO_PIN_5 };
const pinPort DIO6 = { .PORT=GPIOB, .PIN=GPIO_PIN_6 };
const pinPort DIO15 = { .PORT=GPIOA, .PIN=GPIO_PIN_15 };

const pinPort SEL0 = { .PORT=GPIOA, .PIN=GPIO_PIN_9 };
const pinPort SEL1 = { .PORT=GPIOB, .PIN=GPIO_PIN_0 };
const pinPort U5IN0 = { .PORT=GPIOA, .PIN=GPIO_PIN_10 };
const pinPort U5IN1 = { .PORT=GPIOA, .PIN=GPIO_PIN_7 };
const pinPort U5MULTI = { .PORT=GPIOA, .PIN=GPIO_PIN_2 };
const pinPort U6IN0 = { .PORT=GPIOA, .PIN=GPIO_PIN_6 };
const pinPort U6IN1 = { .PORT=GPIOA, .PIN=GPIO_PIN_5 };
const pinPort U6MULTI = { .PORT=GPIOA, .PIN=GPIO_PIN_1 };
const pinPort U7IN0 = { .PORT=GPIOA, .PIN=GPIO_PIN_4 };
const pinPort U7IN1 = { .PORT=GPIOA, .PIN=GPIO_PIN_3 };
const pinPort U7MULTI = { .PORT=GPIOA, .PIN=GPIO_PIN_0 };

//global configuration variables
uint8_t Digital_In_EN; //byte: xxx[DIO15][DI6][DIO5][DIO4][DIO3]
uint8_t Digital_In_Interrupt_EN; //TODO
uint8_t Digital_In_Interrupt_Can_Rising; //TODO
uint8_t Digital_In_Interrupt_Can_Falling; //TODO
uint8_t Digital_In_Interrupt_Power_Rising; //TODO
uint8_t Digital_In_Interrupt_Power_Falling; //TODO
uint8_t Digital_In_Interrupt_PWM_Rising; //TODO
uint8_t Digital_In_Interrupt_PWM_Falling; //TODO
uint8_t Default_Switch_State;
uint16_t Can_IDs[8];
uint8_t Can_DLCs[8];
uint8_t Can_Config_Bytes[8][8];
uint8_t Can_Config_Datas[8][8];
uint8_t Can_Sync_Enable;
uint8_t Can_Timed_Enable;
uint16_t Can_Interval;

uint16_t warn_undervoltage_U5;
uint16_t warn_overvoltage_U5;
uint16_t warn_undertemperature_U5;
uint16_t warn_overtemperature_U5;
uint16_t warn_undercurrent_U5I0;
uint16_t warn_overcurrent_U5I0;
uint16_t warn_undercurrent_U5I1;
uint16_t warn_overcurrent_U5I1;
uint16_t cutoff_overcurrent_U5; //TODO
uint16_t warn_undervoltage_U6;
uint16_t warn_overvoltage_U6;
uint16_t warn_undertemperature_U6;
uint16_t warn_overtemperature_U6;
uint16_t warn_undercurrent_U6I0;
uint16_t warn_overcurrent_U6I0;
uint16_t warn_undercurrent_U6I1;
uint16_t warn_overcurrent_U6I1;
uint16_t cutoff_overcurrent_U6; //TODO
uint16_t warn_undervoltage_U7;
uint16_t warn_overvoltage_U7;
uint16_t warn_undertemperature_U7;
uint16_t warn_overtemperature_U7;
uint16_t warn_undercurrent_U7I0;
uint16_t warn_overcurrent_U7I0;
uint16_t warn_undercurrent_U7I1;
uint16_t warn_overcurrent_U7I1;
uint16_t cutoff_overcurrent_U7; //TODO
//probably several here for which switch to switch and on or off and which pwm out to change and too what

//global variables
uint8_t canErrorToTransmit; //8 32 bit values, each 32 bit value can store 32 errors or warnings
uint32_t canErrors[8];
uint8_t canSendErrorFlag;

uint32_t U5I0[I_ROLLING_AVERAGE];
uint32_t U5I0_real;
uint32_t U5I1[I_ROLLING_AVERAGE];
uint32_t U5I1_real;
uint32_t U5T[T_ROLLING_AVERAGE];
uint32_t U5T_real;
uint32_t U5V[V_ROLLING_AVERAGE];
uint32_t U5V_real;
uint32_t U5GNDV[V_ROLLING_AVERAGE];

uint32_t U6I0[I_ROLLING_AVERAGE];
uint32_t U6I0_real;
uint32_t U6I1[I_ROLLING_AVERAGE];
uint32_t U6I1_real;
uint32_t U6T[T_ROLLING_AVERAGE];
uint32_t U6T_real;
uint32_t U6V[V_ROLLING_AVERAGE];
uint32_t U6V_real;
uint32_t U6GNDV[V_ROLLING_AVERAGE];

uint32_t U7I0[I_ROLLING_AVERAGE];
uint32_t U7I0_real;
uint32_t U7I1[I_ROLLING_AVERAGE];
uint32_t U7I1_real;
uint32_t U7T[T_ROLLING_AVERAGE];
uint32_t U7T_real;
uint32_t U7V[V_ROLLING_AVERAGE];
uint32_t U7V_real;
uint32_t U7GNDV[V_ROLLING_AVERAGE];

uint32_t I0_rolling_average_position=0;
uint32_t I1_rolling_average_position=0;
uint32_t T_rolling_average_position=0;
uint32_t V_rolling_average_position=0;
uint32_t adc_selection=0;
uint32_t ADCDualConvertedValues[3];

uint8_t CANTxData[8];

uint8_t CanBuffer[31] = {[0 ... 30]=255};
uint8_t CanMessagesToSend;
uint8_t CanBufferReadPos;
uint8_t CanBufferWritePos;

uint8_t CanSyncFlag;
uint8_t CanTimerFlag;

//for these 255 means enable continuously while lower numbers mean to take that many samples
uint32_t sample_temperature;
uint32_t sample_voltage;


int main(void)
{
	HAL_Init();

	SystemClock_Config();

	Config_Setup();

	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	MX_FDCAN_Init();
	if(Can_Timed_Enable) { MX_TIM6_Init(); }//only start timer 6 if can will send a message on an interval; this means mcu must be power cycled when enabling sending a can message on an interval though
	MX_TIM7_Init();

	//start everything that can generate interrupts after initialization is done
	if(Can_Timed_Enable) { HAL_TIM_Base_Start_IT(&htim6); }
	HAL_TIM_Base_Start_IT(&htim7);

	while(1)
	{
		if(canErrorToTransmit && canSendErrorFlag)
		{
			Send_Error();
			if(!canErrorToTransmit)
			{
				canSendErrorFlag=0;
			}
		}


		if(CanSyncFlag)
		{
			for(uint32_t i=0; i<8; i++)
			{
				if ((Can_Sync_Enable>>i)&0b1)
				{
					if(CanBuffer[CanBufferWritePos]!=0)
					{
						Set_Error(ERR_CAN_BUFFER_FULL);
					}
					//overwrite unsent messages
					CanBuffer[CanBufferWritePos]=i;

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
			}
			CanSyncFlag=0;
		}
		if(CanTimerFlag)
		{
			for(uint32_t i=0; i<8; i++)
			{
				if ((Can_Timed_Enable>>i)&0b1)
				{
					if(CanBuffer[CanBufferWritePos]!=255)
					{
						Set_Error(ERR_CAN_BUFFER_FULL);
					}
					//overwrite unsent messages
					CanBuffer[CanBufferWritePos]=i;

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
			}
			CanTimerFlag=0;
		}


		if (CanMessagesToSend)
		{
			if (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan) > 0)
			{
				Can_Send(CanBuffer[CanBufferReadPos]);

				CanBuffer[CanBufferReadPos]=255;
				CanMessagesToSend--;
				if(CanBufferReadPos>=30)
				{
					CanBufferReadPos=0;
				}
				else
				{
					CanBufferReadPos++;
				}
			}
		}

		//TODO: start this from a timer instead of here
	    if (HAL_ADCEx_MultiModeStart_DMA(&hadc1, ADCDualConvertedValues, 3) != HAL_OK)
	    {
	     // Error_Handler();
	    }
	    HAL_Delay(1);


//TODO: test high side drivers again for realistic power of fans and pumps while in heatshrink
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		CanTimerFlag=1;
	}
	else if(htim->Instance == TIM7)
	{
		canSendErrorFlag=1;
	}
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC1)
	{
		uint32_t sampled = adc_selection;
		uint32_t convertedValue[3];
		uint32_t masterConvertedValue[3];
		uint32_t slaveConvertedValue[3];

		//we want the ability to not sample temperature and voltage as often as current, as sampling those heats up the switch
		//we also want to make sure that both current channels are sampled
		//TODO: change the constant 3 into however many switches are used; if only channel 0 is used then don't read current on channel 1
		//TODO: set voltage and temperature interrupts from timer, make timer configurable
		switch(adc_selection)
		{
		case 0: //we have just sampled current for channel 0, so sample current for channel 1
			HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 1);
			HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 0);
			adc_selection=1;
			break;
		case 1: //if temperature or voltage has been enabled sample those, otherwise start again with current for channel 0
			if (sample_temperature)
			{
				HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 0);
				HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 1);
				if (sample_temperature<255) { sample_temperature--; }
				adc_selection=2;
			}
			else if (sample_voltage)
			{
				HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 1);
				HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 1);
				if (sample_voltage<255) { sample_voltage--; }
				adc_selection=3;
			}
			else
			{
				HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 0);
				HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 0);
				adc_selection=0;
			}
			break;
		case 2: //if voltage has been enabled sample it, otherwise go back to current for channel 0
			if (sample_voltage)
			{
				HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 1);
				HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 1);
				if (sample_voltage<255) { sample_voltage--; }
				adc_selection=3;
			}
			else
			{
				HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 0);
				HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 0);
				adc_selection=0;
			}
			break;
		case 3: //as voltage has just been sampled go back to current for channel 0
			HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 0);
			HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 0);
			adc_selection=0;
			break;
		default:
			Error_Handler();
			break;
		}
		//TODO: start timer here

		for(uint32_t i=0; i<3; i++)
		{
			slaveConvertedValue[i]=(ADCDualConvertedValues[i]>>16)&0xFFFF;
			masterConvertedValue[i]=ADCDualConvertedValues[i]&0xFFFF;
			if(masterConvertedValue[i]>slaveConvertedValue[i])
			{
				convertedValue[i]=masterConvertedValue[i]-slaveConvertedValue[i];
			}
			else //this should only happen due to error in reading at low voltages
			{
				convertedValue[i]=0;
			}
		}

		if (HAL_ADCEx_MultiModeStop_DMA(&hadc1) != HAL_OK)
		{
			Error_Handler();
		}

		//if not all switches are used this still does not take too much time and it is fine to write some extra 0s to variables
		switch(sampled)
		{
		case 0:
			U5I0[I0_rolling_average_position]=convertedValue[0];
			U6I0[I0_rolling_average_position]=convertedValue[1];
			U7I0[I0_rolling_average_position]=convertedValue[2];
			if (I0_rolling_average_position == I_ROLLING_AVERAGE-1)
			{
				I0_rolling_average_position=0;
			}
			else
			{
				I0_rolling_average_position++;
			}

			uint32_t U5I0_raw=0; uint32_t U6I0_raw=0; uint32_t U7I0_raw=0;
			for(uint32_t i=0; i<I_ROLLING_AVERAGE; i++) //this has possibility to overflow if ROLLING_AVERAGE > 2^(32-10) (reading 10 bit value)
			{
				U5I0_raw+=U5I0[i];
				U6I0_raw+=U6I0[i];
				U7I0_raw+=U7I0[i];
			}
			U5I0_raw/=I_ROLLING_AVERAGE; U6I0_raw/=I_ROLLING_AVERAGE; U7I0_raw/=I_ROLLING_AVERAGE; //TODO: calculated U5I0_calculated from U5I0_raw
			U5I0_real=U5I0_raw; U6I0_real=U6I0_raw; U7I0_real=U7I0_raw; //TODO: warnings on over/undercurrent, overcurrent shutoff

			//TODO: check overcurrent and switch off immediately if too high

			//TODO: probably move this to main because it isn't so important
			//TODO: undercurrent warnings need to only happen when enabled, and logic for enabling them needs to exist
			if (U5I0_real>warn_overcurrent_U5I0)
			{
				Set_Error(WARN_OVERCURR_U5I0);
			}
			if (U5I0_real<warn_undercurrent_U5I0)
			{
				Set_Error(WARN_UNDERCURR_U5I0);
			}
			if (U6I0_real>warn_overcurrent_U6I0)
			{
				Set_Error(WARN_OVERCURR_U6I0);
			}
			if (U6I0_real<warn_undercurrent_U6I0)
			{
				Set_Error(WARN_UNDERCURR_U6I0);
			}
			if (U7I0_real>warn_overcurrent_U7I0)
			{
				Set_Error(WARN_OVERCURR_U7I0);
			}
			if (U7I0_real<warn_undercurrent_U7I0)
			{
				Set_Error(WARN_UNDERCURR_U7I0);
			}
			//TODO: test all these limits when I have power supply and power trimmer

			break;
		case 1:
			U5I1[I1_rolling_average_position]=convertedValue[0];
			U6I1[I1_rolling_average_position]=convertedValue[1];
			U7I1[I1_rolling_average_position]=convertedValue[2];
			if (I1_rolling_average_position == I_ROLLING_AVERAGE-1)
			{
				I1_rolling_average_position=0;
			}
			else
			{
				I1_rolling_average_position++;
			}

			uint32_t U5I1_raw=0; uint32_t U6I1_raw=1; uint32_t U7I1_raw=0;
			for(uint32_t i=0; i<I_ROLLING_AVERAGE; i++) //this has possibility to overflow if ROLLING_AVERAGE > 2^(32-10) (reading 10 bit value)
			{
				U5I1_raw+=U5I1[i];
				U6I1_raw+=U6I1[i];
				U7I1_raw+=U7I1[i];
			}
			U5I1_raw/=I_ROLLING_AVERAGE; U6I1_raw/=I_ROLLING_AVERAGE; U7I1_raw/=I_ROLLING_AVERAGE; //TODO: calculated U5I0_calculated from U5I0_raw
			U5I1_real=U5I1_raw; U6I1_real=U6I1_raw; U7I1_real=U7I1_raw; //TODO: warnings on over/undercurrent, overcurrent shutoff

			//TODO: check overcurrent and switch off immediately if too high

			//TODO: probably move this to main because it isn't so important
			if (U5I1_real>warn_overcurrent_U5I1)
			{
				Set_Error(WARN_OVERCURR_U5I1);
			}
			if (U5I1_real<warn_undercurrent_U5I1)
			{
				Set_Error(WARN_UNDERCURR_U5I1);
			}
			if (U6I1_real>warn_overcurrent_U6I1)
			{
				Set_Error(WARN_OVERCURR_U6I1);
			}
			if (U6I1_real<warn_undercurrent_U6I1)
			{
				Set_Error(WARN_UNDERCURR_U6I1);
			}
			if (U7I1_real>warn_overcurrent_U7I1)
			{
				Set_Error(WARN_OVERCURR_U7I1);
			}
			if (U7I1_real<warn_undercurrent_U7I1)
			{
				Set_Error(WARN_UNDERCURR_U7I1);
			}
			//TODO: test all these limits when I have power supply and power trimmer

			break;
		case 2:
			U5T[T_rolling_average_position]=convertedValue[0];
			U6T[T_rolling_average_position]=convertedValue[1];
			U7T[T_rolling_average_position]=convertedValue[2];
			if (T_rolling_average_position == T_ROLLING_AVERAGE-1)
			{
				T_rolling_average_position=0;
			}
			else
			{
				T_rolling_average_position++;
			}

			uint32_t U5T_raw=0; uint32_t U6T_raw=1; uint32_t U7T_raw=0;
			for(uint32_t i=0; i<T_ROLLING_AVERAGE; i++) //this has possibility to overflow if ROLLING_AVERAGE > 2^(32-10) (reading 10 bit value)
			{
				U5T_raw+=U5T[i];
				U6T_raw+=U6T[i];
				U7T_raw+=U7T[i];
			}
			U5T_raw/=T_ROLLING_AVERAGE; U6T_raw/=T_ROLLING_AVERAGE; U7T_raw/=T_ROLLING_AVERAGE; //TODO: calculated U5I0_calculated from U5I0_raw
			U5T_real=U5T_raw; U6T_real=U6T_raw; U7T_real=U7T_raw; //TODO: warnings on over/undercurrent, overcurrent shutoff

			//TODO: probably move this to main because it isn't so important
			if (U5T_real>warn_overtemperature_U5)
			{
				Set_Error(WARN_OVERTEMP_U5);
			}
			if (U5T_real<warn_undertemperature_U5)
			{
				Set_Error(WARN_UNDERTEMP_U5);
			}
			if (U6T_real>warn_overtemperature_U6)
			{
				Set_Error(WARN_OVERTEMP_U6);
			}
			if (U6T_real<warn_undertemperature_U6)
			{
				Set_Error(WARN_UNDERTEMP_U6);
			}
			if (U7T_real>warn_overtemperature_U7)
			{
				Set_Error(WARN_OVERTEMP_U7);
			}
			if (U7T_real<warn_undertemperature_U7)
			{
				Set_Error(WARN_UNDERTEMP_U7);
			}
			//TODO: test all these limits when I have power supply and power trimmer

			break;
		case 3:
			U5V[V_rolling_average_position]=convertedValue[0]; U5GNDV[V_rolling_average_position]=slaveConvertedValue[0];
			U6V[V_rolling_average_position]=convertedValue[1]; U6GNDV[V_rolling_average_position]=slaveConvertedValue[1];
			U7V[V_rolling_average_position]=convertedValue[2]; U7GNDV[V_rolling_average_position]=slaveConvertedValue[2];
			if (V_rolling_average_position == V_ROLLING_AVERAGE-1)
			{
				V_rolling_average_position=0;
			}
			else
			{
				V_rolling_average_position++;
			}

			uint32_t U5V_raw=0; uint32_t U6V_raw=0; uint32_t U7V_raw=0; uint32_t U5GNDV_raw=0; uint32_t U6GNDV_raw=0; uint32_t U7GNDV_raw=0;
			for(uint32_t i=0; i<V_ROLLING_AVERAGE; i++) //this has possibility to overflow if ROLLING_AVERAGE > 2^(32-10) (reading 10 bit value)
			{
				U5V_raw+=U5V[i]; U5GNDV_raw+=U5GNDV[i];
				U6V_raw+=U6V[i]; U6GNDV_raw+=U6GNDV[i];
				U7V_raw+=U7V[i]; U7GNDV_raw+=U7GNDV[i];
			}
			U5V_raw/=V_ROLLING_AVERAGE; U6V_raw/=V_ROLLING_AVERAGE; U7V_raw/=V_ROLLING_AVERAGE; U5GNDV_raw/=V_ROLLING_AVERAGE; U6GNDV_raw/=V_ROLLING_AVERAGE; U7GNDV_raw/=V_ROLLING_AVERAGE;
			U5V_real=Parse_Voltage(U5V_raw, U5GNDV_raw); U6V_real=Parse_Voltage(U6V_raw, U6GNDV_raw); U7V_real=Parse_Voltage(U7V_raw, U7GNDV_raw);

			//TODO: probably move this to main because it isn't so important
			if (U5V_real>warn_overvoltage_U5)
			{
				Set_Error(WARN_OVERVOLT_U5);
			}
			if (U5V_real<warn_undervoltage_U5)
			{
				Set_Error(WARN_UNDERVOLT_U5);
			}
			if (U6V_real>warn_overvoltage_U6)
			{
				Set_Error(WARN_OVERVOLT_U6);
			}
			if (U6V_real<warn_undervoltage_U6)
			{
				Set_Error(WARN_UNDERVOLT_U6);
			}
			if (U7V_real>warn_overvoltage_U7)
			{
				Set_Error(WARN_OVERVOLT_U7);
			}
			if (U7V_real<warn_undervoltage_U7)
			{
				Set_Error(WARN_UNDERVOLT_U7);
			}
			//TODO: test all these limits when I have power supply and power trimmer
			break;
		default:
			Error_Handler();
			break;
		}

	}
}


void Can_Send(uint8_t message)
{
	if(HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan) < 1)
	{
		Set_Error(ERR_CAN_FIFO_FULL);
		return;
	}

	if(Can_IDs[message]>2047)
	{
		Set_Error(ERR_MESSAGE_DISABLED);
		return;
	}
	else if(Can_DLCs[message]==0)
	{
		Set_Error(ERR_DLC_0);
		return;
	}
	else if(Can_DLCs[message]>8)
	{
		Set_Error(ERR_DLC_LONG);
		return;
	}

	FDCAN_TxHeaderTypeDef TxHeader;

	TxHeader.Identifier = Can_IDs[message];
	TxHeader.DataLength = (Can_DLCs[message]<<16); //<<16 makes storing the number of bytes not require a switch statement for classic can

	//clear can tx data so that data from incorrectly configured message is 0
	for(uint32_t i=0; i<8; i++)
	{
		CANTxData[i]=0;
	}

	uint32_t pos=0;
	for(uint32_t i=0; i<Can_DLCs[message]; i++) //max number of function calls is same as number of bytes
	{
		Set_Can_Bytes(&pos, message);
		if(pos>=Can_DLCs[message])
		{
			break;
		}
	}

	TxHeader.IdType = FDCAN_STANDARD_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader.MessageMarker = 0;

	if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan, &TxHeader, CANTxData) != HAL_OK)
	{
		Set_Error(ERR_SEND_FAILED);
		return;
	}
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	FDCAN_RxHeaderTypeDef RxHeader;
	uint8_t CANRxData[8];

	if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
	{
		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, CANRxData) != HAL_OK)
		{
			Set_Error(ERR_RECIEVE_FAILED);
		}

		//set any bytes not actaully read to 0 to prevent unknown values being in them
		for(uint32_t i=(RxHeader.DataLength>>16); i<8; i++)
		{
			CANRxData[i]=0;
		}

		if (RxHeader.Identifier == CANID_SYNC)
		{
			//TODO: add an option for a delay to this
			CanSyncFlag=1;
		}
		else if (RxHeader.Identifier == CANID_CONFIG)
		{
			//TODO: PWM frequencies
			if(CANRxData[0] == ID)
			{
				switch(CANRxData[1])
				{
				case SWITCH_POWER:
					Switch_Power(CANRxData[2], CANRxData[3]);
					if ((RxHeader.DataLength>>16) < 4) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case SAVE_CONFIGS:
					Save_Config();
					break;
				case CONFIG_MESSAGE:
					Config_Message(CANRxData[2], CANRxData[3], (((uint16_t)CANRxData[4])<<8)+(((uint16_t)CANRxData[5])<<0));
					if ((RxHeader.DataLength>>16) < 6) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CONFIG_SWITCHES_DEFAULT:
					Config_Switch_Defaults(CANRxData[2], CANRxData[3]);
					if ((RxHeader.DataLength>>16) < 4) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CONFIG_CAN_SYNC:
					Config_Can_Sync(CANRxData[2], CANRxData[3]);
					if ((RxHeader.DataLength>>16) < 4) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CONFIG_CAN_TIMED:
					Config_Can_Timed(CANRxData[2], CANRxData[3]);
					if ((RxHeader.DataLength>>16) < 4) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CONFIG_CAN_INTERVAL:
					Config_Can_Interval((((uint16_t)CANRxData[2])<<8)+(((uint16_t)CANRxData[3])<<0));
					if ((RxHeader.DataLength>>16) < 4) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				default:
					Set_Error(ERR_INVALID_COMMAND);
					break;
				}
			}
		}
		else
		{
			Set_Error(ERR_RECIEVED_INVALID_ID);
		}
	}
}


void Set_Error(uint32_t error)
{
	canErrors[(error/32)]  |= (1<<(error%32));
	canErrorToTransmit |= (1<<(error/32));
}

void Send_Error(void)
{
	for(uint32_t i=0; i<8; i++)
	{
		if (canErrorToTransmit&(1<<i))
		{
			if(HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan) > 0)
			{
				FDCAN_TxHeaderTypeDef TxHeader;

				TxHeader.Identifier = CANID_ERROR;
				TxHeader.DataLength = FDCAN_DLC_BYTES_6;

				CANTxData[0]=ID;
				CANTxData[1]=i;
				CANTxData[2]=(canErrors[i]>>24)&0xFF;
				CANTxData[3]=(canErrors[i]>>16)&0xFF;
				CANTxData[4]=(canErrors[i]>>8)&0xFF;
				CANTxData[5]=(canErrors[i]>>0)&0xFF;

				TxHeader.IdType = FDCAN_STANDARD_ID;
				TxHeader.TxFrameType = FDCAN_DATA_FRAME;
				TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
				TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
				TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
				TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
				TxHeader.MessageMarker = 0;

				if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan, &TxHeader, CANTxData) != HAL_OK)
				{
					Set_Error(ERR_SEND_FAILED);
					return;
				}
				else
				{
					//if we sent the error message clear the error so that if it only occurs once the error is not sent continuously
					canErrors[i]=0;
					canErrorToTransmit &= ~(1<<i);
				}
			}
		}
	}
}


void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
	RCC_OscInitStruct.PLL.PLLN = 85;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_8) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12|RCC_PERIPHCLK_FDCAN;
	PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_HSE;
	PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_ADC1_Init(void)
{
	ADC_MultiModeTypeDef multimode = {0};
	ADC_ChannelConfTypeDef sConfig = {0};

	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_10B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.GainCompensation = 0;
	hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 3;
	hadc1.Init.DiscontinuousConvMode = ENABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	hadc1.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}

	multimode.Mode = ADC_DUALMODE_REGSIMULT;
	multimode.DMAAccessMode = ADC_DMAACCESSMODE_12_10_BITS;
	multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_12CYCLES;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
	{
		Error_Handler();
	}


	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	sConfig.SamplingTime = ADC_SAMPLETIME_24CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_ADC2_Init(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};

	hadc2.Instance = ADC2;
	hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc2.Init.Resolution = ADC_RESOLUTION_10B;
	hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc2.Init.GainCompensation = 0;
	hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc2.Init.LowPowerAutoWait = DISABLE;
	hadc2.Init.ContinuousConvMode = DISABLE;
	hadc2.Init.NbrOfConversion = 3;
	hadc2.Init.DiscontinuousConvMode = ENABLE;
	hadc2.Init.DMAContinuousRequests = ENABLE;
	hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	hadc2.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc2) != HAL_OK)
	{
		Error_Handler();
	}


	sConfig.Channel = ADC_CHANNEL_10;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_24CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sConfig.Rank = ADC_REGULAR_RANK_3;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_FDCAN_Init(void)
{
	FDCAN_FilterTypeDef	sFilterConfig;

	hfdcan.Instance = FDCAN1;
	hfdcan.Init.ClockDivider = FDCAN_CLOCK_DIV1;
	hfdcan.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
	hfdcan.Init.Mode = FDCAN_MODE_NORMAL;
	hfdcan.Init.AutoRetransmission = DISABLE;
	hfdcan.Init.TransmitPause = DISABLE;
	hfdcan.Init.ProtocolException = DISABLE;
	hfdcan.Init.NominalPrescaler = 1;
	hfdcan.Init.NominalSyncJumpWidth = 1;
	hfdcan.Init.NominalTimeSeg1 = 13;
	hfdcan.Init.NominalTimeSeg2 = 2;
	hfdcan.Init.DataPrescaler = 1;
	hfdcan.Init.DataSyncJumpWidth = 1;
	hfdcan.Init.DataTimeSeg1 = 1;
	hfdcan.Init.DataTimeSeg2 = 1;
	hfdcan.Init.StdFiltersNbr = 2;
	hfdcan.Init.ExtFiltersNbr = 0;
	hfdcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	if (HAL_FDCAN_Init(&hfdcan) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_FDCAN_ConfigRxFifoOverwrite(&hfdcan, FDCAN_RX_FIFO0, FDCAN_RX_FIFO_OVERWRITE) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_FDCAN_ConfigRxFifoOverwrite(&hfdcan, FDCAN_RX_FIFO1, FDCAN_RX_FIFO_OVERWRITE) != HAL_OK)
	{
		Error_Handler();
	}

	//only accept config/request can messages and sync can messages
	sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 0;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = CANID_CONFIG;
	sFilterConfig.FilterID2 = 0x7FF;
	if (HAL_FDCAN_ConfigFilter(&hfdcan, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 1;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = CANID_SYNC;
	sFilterConfig.FilterID2 = 0x7FF;
	if (HAL_FDCAN_ConfigFilter(&hfdcan, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE) != HAL_OK)
	{
		Error_Handler();
	}

	if(HAL_FDCAN_Start(&hfdcan) != HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_FDCAN_ActivateNotification(&hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_DMA_Init(void) 
{
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
}

static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	//led pin, sel pins, and in pins should always be outputs
	//led pin and sel pins should always start low, in pins should start based on configuration
	HAL_GPIO_WritePin(LED.PORT, LED.PIN, 0);
	HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 0);
	HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 0);
	HAL_GPIO_WritePin(U5IN0.PORT, U5IN0.PIN, (Default_Switch_State>>0)&0b1);
	HAL_GPIO_WritePin(U5IN1.PORT, U5IN1.PIN, (Default_Switch_State>>1)&0b1);
	HAL_GPIO_WritePin(U6IN0.PORT, U6IN0.PIN, (Default_Switch_State>>2)&0b1);
	HAL_GPIO_WritePin(U6IN1.PORT, U6IN1.PIN, (Default_Switch_State>>3)&0b1);
	HAL_GPIO_WritePin(U7IN0.PORT, U7IN0.PIN, (Default_Switch_State>>4)&0b1);
	HAL_GPIO_WritePin(U7IN1.PORT, U7IN1.PIN, (Default_Switch_State>>5)&0b1);

	//all outputs on portA assigned here
	GPIO_InitStruct.Pin = LED.PIN|SEL0.PIN|U5IN0.PIN|U5IN1.PIN|U6IN0.PIN|U6IN1.PIN|U7IN0.PIN|U7IN1.PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//all outputs on portB assigned here
	GPIO_InitStruct.Pin = SEL1.PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


	if(Digital_In_EN && (1<<0))
	{
		HAL_GPIO_WritePin(DIO3.PORT, DIO3.PIN, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = DIO3.PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DIO3.PORT, &GPIO_InitStruct);
	}
	else if (1) //TODO
	{
		//here we configure pins intended for PWM purposes
	}

	if(Digital_In_EN && (1<<1))
	{
		HAL_GPIO_WritePin(DIO4.PORT, DIO4.PIN, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = DIO4.PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DIO4.PORT, &GPIO_InitStruct);
	}
	else if (1)
	{
		//here we configure pins intended for PWM purposes
	}

	if(Digital_In_EN && (1<<2))
	{
		HAL_GPIO_WritePin(DIO5.PORT, DIO5.PIN, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = DIO5.PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DIO5.PORT, &GPIO_InitStruct);
	}
	else if (1)
	{
		//here we configure pins intended for PWM purposes
	}

	if(Digital_In_EN && (1<<3))
	{
		HAL_GPIO_WritePin(DIO6.PORT, DIO6.PIN, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = DIO6.PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DIO6.PORT, &GPIO_InitStruct);
	}
	else if (1)
	{
		//here we configure pins intended for PWM purposes
	}

	if(Digital_In_EN && (1<<4))
	{
		HAL_GPIO_WritePin(DIO15.PORT, DIO15.PIN, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = DIO15.PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DIO15.PORT, &GPIO_InitStruct);
	}
	else if (1)
	{
		//here we configure pins intended for PWM purposes
	}
}

static void MX_TIM6_Init(void)
{
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 16999;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = Can_Interval;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_TIM7_Init(void)
{
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = 16999;
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = ERROR_PERIOD_100US;
	htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
	{
		Error_Handler();
	}
}


void Error_Handler(void)
{
	while(1)
	{
		HAL_GPIO_TogglePin(LED.PORT, LED.PIN);
		HAL_Delay(33);
	}
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{ 

}
#endif
