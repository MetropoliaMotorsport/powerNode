#include "main.h"
#include "stm32g4xx_it.h"

//external type handlers
extern FDCAN_HandleTypeDef hfdcan;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;

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

/**
  * @brief This function handles DMA1 channel2 global interrupt.
  */
void DMA1_Channel2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&hdma_adc2);
}


void FDCAN1_IT0_IRQHandler(void)
{
	HAL_FDCAN_IRQHandler(&hfdcan);
}
