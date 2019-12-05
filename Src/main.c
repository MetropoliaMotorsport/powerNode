//includes
#include "main.h"

//function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_FDCAN1_Init(void);

void Flash_Write(uint32_t, uint32_t, uint32_t[512], int);
uint32_t Flash_Read(uint32_t);

//type definitions
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
FDCAN_HandleTypeDef hfdcan;
FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_RxHeaderTypeDef RxHeader;
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
uint32_t Digital_In_Interrupt_EN;
uint32_t Digital_In_Interrupt_Can_Id;
uint32_t Digital_In_Interrupt_Can_Rising;
uint32_t Digital_In_Interrupt_Can_Falling;
uint32_t Digital_In_Interrupt_Power_Rising;
uint32_t Digital_In_Interrupt_Power_Falling;
uint32_t Digital_In_Interrupt_PWM_Rising;
uint32_t Digital_In_Interrupt_PWM_Falling;
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

#endif

		//bytes: [enable falling edge to can], [enable rising edge to can], [digital in interrupt enable], [digital in enable]
		data[DIGITAL_IN_0_POS]=Digital_In_EN+(Digital_In_Interrupt_EN<<8)+(Digital_In_Interrupt_Can_Rising<<16)+(Digital_In_Interrupt_Can_Falling<<24); //TODO: set this to be the things it should be for digital_in
		//byte: [can ID low], [can ID high], [enable rising edge switch power], [enable falling edge switch power]
		data[DIGITAL_IN_1_POS]=(0)+(0)+(Digital_In_Interrupt_Power_Rising<<16)+(Digital_In_Interrupt_Power_Falling<<24);

		Flash_Write(FLASH_PAGE_63, 63, data, 1);
	}
	else //if flash is not blank read in values from flash
	{
		Digital_In_EN = ((0b00011101<<0)&DIGITAL_IN_0)>>0; //bit for PB4 is 0 to ensure it isn't used as PB4 seemed to have hardware problems
	}


	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	MX_FDCAN1_Init();

	while (1)
	{
		//example commands stored here
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)); //reading pins may be wanted with interrupts at some time, and it may be wanted to debounce some digital inputs

//TODO: test high side drivers with this, make sure they can handle at least two amps each and let pedro know
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
		HAL_GPIO_WritePin(U5IN0.PORT, U5IN0.PIN, 1);
		HAL_GPIO_TogglePin(U5IN1.PORT, U5IN1.PIN);
		HAL_GPIO_TogglePin(U6IN0.PORT, U6IN0.PIN);
		HAL_Delay(5);
		HAL_GPIO_TogglePin(U6IN1.PORT, U6IN1.PIN);
		HAL_Delay(5);
		HAL_GPIO_TogglePin(U7IN0.PORT, U7IN0.PIN);
		HAL_Delay(5);
		HAL_GPIO_TogglePin(U7IN1.PORT, U7IN1.PIN);
		HAL_Delay(5);
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
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_8) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12|RCC_PERIPHCLK_FDCAN;
	PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_HSE;
	PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
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
  hfdcan.Init.StdFiltersNbr = 0;
  hfdcan.Init.ExtFiltersNbr = 0;
  hfdcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

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

	//led pin, sel pins, and in pins should always be outputs
	HAL_GPIO_WritePin(LED.PORT, LED.PIN, 0);
	HAL_GPIO_WritePin(SEL0.PORT, SEL0.PIN, 0);
	HAL_GPIO_WritePin(SEL1.PORT, SEL1.PIN, 0);
	HAL_GPIO_WritePin(U5IN0.PORT, U5IN0.PIN, 0);
	HAL_GPIO_WritePin(U5IN1.PORT, U5IN1.PIN, 0);
	HAL_GPIO_WritePin(U6IN0.PORT, U6IN0.PIN, 0);
	HAL_GPIO_WritePin(U6IN1.PORT, U6IN1.PIN, 0);
	HAL_GPIO_WritePin(U7IN0.PORT, U7IN0.PIN, 0);
	HAL_GPIO_WritePin(U7IN1.PORT, U7IN1.PIN, 0);
//TODO: check that IN pins should start low, if they should start high then start them high here

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


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
		HAL_Delay(33);
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
