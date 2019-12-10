//includes
#include "main.h"

//function prototypes
uint32_t CanSend(uint32_t);

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_FDCAN_Init(void);

void Flash_Write(uint32_t, uint32_t, uint32_t[512], int);
uint32_t Flash_Read(uint32_t);

//type handlers
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
FDCAN_HandleTypeDef hfdcan;
//FDCAN_TxHeaderTypeDef TxHeader; //only use this locally instead
//FDCAN_RxHeaderTypeDef RxHeader; //also only use this locally
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
uint32_t Digital_In_EN; //byte: xxx[DIO15][DI6][DIO5][DIO4][DIO3]
uint32_t Digital_In_Interrupt_EN; //TODO
uint32_t Digital_In_Interrupt_Can_Rising; //TODO
uint32_t Digital_In_Interrupt_Can_Falling; //TODO
uint32_t Digital_In_Interrupt_Power_Rising; //TODO
uint32_t Digital_In_Interrupt_Power_Falling; //TODO
uint32_t Digital_In_Interrupt_PWM_Rising; //TODO
uint32_t Digital_In_Interrupt_PWM_Falling; //TODO
uint32_t Can_IDs[8];
uint32_t Can_DLCs[8];
//probably several here for which switch to switch and on or off and which pwm out to change and too what

//global variables


int main(void)
{
	HAL_Init();

	SystemClock_Config();

	if(Flash_Read(FLASH_PAGE_63)==0xFFFFFFFF) //initialize the flash to avoid errors
	{
		uint32_t data[512] = {0};


#if ID == 1

//TODO: setup code for separate nodes

#else //catch everything that is not a proper ID, give it settings that the debug board would get

	#if TEST_PWM_NOT_INPUT //in this case we are testing pwm outputs

		Digital_In_EN = 0xb00000000;

	#else //in this case we test digital inputs

		Digital_In_EN = 0b00011101; //bit for PB4 is 0 to ensure it isn't used as PB4 seemed to have hardware problems

	#endif

		Can_IDs[0] = 0x0F;
		Can_DLCs[0] = 1;
		Can_IDs[1] = 0xFF;
		Can_DLCs[1] = 0;
		Can_IDs[2] = -1;
		Can_DLCs[2] = 4;
		Can_IDs[3] = -1;
		Can_DLCs[3] = 0;
		Can_IDs[4] = -1;
		Can_DLCs[4] = 0;
		Can_IDs[5] = -1;
		Can_DLCs[5] = 0;
		Can_IDs[6] = -1;
		Can_DLCs[6] = 0;
		Can_IDs[7] = -1;
		Can_DLCs[7] = 0;
#endif
//TODO: move this code to separate file and some different functions
		//bytes: [enable falling edge to can], [enable rising edge to can], [digital in interrupt enable], [digital in enable]
		data[DIGITAL_IN_0_POS]=Digital_In_EN+(Digital_In_Interrupt_EN<<8)+(Digital_In_Interrupt_Can_Rising<<16)+(Digital_In_Interrupt_Can_Falling<<24); //TODO: set this to be the things it should be for digital_in
		//bytes: [unused], [unused], [enable rising edge switch power], [enable falling edge switch power]
		data[DIGITAL_IN_1_POS]=(0)+(0)+(Digital_In_Interrupt_Power_Rising<<16)+(Digital_In_Interrupt_Power_Falling<<24);
		//bytes: [unused], [can dlc], [can id high], [can id low] x8 //any can id outside the valid range of 0 to 2047 should be treated as disabled
		data[CAN_ID_0_POS]=(0)+(Can_DLCs[0]<<16)+(Can_IDs[0]);
		data[CAN_ID_1_POS]=(0)+(Can_DLCs[1]<<16)+(Can_IDs[1]);
		data[CAN_ID_2_POS]=(0)+(Can_DLCs[2]<<16)+(Can_IDs[2]);
		data[CAN_ID_3_POS]=(0)+(Can_DLCs[3]<<16)+(Can_IDs[3]);
		data[CAN_ID_4_POS]=(0)+(Can_DLCs[4]<<16)+(Can_IDs[4]);
		data[CAN_ID_5_POS]=(0)+(Can_DLCs[5]<<16)+(Can_IDs[5]);
		data[CAN_ID_6_POS]=(0)+(Can_DLCs[6]<<16)+(Can_IDs[6]);
		data[CAN_ID_7_POS]=(0)+(Can_DLCs[7]<<16)+(Can_IDs[7]);

		Flash_Write(FLASH_PAGE_63, 63, data, 1);
	}
	else //if flash is not blank read in values from flash
	{
		Digital_In_EN = (0b00011101&(DIGITAL_IN_0>>0)); //bit for PB4 is 0 to ensure it isn't used as PB4 seemed to have hardware problems

		Can_IDs[0] = ((CAN_ID_0>>0)&&0xFFFF);
		Can_DLCs[0] = ((CAN_ID_0>>16)&0xFF);
		Can_IDs[1] = ((CAN_ID_1>>0)&&0xFFFF);
		Can_DLCs[1] = ((CAN_ID_1>>16)&0xFF);
		Can_IDs[2] = ((CAN_ID_2>>0)&&0xFFFF);
		Can_DLCs[2] = ((CAN_ID_2>>16)&0xFF);
		Can_IDs[3] = ((CAN_ID_3>>0)&&0xFFFF);
		Can_DLCs[3] = ((CAN_ID_3>>16)&0xFF);
		Can_IDs[4] = ((CAN_ID_4>>0)&&0xFFFF);
		Can_DLCs[4] = ((CAN_ID_4>>16)&0xFF);
		Can_IDs[5] = ((CAN_ID_5>>0)&&0xFFFF);
		Can_DLCs[5] = ((CAN_ID_5>>16)&0xFF);
		Can_IDs[6] = ((CAN_ID_6>>0)&&0xFFFF);
		Can_DLCs[6] = ((CAN_ID_6>>16)&0xFF);
		Can_IDs[7] = ((CAN_ID_7>>0)&&0xFFFF);
		Can_DLCs[7] = ((CAN_ID_7>>16)&0xFF);
	}


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
			}
		HAL_Delay(133);

//TODO: test high side drivers again for realistic power of fans and pumps while in heatshrink
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
	else if(Can_DLCs[message]>4)
	{
		return -1;
		//TODO: set warning for trying to send too long message
	}

	FDCAN_TxHeaderTypeDef TxHeader;

	TxHeader.Identifier = Can_IDs[message];
	TxHeader.DataLength = Can_DLCs[message];
	uint8_t CANTxData[8] = { 0x88, 0xFF, 0x00, 0x01, 0x12, 0x11, 0x22, 0x23 }; //TODO: tx data based on values from flash somehow

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
// moved everything to one fifo.  consider using both for prioritisation
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

		HAL_GPIO_TogglePin(LED.PORT, LED.PIN);
		//TODO: double check that id is CANID_CONFIG or CANID_SYNC
		//TODO: put logic here for toggling output pins and pwm frequencies
		//TODO: logic for writing config to flash
		//TODO: put logic for sync message here
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

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_10B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.GainCompensation = 0;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the ADC multi-mode 
  */
  multimode.Mode = ADC_DUALMODE_REGSIMULT;
  multimode.DMAAccessMode = ADC_DMAACCESSMODE_12_10_BITS;
  multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_1CYCLE;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_24CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Common config 
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.GainCompensation = 0;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.NbrOfConversion = 3;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
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
  /** Configure Regular Channel 
  */
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

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

	//only accept config/request can messages right now
	//TODO: add a filter to accept whatever sync message is
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

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}

static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	//led pin, sel pins, and in pins should always be outputs and should always be low before main is running
	//TODO: somewhere else check and set the default for IN pins
	//TODO: in analog reading section use SEL0 and SEL1 for multisense multiplexing
	HAL_GPIO_WritePin(LED.PORT, LED.PIN, 0);
	HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 0);
	HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 0);
	HAL_GPIO_WritePin(U5IN0.PORT, U5IN0.PIN, 0);
	HAL_GPIO_WritePin(U5IN1.PORT, U5IN1.PIN, 0);
	HAL_GPIO_WritePin(U6IN0.PORT, U6IN0.PIN, 0);
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


void Flash_Write(uint32_t Flash_Address, uint32_t Page, uint32_t Flash_Data[512], int Data_Words)
{
	FLASH_EraseInitTypeDef pEraseInit;
	uint32_t pError = 0;

	pEraseInit.Page = Page;
	pEraseInit.NbPages = 1;
	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;

	__disable_irq();
	if(HAL_FLASH_Unlock() != HAL_OK)
	{
		__enable_irq();
		Error_Handler();
	}

	while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) != 0) { }

	if(HAL_FLASHEx_Erase(&pEraseInit, &pError) != HAL_OK)
	{
		__enable_irq();
		while(1)
		{
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
			HAL_Delay(33);
		}
	}

	uint64_t data;
	for(int i=0; i<((Data_Words+1)/2); i++) //here we want integer division that rounds up instead of down
	{
		data=(uint64_t)Flash_Data[i*2];
		if((i*2)+1<Data_Words)
		{
			data+=((uint64_t)Flash_Data[i*2+1]<<32);
		}

			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Flash_Address+i*0x08, data) != HAL_OK)
			{
				__enable_irq();
				Error_Handler();
			}
		while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) != 0) { }
	}

	if(HAL_FLASH_Lock() != HAL_OK)
	{
		__enable_irq();
		Error_Handler();
	}
	__enable_irq();
}

uint32_t Flash_Read(uint32_t Flash_Address)
{
	return *(uint32_t*)Flash_Address;
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
