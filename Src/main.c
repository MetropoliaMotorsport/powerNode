#include "main.h"

//function prototypes
uint32_t CanSend(uint32_t);

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_FDCAN_Init(void);

//type handlers
FDCAN_HandleTypeDef hfdcan;
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;

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
uint16_t Can_IDs[8];
uint8_t Can_DLCs[8];
uint8_t Can_Config_Bytes[8][8];
uint8_t Can_Config_Datas[8][8];
//probably several here for which switch to switch and on or off and which pwm out to change and too what

//global variables
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


	while(1)
	{
		//example commands stored here
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)); //reading pins may be wanted with interrupts at some time, and it may be wanted to debounce some digital inputs

		for(int i=0; i<8; i++)
			{
			CanSend(i);
			HAL_Delay(1);
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

			//TODO: calculate averages here

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

			//TODO: calculate averages here

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
			//TODO: warnings on over/undercurrent, overcurrent shutoff

			break;
		default:
			Error_Handler();
			break;
		}

	}
}


uint32_t CanSend(uint32_t message)
{
	//TODO: maybe have warning states based on these if statements
	if(HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan) < 1)
	{
		return -1;
	}

	if(Can_IDs[message]>2047)
	{
		return -1;
		//TODO: for sure we want warning for trying to send disabled message
	}
	else if(Can_DLCs[message]==0)
	{
		return -1;
		//TODO: set warning for trying to send message with 0 DLC
	}
	else if(Can_DLCs[message]>8)
	{
		return -1;
		//TODO: set warning for trying to send too long message
	}

	FDCAN_TxHeaderTypeDef TxHeader;

	TxHeader.Identifier = Can_IDs[message];
	TxHeader.DataLength = (Can_DLCs[message]<<16); //<<16 makes storing the number of bytes not require a switch statement for classic can
 //TODO: tx data based on values from flash somehow
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
	//TODO: logic for different can tx data

	TxHeader.IdType = FDCAN_STANDARD_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader.MessageMarker = 0;

	if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan, &TxHeader, CANTxData) != HAL_OK)
	{
		return -1;
		//Error_Handler();
	}

	return 0;
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	FDCAN_RxHeaderTypeDef RxHeader;
	uint8_t CANRxData[8];

	if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
	{
		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, CANRxData) != HAL_OK)
		{
			Error_Handler();
			//TODO: move to error can message
		}

		if (RxHeader.Identifier == CANID_SYNC)
		{
			//TODO: put logic for sync message here
		}
		else if (RxHeader.Identifier == CANID_CONFIG)
		{
			//TODO: put logic here for toggling output pins and pwm frequencies
			//TODO: logic for writing config to flash
			if(CANRxData[0] == ID)
			{
				switch(CANRxData[1])
				{
				case SWITCH_POWER:
					Switch_Power(CANRxData[2], CANRxData[3]);
					break;
				default:
					//TODO: warning to canbus for undefined configuration command
					break;

				}
			}
		}
		else
		{
			Error_Handler();
			//TODO: move to error can message
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
	//TODO: somewhere else check and set the default for IN pins
	//TODO: in analog reading section use SEL0 and SEL1 for multisense multiplexing
	HAL_GPIO_WritePin(LED.PORT, LED.PIN, 0);
	HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 0);
	HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 0);
	HAL_GPIO_WritePin(U5IN0.PORT, U5IN0.PIN, 1);
	HAL_GPIO_WritePin(U5IN1.PORT, U5IN1.PIN, 1);
	HAL_GPIO_WritePin(U6IN0.PORT, U6IN0.PIN, 1);
	HAL_GPIO_WritePin(U6IN1.PORT, U6IN1.PIN, 0);
	HAL_GPIO_WritePin(U7IN0.PORT, U7IN0.PIN, 0);
	HAL_GPIO_WritePin(U7IN1.PORT, U7IN1.PIN, 0);

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
	else if (1)
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
