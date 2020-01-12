#include "main.h"
#include "stm32g4xx_it.h"

//external type handlers
extern FDCAN_HandleTypeDef hfdcan;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;

//external variables
extern uint8_t CanTimerFlag;

void NMI_Handler(void)
{

}

void HardFault_Handler(void)
{
  while (1)
  {

  }
}

void MemManage_Handler(void)
{
  while (1)
  {

  }
}

void BusFault_Handler(void)
{
  while (1)
  {

  }
}

void UsageFault_Handler(void)
{
  while (1)
  {

  }
}

void SVC_Handler(void)
{

}

void DebugMon_Handler(void)
{

}

void PendSV_Handler(void)
{

}

void SysTick_Handler(void)
{
  HAL_IncTick();
}


void DMA1_Channel1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_adc1);
}

void DMA1_Channel2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_adc2);
}


void FDCAN1_IT0_IRQHandler(void)
{
	HAL_FDCAN_IRQHandler(&hfdcan);
}

void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim6);
}

void TIM7_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim7);
}

void TIM1_BRK_TIM15_IRQHandler(void)
{
	//we don't care or want to do anything with TIM1 BRK
	HAL_TIM_IRQHandler(&htim15);
}

void TIM1_UP_TIM16_IRQHandler(void)
{
	//this will work becuase IRQHandler checks to see what event happened
	//however, IRQHandler is not very efficient function for the things we are using here, code could be faster checking registers and calling callback function directly
	//but no time for writing that now
	HAL_TIM_IRQHandler(&htim1);
	HAL_TIM_IRQHandler(&htim16);
}

//it seems like the interrupts for PWM input are not called
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim2);
}

/*void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim4);
}

void TIM8_CC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim8);
}*/
