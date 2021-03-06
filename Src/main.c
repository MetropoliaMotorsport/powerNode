#include "main.h"


//static function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_FDCAN_Init(void);
static void MX_TIM1_Init(void); //regular mode timers first
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM15_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM17_Init(void); //output only pwm timer next
static void MX_TIM3_Init(void); //this has functionality for pwm input, but digital input doesn't even work, so probably pwm input won't work
static void MX_TIM2_Init(void); //pwm io timers last
static void MX_TIM4_Init(void);
static void MX_TIM8_Init(void);
static void MX_LPTIM1_Init(void);

//type handlers
FDCAN_HandleTypeDef hfdcan;
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;
LPTIM_HandleTypeDef hlptim1;

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
uint8_t Digital_In_EN;
uint8_t Digital_In_Interrupt_EN;
uint8_t Digital_In_Interrupt_Can_Rising[5];
uint8_t Digital_In_Interrupt_Can_Falling[5];
uint8_t Digital_In_Interrupt_Power_High_Rising[5];
uint8_t Digital_In_Interrupt_Power_High_Falling[5];
uint8_t Digital_In_Interrupt_Power_Low_Rising[5];
uint8_t Digital_In_Interrupt_Power_Low_Falling[5];
uint8_t Digital_In_Interrupt_PWM_Rising[5];
uint8_t Digital_In_Interrupt_PWM_Falling[5];

uint8_t PWM_Out_EN;
uint16_t PWM_Prescalers[5];
uint16_t PWM_Pulses[5];

uint8_t PWM_In_EN;

uint8_t Default_Switch_State;

uint16_t Can_IDs[8];
uint8_t Can_DLCs[8];
uint8_t Can_Config_Bytes[8][8];
uint8_t Can_Config_Datas[8][8];
uint8_t Can_Sync_Enable;
uint8_t Can_Timed_Enable;
uint16_t Can_Interval;
uint16_t Can_Sync_Delay;

//voltage limits are in mV, temperature limits are in °C, current limits are in 100's of uAs
uint16_t warn_undervoltage_U5;
uint16_t warn_overvoltage_U5;
uint16_t warn_undertemperature_U5;
uint16_t warn_overtemperature_U5;
uint16_t warn_undercurrent_U5I0;
uint16_t warn_overcurrent_U5I0;
uint16_t warn_undercurrent_U5I1;
uint16_t warn_overcurrent_U5I1;
uint16_t cutoff_overcurrent_U5I0;
uint16_t cutoff_overcurrent_U5I1;
uint16_t warn_undervoltage_U6;
uint16_t warn_overvoltage_U6;
uint16_t warn_undertemperature_U6;
uint16_t warn_overtemperature_U6;
uint16_t warn_undercurrent_U6I0;
uint16_t warn_overcurrent_U6I0;
uint16_t warn_undercurrent_U6I1;
uint16_t warn_overcurrent_U6I1;
uint16_t cutoff_overcurrent_U6I0;
uint16_t cutoff_overcurrent_U6I1;
uint16_t warn_undervoltage_U7;
uint16_t warn_overvoltage_U7;
uint16_t warn_undertemperature_U7;
uint16_t warn_overtemperature_U7;
uint16_t warn_undercurrent_U7I0;
uint16_t warn_overcurrent_U7I0;
uint16_t warn_undercurrent_U7I1;
uint16_t warn_overcurrent_U7I1;
uint16_t cutoff_overcurrent_U7I0;
uint16_t cutoff_overcurrent_U7I1;
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

uint32_t I0_rolling_average_position;
uint32_t I1_rolling_average_position;
uint32_t T_rolling_average_position;
uint32_t V_rolling_average_position;
uint32_t adc_selection;
uint32_t ADCDualConvertedValues[3];

uint8_t CANTxData[8];

uint8_t CanBuffer[31] = {[0 ... 30]=255};
uint8_t CanMessagesToSend;
uint8_t CanBufferReadPos;
uint8_t CanBufferWritePos;

uint8_t CanTimerFlag;
uint8_t CanSyncFlag;

uint16_t SampleTemperatureVoltagePeriod;
uint8_t SampleTemperatureBurst;
uint8_t SampleVoltageBurst;

//for these 255 means enable continuously while lower numbers mean to take that many samples
uint32_t sample_temperature;
uint32_t sample_voltage;

uint32_t GPIO_States[5];
uint32_t GPIO_Current_Interrupts;
uint32_t GPIO_Interrupt[5]; //shouldn't be more interrupts pending than digital inputs, this is used to keep track of order
uint32_t GPIO_Interrupt_Active[5]; //and this is used to keep track of if the interrupt is already active
uint32_t GPIO_Interrupt_Write_Pos;
uint32_t GPIO_Interrupt_Read_Pos;
uint32_t GPIO_Timer_Ready = 1;

uint8_t Check_I0_Flag; //note this is only for warning, max current is checked in the interrupt
uint8_t Check_I1_Flag;
uint8_t Check_T_Flag;
uint8_t Check_V_Flag;

uint8_t U5I0_active;
uint8_t U5I1_active;
uint8_t U6I0_active;
uint8_t U6I1_active;
uint8_t U7I0_active;
uint8_t U7I1_active;

uint8_t U5I0_error;
uint8_t U5I1_error;
uint8_t U6I0_error;
uint8_t U6I1_error;
uint8_t U7I0_error;
uint8_t U7I1_error;


uint8_t CheckCanError( void )
{
	FDCAN_ProtocolStatusTypeDef CAN1Status;

	static uint8_t offcan1 = 0;

	HAL_FDCAN_GetProtocolStatus(&hfdcan, &CAN1Status);

	static uint8_t offcan = 0;

	if ( !offcan1 && CAN1Status.BusOff) // detect passive error instead and try to stay off bus till clears?
	{
		  HAL_FDCAN_Stop(&hfdcan);
		  Set_Error(ERR_CANOFFLINE);
		  // set LED.
		  offcan = 1;
		  return 0;
	}

	// use the senderrorflag to only try once a second to get back onbus.
	if ( CAN1Status.BusOff && canSendErrorFlag )
	{
		if (HAL_FDCAN_Start(&hfdcan) == HAL_OK)
		{
			offcan = 0;
		}
	}

	return offcan;
}

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

	MX_TIM1_Init(); //""  ""
	MX_TIM6_Init(); //initialize unnecessary timers to avoid error handler being called when configuration is changed
	MX_TIM7_Init();
	MX_TIM15_Init();
	MX_TIM16_Init(); //""  ""

	MX_TIM17_Init();
	MX_TIM3_Init();

	MX_TIM2_Init();
	MX_TIM4_Init();
	MX_TIM8_Init();

	MX_LPTIM1_Init();

	//start everything that can generate interrupts after initialization is done
	HAL_TIM_Base_Start_IT(&htim1);
	if (Can_Timed_Enable) { HAL_TIM_Base_Start_IT(&htim6); }
	HAL_TIM_Base_Start_IT(&htim7);

	//start pwm input channels if they are enabled
	if ((PWM_In_EN>>0)&1) { HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); }
	if ((PWM_In_EN>>3)&1) { HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2); HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); }
	if ((PWM_In_EN>>4)&1) { HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2); HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1); }

	//this timer starts the adc, so start it last
	HAL_TIM_Base_Start_IT(&htim15);

	Can_Sync();

	while(1)
	{
		if ( ! CheckCanError() ) // don't try to do anything canbus related if it's offline.
		{
			if (CanSyncFlag)
			{
				Can_Sync();
				CanSyncFlag=0;
			}

			if(canErrorToTransmit && canSendErrorFlag)
			{
				Send_Error();
				if(!canErrorToTransmit)
				{
					canSendErrorFlag=0;
				}
			}

			if(CanTimerFlag)
			{
				for(uint32_t i=0; i<8; i++)
				{
					if ((Can_Timed_Enable>>i)&0b1)
					{
						Buffer_Can_Message(i);
					}
				}
				CanTimerFlag=0;
			}

			if (CanMessagesToSend)
			{
				//only put one thing to the fifo at a time so that sync message can be put to the front of the fifo
				if (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan) > 2)
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
		}

		if (Check_I0_Flag)
		{
			Check_I0_Warn();

			Check_I0_Flag=0;
		}
		if (Check_I1_Flag)
		{
			Check_I1_Warn();

			Check_I1_Flag=0;
		}
		if (Check_T_Flag)
		{
			Check_T_Warn();

			Check_T_Flag=0;
		}
		if (Check_V_Flag)
		{
			Check_V_Warn();

			Check_V_Flag=0;
		}

		if (U5I0_error) { Set_Error(U5I0_SWITCH_OFF); }
		if (U5I1_error) { Set_Error(U5I1_SWITCH_OFF); }
		if (U6I0_error) { Set_Error(U6I0_SWITCH_OFF); }
		if (U6I1_error) { Set_Error(U6I1_SWITCH_OFF); }
		if (U7I0_error) { Set_Error(U7I0_SWITCH_OFF); }
		if (U7I1_error) { Set_Error(U7I1_SWITCH_OFF); }
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM15)
	{
		HAL_TIM_Base_Stop_IT(&htim15);
	    if (HAL_ADCEx_MultiModeStart_DMA(&hadc1, ADCDualConvertedValues, 3) != HAL_OK)
	    {
	    	Error_Handler();
	    }
	}
	else if (htim->Instance == TIM16)
	{
		HAL_TIM_Base_Stop_IT(&htim16);
		CanSyncFlag=1;
	}
	else if (htim->Instance == TIM1)
	{
		sample_temperature+=SampleTemperatureBurst;
		if(sample_temperature>255) //in this case sample_temperature should be continuous or it should be measured slower
		{
			Set_Error(WARN_TEMP_MEASURE_OVERFLOW);
		}

		sample_voltage+=SampleVoltageBurst;
		if(sample_voltage>255)
		{
			Set_Error(WARN_VOLT_MEASURE_OVERFLOW);
		}
	}
	else if (htim->Instance == TIM7)
	{
		canSendErrorFlag=1;
	}
	else if (htim->Instance == TIM6)
	{
		CanTimerFlag=1;
	}
	else
	{
		Error_Handler();
	}
}

extern const pinPort *switches[];
extern uint8_t *actives[];

//TODO: this could be modified to not wait another ms if one input occurs halfway through the timer of another input HAL_LPTIM_ReadCounter(&lptim1)
void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
	if (hlptim->Instance == LPTIM1)
	{
		if (HAL_LPTIM_TimeOut_Stop_IT(&hlptim1) != HAL_OK)
		{
			Error_Handler();
		}

		uint32_t pinState;

		switch(GPIO_Interrupt[GPIO_Interrupt_Read_Pos])
		{
		case 0:
			pinState = HAL_GPIO_ReadPin(DIO3.PORT, DIO3.PIN);
			break;
		case 2:
			pinState = HAL_GPIO_ReadPin(DIO5.PORT, DIO5.PIN);
			break;
		case 3:
			pinState = HAL_GPIO_ReadPin(DIO6.PORT, DIO6.PIN);
			break;
		case 4:
			pinState = HAL_GPIO_ReadPin(DIO15.PORT, DIO15.PIN);
			break;
		default:
			Set_Error(WARN_UNDEFINED_GPIO);
			break;
		}

		if(pinState != GPIO_States[GPIO_Interrupt[GPIO_Interrupt_Read_Pos]])
		{
			GPIO_States[GPIO_Interrupt[GPIO_Interrupt_Read_Pos]]=pinState;
			switch(pinState)
			{
			case 1: //rising edge detected

				for(uint32_t i=0; i<8; i++)
				{
					if (Digital_In_Interrupt_Can_Rising[GPIO_Interrupt[GPIO_Interrupt_Read_Pos]]>>i & 1)
					{
						Can_Send(i);
					}
				}

				for(uint32_t i=0; i<6; i++)
				{
					if (Digital_In_Interrupt_Power_Low_Rising[GPIO_Interrupt[GPIO_Interrupt_Read_Pos]]>>i & 1)
					{
						HAL_GPIO_WritePin(switches[i]->PORT, switches[i]->PIN, 0);
						*actives[i]=0;
					}
					else if (Digital_In_Interrupt_Power_High_Rising[GPIO_Interrupt[GPIO_Interrupt_Read_Pos]]>>i & 1)
					{
						HAL_GPIO_WritePin(switches[i]->PORT, switches[i]->PIN, 1);
						*actives[i]=1;
					}
				}

				//GPIO_Interrupt[GPIO_Interrupt_Read_Pos]
				//Digital_In_Interrupt_PWM_Rising[5] //TODO

				break;
			case 0: //falling edge detected

				for(uint32_t i=0; i<8; i++)
				{
					if (Digital_In_Interrupt_Can_Falling[GPIO_Interrupt[GPIO_Interrupt_Read_Pos]]>>i & 1)
					{
						Buffer_Can_Message(i);
					}
				}

				for(uint32_t i=0; i<6; i++)
				{
					if (Digital_In_Interrupt_Power_Low_Falling[GPIO_Interrupt[GPIO_Interrupt_Read_Pos]]>>i & 1)
					{
						HAL_GPIO_WritePin(switches[i]->PORT, switches[i]->PIN, 0);
						*actives[i]=0;
					}
					else if (Digital_In_Interrupt_Power_High_Falling[GPIO_Interrupt[GPIO_Interrupt_Read_Pos]]>>i & 1)
					{
						HAL_GPIO_WritePin(switches[i]->PORT, switches[i]->PIN, 1);
						*actives[i]=1;
					}
				}

//TODO: PWM

				break;
			default:
				//should never reach this point
				break;
			}
		}

		GPIO_Current_Interrupts--;
		GPIO_Interrupt_Active[GPIO_Interrupt[GPIO_Interrupt_Read_Pos]]=0;
		GPIO_Interrupt_Read_Pos++;
		if (GPIO_Interrupt_Read_Pos>=5) { GPIO_Interrupt_Read_Pos=0; }

		if(GPIO_Current_Interrupts)
		{
			//start the timer again for the next channel
			if (HAL_LPTIM_TimeOut_Start_IT(&hlptim1, 13282, 13283) != HAL_OK) //~1ms debounce time
			{
				Error_Handler();
			}
		}
		else
		{
			GPIO_Timer_Ready=1;
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint32_t wrote=0;

	if(GPIO_Current_Interrupts)
	{
		switch (GPIO_Pin)
		{
		case (1<<3):
			if (!GPIO_Interrupt_Active[0])
			{
				GPIO_Interrupt_Active[0]=1;
				GPIO_Interrupt[GPIO_Interrupt_Write_Pos]=0;
				wrote=1;
			}
			break;
		case (1<<5):
			if (!GPIO_Interrupt_Active[2])
			{
				GPIO_Interrupt_Active[2]=1;
				GPIO_Interrupt[GPIO_Interrupt_Write_Pos]=2;
				wrote=1;
			}
			break;
		case (1<<6):
			if (!GPIO_Interrupt_Active[3])
			{
				GPIO_Interrupt_Active[3]=1;
				GPIO_Interrupt[GPIO_Interrupt_Write_Pos]=3;
				wrote=1;
			}
			break;
		case (1<<15):
			if (!GPIO_Interrupt_Active[3])
			{
				GPIO_Interrupt_Active[3]=1;
				GPIO_Interrupt[GPIO_Interrupt_Write_Pos]=2;
				wrote=1;
			}
			break;
		default:
			Set_Error(WARN_UNDEFINED_GPIO);
		break;
		}
	}
	else
	{
		wrote=1;

		//this case should only execute when there are no pending GPIO interrupts
		switch (GPIO_Pin)
		{
		case (1<<3):
			GPIO_Interrupt_Active[0]=1;
			GPIO_Interrupt[GPIO_Interrupt_Write_Pos]=0;
			break;
		case (1<<5):
			GPIO_Interrupt_Active[2]=1;
			GPIO_Interrupt[GPIO_Interrupt_Write_Pos]=2;
			break;
		case (1<<6):
			GPIO_Interrupt_Active[3]=1;
			GPIO_Interrupt[GPIO_Interrupt_Write_Pos]=3;
			break;
		case (1<<15):
			GPIO_Interrupt_Active[4]=1;
			GPIO_Interrupt[GPIO_Interrupt_Write_Pos]=15;
			break;
		default:
			Set_Error(WARN_UNDEFINED_GPIO);
		break;
		}

		if (GPIO_Timer_Ready)
		{
			GPIO_Timer_Ready=0;
			if (HAL_LPTIM_TimeOut_Start_IT(&hlptim1, 13282, 13283) != HAL_OK) //~1ms debounce time
			{
				Error_Handler();
			}
		}
	}

	if(wrote)
	{
		GPIO_Current_Interrupts++;
		GPIO_Interrupt_Write_Pos++;
		if (GPIO_Interrupt_Write_Pos>=5) { GPIO_Interrupt_Write_Pos=0; }
	}
}


//externs needed for here
extern uint8_t U5I0_active_counter;
extern uint8_t U5I1_active_counter;
extern uint8_t U6I0_active_counter;
extern uint8_t U6I1_active_counter;
extern uint8_t U7I0_active_counter;
extern uint8_t U7I1_active_counter;

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

		HAL_TIM_Base_Start_IT(&htim15);

		for(uint32_t i=0; i<3; i++)
		{
			slaveConvertedValue[i]=(ADCDualConvertedValues[i]>>16)&0xFFFF;
			masterConvertedValue[i]=ADCDualConvertedValues[i]&0xFFFF;
			/*if(masterConvertedValue[i]>slaveConvertedValue[i])
			{
				convertedValue[i]=masterConvertedValue[i]-slaveConvertedValue[i];
			}
			else //this should only happen due to error in reading at low voltages
			{
				convertedValue[i]=0;
			}*/
			convertedValue[i]=masterConvertedValue[i]; //at least for current this seems much more stable and ground voltage is almost constant anyways, so this gives much better reading for current it seems (though it shouoldn't, not sure why)
		}

		if (HAL_ADCEx_MultiModeStop_DMA(&hadc1) != HAL_OK)
		{
			Error_Handler();
		}

		//initialie all values to avoid incorrect huge values being read
		uint32_t U5I0_raw=0; uint32_t U6I0_raw=0; uint32_t U7I0_raw=0;
		uint32_t U5I1_raw=0; uint32_t U6I1_raw=1; uint32_t U7I1_raw=0;

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

			for(uint32_t i=0; i<I_ROLLING_AVERAGE; i++) //this has possibility to overflow if ROLLING_AVERAGE > 2^(32-10) (reading 10 bit value)
			{
				U5I0_raw+=U5I0[i];
				U6I0_raw+=U6I0[i];
				U7I0_raw+=U7I0[i];
			}
			U5I0_raw/=I_ROLLING_AVERAGE; U6I0_raw/=I_ROLLING_AVERAGE; U7I0_raw/=I_ROLLING_AVERAGE;
			U5I0_real=Parse_Current(U5I0_raw, 0, U5I0_active); U6I0_real=Parse_Current(U6I0_raw, 2, U6I1_active); U7I0_real=Parse_Current(U7I0_raw, 4, U7I0_active);

			//error states
			if (U5I0_active && U5I0_raw>560 && U5I0_active_counter>I_ROLLING_AVERAGE)
			{
				U5I0_error=1;
				HAL_GPIO_WritePin(U5IN0.PORT, U5IN0.PIN, 0);
				U5I0_active=0;
			}
			if (U6I0_active && U6I0_raw>560 && U6I0_active_counter>I_ROLLING_AVERAGE)
			{
				U6I0_error=1;
				HAL_GPIO_WritePin(U6IN0.PORT, U6IN0.PIN, 0);
				U6I0_active=0;
			}
			if (U7I0_active && U7I0_raw>560 && U7I0_active_counter>I_ROLLING_AVERAGE)
			{
				U7I0_error=1;
				HAL_GPIO_WritePin(U7IN0.PORT, U7IN0.PIN, 0);
				U7I0_active=0;
			}

			Check_I0_Switch();
			Check_I0_Flag=1;

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

			for(uint32_t i=0; i<I_ROLLING_AVERAGE; i++) //this has possibility to overflow if ROLLING_AVERAGE > 2^(32-10) (reading 10 bit value)
			{
				U5I1_raw+=U5I1[i];
				U6I1_raw+=U6I1[i];
				U7I1_raw+=U7I1[i];
			}
			U5I1_raw/=I_ROLLING_AVERAGE; U6I1_raw/=I_ROLLING_AVERAGE; U7I1_raw/=I_ROLLING_AVERAGE;
			U5I1_real=Parse_Current(U5I1_raw, 1, U5I1_active); U6I1_real=Parse_Current(U6I1_raw, 3, U6I1_active); U7I1_real=Parse_Current(U7I1_raw, 5, U7I1_active);


			//error states
			if (U5I1_active && U5I1_raw>560 && U5I1_active_counter>I_ROLLING_AVERAGE)
			{
				U5I1_error=1;
				HAL_GPIO_WritePin(U5IN1.PORT, U5IN1.PIN, 0);
				U5I1_active=0;
			}
			if (U6I1_active && U6I1_raw>560 && U6I1_active_counter>I_ROLLING_AVERAGE)
			{
				U6I1_error=1;
				HAL_GPIO_WritePin(U6IN1.PORT, U6IN0.PIN, 0);
				U6I1_active=0;
			}
			if (U7I1_active && U7I1_raw>560 && U7I1_active_counter>I_ROLLING_AVERAGE)
			{
				U7I1_error=1;
				HAL_GPIO_WritePin(U7IN1.PORT, U7IN1.PIN, 0);
				U7I1_active=0;
			}

			Check_I1_Switch();
			Check_I1_Flag=1;

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

			Check_T_Flag=1;

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

			Check_V_Flag=1;

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

//for can sync we must send from the interrupt to ensure that we send the message quickly, but this means only 2 (or sometimes 3) messages may be sent without discarding messages
void Can_Sync(void)
{
	for(uint32_t i=0; i<8; i++)
	{
		if ((Can_Sync_Enable>>i)&0b1)
		{
			if (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan) > 0)
			{
				Can_Send(i);
			}
		}
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
			if(Can_Sync_Delay)
			{
				HAL_TIM_Base_Start_IT(&htim16);
			}
			else
			{
				CanSyncFlag = 1;
			}
		}
		else if (RxHeader.Identifier == CANID_CONFIG)
		{
			if(CANRxData[0] == ID)
			{
				switch(CANRxData[1])
				{
				case SWITCH_POWER:
					Switch_Power(CANRxData[2], CANRxData[3]);
					if ((RxHeader.DataLength>>16) < 4) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CHANGE_DC:
					Switch_DC(CANRxData[2], CANRxData);
					if ((RxHeader.DataLength>>16) < (3+((CANRxData[2]>>4)&1))+((CANRxData[2]>>3)&1)+((CANRxData[2]>>2)&1)+((CANRxData[2]>>1)&1)+((CANRxData[2]>>0)&1)) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CAN_MESS0:
					Buffer_Can_Message(0);
					break;
				case CAN_MESS1:
					Buffer_Can_Message(1);
					break;
				case CAN_MESS2:
					Buffer_Can_Message(2);
					break;
				case CAN_MESS3:
					Buffer_Can_Message(3);
					break;
				case CAN_MESS4:
					Buffer_Can_Message(4);
					break;
				case CAN_MESS5:
					Buffer_Can_Message(5);
					break;
				case CAN_MESS6:
					Buffer_Can_Message(6);
					break;
				case CAN_MESS7:
					Buffer_Can_Message(7);
					break;
				case SAMPLE_TEMP_VOLT:
					Sample_Temperature_Voltage(CANRxData[2], CANRxData[3]);
					if ((RxHeader.DataLength>>16) < 4) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CLEAR_ERROR:
					Clear_Error(CANRxData[2]);
					if ((RxHeader.DataLength>>16) < 3) { Set_Error(ERR_COMMAND_SHORT); }
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
				case CONFIG_CAN_SYNC_DELAY:
					Config_Can_Sync_Delay((((uint16_t)CANRxData[2])<<8)+(((uint16_t)CANRxData[3])<<0));
					if ((RxHeader.DataLength>>16) < 4) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CONFIG_CAN_TV_READING:
					Config_Temperature_Voltage_Reading((((uint16_t)CANRxData[2])<<8)+(((uint16_t)CANRxData[3])<<0), CANRxData[4], CANRxData[5]);
					if ((RxHeader.DataLength>>16) < 6) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CONFIG_DEFAULT_DC:
					Config_Default_DC(CANRxData[2], CANRxData);
					if ((RxHeader.DataLength>>16) < (3+((CANRxData[2]>>4)&1))+((CANRxData[2]>>3)&1)+((CANRxData[2]>>2)&1)+((CANRxData[2]>>1)&1)+((CANRxData[2]>>0)&1)) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CONFIG_PWM_PRESCALERS:
					Config_PWM_Prescalers(CANRxData[2], CANRxData);
					if ((RxHeader.DataLength>>16) < (3+((CANRxData[2]>>4)&1))+((CANRxData[2]>>3)&1)+((CANRxData[2]>>2)&1)+((CANRxData[2]>>1)&1)+((CANRxData[2]>>0)&1)) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CONFIG_DIO:
					Config_DIO_Pins(CANRxData[2], CANRxData[3], CANRxData[4], CANRxData[5]);
					if ((RxHeader.DataLength>>16) < 6) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CONFIG_INTERRUPT_POWER:
					Config_Interrupt_Power(CANRxData[2], CANRxData[3], CANRxData[4], CANRxData[5], CANRxData[6]);
					if ((RxHeader.DataLength>>16) < 7) { Set_Error(ERR_COMMAND_SHORT); }
					break;
				case CONFIG_INTERRUPT_CAN:
					Config_Interrupt_Can(CANRxData[2], CANRxData[3], CANRxData[4]);
					if ((RxHeader.DataLength>>16) < 5) { Set_Error(ERR_COMMAND_SHORT); }
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

void Write_PWM(uint32_t DIO_channel, uint16_t pulse) //TODO: make sure the pwm signal works nicely in real life
{
	if(PWM_Out_EN&(1<<DIO_channel))
	{
		TIM_HandleTypeDef htim = {0};
		uint32_t channel = 0;
		TIM_OC_InitTypeDef sConfigOC = {0};

		if(pulse>255) { pulse=255; }

		switch(DIO_channel)
		{
			case 0: //PB3
				htim = htim2;
				channel = TIM_CHANNEL_2;
				break;
			case 1: //PB4
				htim = htim3;
				channel = TIM_CHANNEL_1;
				break;
			case 2: //PB5
				htim = htim17;
				channel = TIM_CHANNEL_1;
				break;
			case 3: //PB6
				htim = htim4;
				channel = TIM_CHANNEL_1;
				break;
			case 4: //PA15
				htim = htim8;
				channel = TIM_CHANNEL_1;
				break;
			default:
				Set_Error(WARN_PWM_INVALID_CHANNEL);
				return;
		}

		if (HAL_TIM_PWM_Stop(&htim, channel) != HAL_OK)
		{
			Error_Handler();
		}
		/*htim.Init.Period = 255;
		HAL_TIM_PWM_Init(&htim);*/
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = pulse;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		if (HAL_TIM_PWM_ConfigChannel(&htim, &sConfigOC, channel) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_TIM_PWM_Start(&htim, channel) != HAL_OK)
		{
			Error_Handler();
		}
	}
	else
	{
		Set_Error(WARN_PWM_NOT_ENABLED);
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
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV8;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
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
	sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
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
	hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV8;
	hadc2.Init.Resolution = ADC_RESOLUTION_12B;
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
	sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
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
	hfdcan.Init.AutoRetransmission = ENABLE;
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
	HAL_GPIO_WritePin(U5IN0.PORT, U5IN0.PIN, (Default_Switch_State>>0)&0b1); U5I0_active = (Default_Switch_State>>0)&0b1;
	HAL_GPIO_WritePin(U5IN1.PORT, U5IN1.PIN, (Default_Switch_State>>1)&0b1); U5I1_active = (Default_Switch_State>>0)&0b1;
	HAL_GPIO_WritePin(U6IN0.PORT, U6IN0.PIN, (Default_Switch_State>>2)&0b1); U6I0_active = (Default_Switch_State>>0)&0b1;
	HAL_GPIO_WritePin(U6IN1.PORT, U6IN1.PIN, (Default_Switch_State>>3)&0b1); U6I1_active = (Default_Switch_State>>0)&0b1;
	HAL_GPIO_WritePin(U7IN0.PORT, U7IN0.PIN, (Default_Switch_State>>4)&0b1); U7I0_active = (Default_Switch_State>>0)&0b1;
	HAL_GPIO_WritePin(U7IN1.PORT, U7IN1.PIN, (Default_Switch_State>>5)&0b1); U7I1_active = (Default_Switch_State>>0)&0b1;

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
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DIO3.PORT, &GPIO_InitStruct);
	}

	if(Digital_In_EN && (1<<1))
	{
		HAL_GPIO_WritePin(DIO4.PORT, DIO4.PIN, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = DIO4.PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DIO4.PORT, &GPIO_InitStruct);
	}

	if(Digital_In_EN && (1<<2))
	{
		HAL_GPIO_WritePin(DIO5.PORT, DIO5.PIN, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = DIO5.PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DIO5.PORT, &GPIO_InitStruct);
	}

	if(Digital_In_EN && (1<<3))
	{
		HAL_GPIO_WritePin(DIO6.PORT, DIO6.PIN, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = DIO6.PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DIO6.PORT, &GPIO_InitStruct);
	}

	if(Digital_In_EN && (1<<4))
	{
		HAL_GPIO_WritePin(DIO15.PORT, DIO15.PIN, GPIO_PIN_RESET);
		GPIO_InitStruct.Pin = DIO15.PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DIO15.PORT, &GPIO_InitStruct);
	}

	if(Digital_In_EN && (1<<0)) { GPIO_States[0] = HAL_GPIO_ReadPin(DIO3.PORT, DIO3.PIN); }
	if(Digital_In_EN && (1<<2)) { GPIO_States[0] = HAL_GPIO_ReadPin(DIO5.PORT, DIO5.PIN); }
	if(Digital_In_EN && (1<<3)) { GPIO_States[0] = HAL_GPIO_ReadPin(DIO6.PORT, DIO6.PIN); }
	if(Digital_In_EN && (1<<4)) { GPIO_States[0] = HAL_GPIO_ReadPin(DIO15.PORT, DIO15.PIN); }

	//interrupts for PB3, PB5, PB6, PA15; PB4 will maybe have a different interrupt enable
	if (Digital_In_EN && (1<<0))
	{
		HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	}

	if ((Digital_In_EN && (1<<2)) || (Digital_In_EN && (1<<3)))
	{
		HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	}

	if (Digital_In_EN && (1<<4))
	{
		HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
}


static void MX_TIM1_Init(void)
{
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 16999; //resolution in 100's of us
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = SampleTemperatureVoltagePeriod;
	if (HAL_TIM_Base_Init(&htim1) !=HAL_OK)
	{
		Error_Handler();
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

static void MX_TIM15_Init(void)
{
	htim15.Instance = TIM15;
	htim15.Init.Prescaler = 16;
	htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim15.Init.Period = 1000; //this corresponds to 100us, as going at max speed (20us) seems to be too fast
	if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_TIM16_Init(void)
{
	htim16.Instance = TIM16;
	htim16.Init.Prescaler = 1699; //10us resolution
	htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim16.Init.Period = Can_Sync_Delay;
	htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_TIM17_Init(void)
{
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim17.Instance = TIM17;
	htim17.Init.Prescaler = PWM_Prescalers[2];
	htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim17.Init.Period = 255; //pulse will be from 0 to 255
	htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim17.Init.RepetitionCounter = 0;
	htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
	{
		Error_Handler();
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = PWM_Pulses[2];
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_TIM_MspPostInit(&htim17);

	if(PWM_Out_EN&(1<<2))
	{
		if (HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

static void MX_TIM2_Init()
{
	if (!((PWM_In_EN>>0)&1)) //default if pin is not used as pwm input
	{
		TIM_OC_InitTypeDef sConfigOC = {0};

		htim2.Instance = TIM2;
		htim2.Init.Prescaler = PWM_Prescalers[0];
		htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim2.Init.Period = 255;
		htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
		{
			Error_Handler();
		}

		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = PWM_Pulses[0];
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
		{
			Error_Handler();
		}

		HAL_TIM_MspPostInit(&htim2);

		if(PWM_Out_EN&(1<<0))
		{
			if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
			{
				Error_Handler();
			}
		}
	}
	else //pin is used as pwm input pin
	{
		TIM_SlaveConfigTypeDef sSlaveConfig = {0};
		TIM_IC_InitTypeDef sConfigIC = {0};
		TIM_MasterConfigTypeDef sMasterConfig = {0};

		htim2.Instance = TIM2;
		htim2.Init.Prescaler = PWM_Prescalers[0];
		htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim2.Init.Period = 65535;
		htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
		{
			Error_Handler();
		}

		sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
		sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
		sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
		sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
		sSlaveConfig.TriggerFilter = 0;
		if (HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK)
		{
			Error_Handler();
		}

		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
		sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
		sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
		sConfigIC.ICFilter = 0;
		if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
		{
			Error_Handler();
		}

		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
		sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
		if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
		{
			Error_Handler();
		}

		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

static void MX_TIM3_Init()
{
	TIM_OC_InitTypeDef sConfigOC = {0};

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = PWM_Prescalers[1];
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 255;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = PWM_Pulses[1];
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_TIM_MspPostInit(&htim3);

	if(PWM_Out_EN&(1<<1))
	{
		if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

static void MX_TIM4_Init()
{
	if (!((PWM_In_EN>>0)&1)) //default if pin is not used as pwm input
	{
		TIM_OC_InitTypeDef sConfigOC = {0};

		htim4.Instance = TIM4;
		htim4.Init.Prescaler = PWM_Prescalers[3];
		htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim4.Init.Period = 255;
		htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
		{
			Error_Handler();
		}

		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = PWM_Pulses[3];
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
		{
			Error_Handler();
		}

		HAL_TIM_MspPostInit(&htim4);

		if(PWM_Out_EN&(1<<3))
		{
			if (HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1) != HAL_OK)
			{
				Error_Handler();
			}
		}
	}
	else //pin is used as a pwm input
	{
		TIM_SlaveConfigTypeDef sSlaveConfig = {0};
		TIM_IC_InitTypeDef sConfigIC = {0};
		TIM_MasterConfigTypeDef sMasterConfig = {0};

		htim4.Instance = TIM4;
		htim4.Init.Prescaler = 999;
		htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim4.Init.Period = 65535;
		htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
		{
			Error_Handler();
		}

		sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
		sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
		sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
		sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
		sSlaveConfig.TriggerFilter = 0;
		if (HAL_TIM_SlaveConfigSynchro(&htim4, &sSlaveConfig) != HAL_OK)
		{
			Error_Handler();
		}

		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
		sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
		sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
		sConfigIC.ICFilter = 0;
		if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
		{
			Error_Handler();
		}

		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
		sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
		if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
		{
			Error_Handler();
		}

		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

static void MX_TIM8_Init()
{
	if (!((PWM_In_EN>>4)&1)) //default if pin is not used as a pwm input
	{
		TIM_OC_InitTypeDef sConfigOC = {0};

		htim8.Instance = TIM8;
		htim8.Init.Prescaler = PWM_Prescalers[4];
		htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim8.Init.Period = 255;
		htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim8.Init.RepetitionCounter = 0;
		htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
		{
			Error_Handler();
		}

		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = PWM_Pulses[4];
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
		{
			Error_Handler();
		}

		HAL_TIM_MspPostInit(&htim8);

		if(PWM_Out_EN&(1<<4))
		{
			if (HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1) != HAL_OK)
			{
				Error_Handler();
			}
		}
	}
	else //pin is used as a pwm input
	{
		TIM_SlaveConfigTypeDef sSlaveConfig = {0};
		TIM_IC_InitTypeDef sConfigIC = {0};
		TIM_MasterConfigTypeDef sMasterConfig = {0};
		TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

		htim8.Instance = TIM8;
		htim8.Init.Prescaler = PWM_Prescalers[4];
		htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim8.Init.Period = 65535;
		htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim8.Init.RepetitionCounter = 0;
		htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_IC_Init(&htim8) != HAL_OK)
		{
			Error_Handler();
		}

		sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
		sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
		sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
		sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
		sSlaveConfig.TriggerFilter = 0;
		if (HAL_TIM_SlaveConfigSynchro(&htim8, &sSlaveConfig) != HAL_OK)
		{
			Error_Handler();
		}

		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
		sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
		sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
		sConfigIC.ICFilter = 0;
		if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
		{
			Error_Handler();
		}

		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
		sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
		if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
		{
			Error_Handler();
		}

		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
		{
			Error_Handler();
		}

		sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
		sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
		if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

static void MX_LPTIM1_Init(void)
{
	hlptim1.Instance = LPTIM1;
	hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
	hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV128;
	hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
	hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
	hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
	hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
	hlptim1.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
	hlptim1.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
	if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
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
