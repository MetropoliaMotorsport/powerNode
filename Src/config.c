#include "main.h"
#include "config.h"


void Config_Setup(void)
{
	if(Flash_Read(FLASH_PAGE_63)==0xFFFFFFFF) //initialize the flash to avoid errors
	{

#if ID == 1

		Config_1();

#else //catch everything that is not a proper ID, give it settings that the debug board would get

		Config_0();

#endif

		Config_Write_Flash();
	}
	else //if flash is not blank read in values from flash
	{
		Config_Read_Flash();
	}
}


void Config_0(void)
{
#if TEST_PWM_NOT_INPUT //in this case we are testing pwm outputs

	Digital_In_EN = 0xb00000000;

#else //in this case we test digital inputs

	Digital_In_EN = 0b00011101; //bit for PB4 is 0 to ensure it isn't used as PB4 seemed to have hardware problems

#endif

	Can_IDs[0] = 0x0F; Can_IDs[1] = 0x10; Can_IDs[2] = 0x11; Can_IDs[3] = 0x12; Can_IDs[4] = 0x13; Can_IDs[5] = 0x14; Can_IDs[6] = 0x15; Can_IDs[7] = 0x16;
	Can_DLCs[0] = 8; Can_DLCs[1] = 8; Can_DLCs[2] = 7; Can_DLCs[3] = 3; Can_DLCs[4] = 2; Can_DLCs[5] = 0; Can_DLCs[6] = 3; Can_DLCs[7] = 0;

	//TODO: write and read this from flash as well
	uint8_t temp_Can_Config_Bytes[8][8]={	{ 0, 1, 2, 3, 4, 5, 6, 7 },
											{ 2, 0, 2, 0, 2, 0, 2, 0 },
											{ 1, 1, 1, 1, 1, 0, 0, 0 },
											{ 1, 1, 1, 0, 0, 0, 0, 0 },
											{ 2, 0, 0, 0, 0, 0, 0, 0 },
											{ 0, 0, 0, 0, 0, 0, 0, 0 },
											{ 0, 0, 0, 0, 0, 0, 0, 0 },
											{ 0, 0, 0, 0, 0, 0, 0, 0 }	};
	uint8_t temp_Can_Config_Datas[8][8]={	{ 0, 1, 2, 3, 4, 5, 6, 7 },
											{ MESS_U5I0, 0, MESS_U5I1, 0, MESS_U6I0, 0, MESS_U6I1, 0 },
											{ MESS_U7I0, MESS_U7I1, MESS_U5T, MESS_U6T, MESS_U7T, 0, 0, 0 },
											{ MESS_U5V, MESS_U6V, MESS_U7V, 0, 0, 0, 0, 0 },
											{ MESS_U5V, 0, 0, 0, 0, 0, 0, 0 },
											{ 0, 0, 0, 0, 0, 0, 0, 0 },
											{ 0, 0, 0, 0, 0, 0, 0, 0 },
											{ 0, 0, 0, 0, 0, 0, 0, 0 }	};
	for(uint32_t i=0; i<8; i++)
	{
		for(uint32_t j=0; j<8; j++)
		{
			Can_Config_Bytes[i][j]=temp_Can_Config_Bytes[i][j];
			Can_Config_Datas[i][j]=temp_Can_Config_Datas[i][j];
		}
	}

}

void Config_1(void)
{
	//TODO: setup code for each node that will be in the car
}


void Config_Write_Flash(void)
{
	uint32_t data[512] = {0};

	//bytes: [enable falling edge to can], [enable rising edge to can], [digital in interrupt enable], [digital in enable]
	data[DIGITAL_IN_0_POS]=Digital_In_EN+(Digital_In_Interrupt_EN<<8)+(Digital_In_Interrupt_Can_Rising<<16)+(Digital_In_Interrupt_Can_Falling<<24); //TODO: set this to be the things it should be for digital_in
	//bytes: [unused], [unused], [enable rising edge switch power], [enable falling edge switch power]
	data[DIGITAL_IN_1_POS]=(0)+(0)+(Digital_In_Interrupt_Power_Rising<<16)+(Digital_In_Interrupt_Power_Falling<<24);

	uint32_t CanPos[8] = {CAN_ID_0_POS, CAN_ID_1_POS, CAN_ID_2_POS, CAN_ID_3_POS, CAN_ID_4_POS, CAN_ID_5_POS, CAN_ID_6_POS, CAN_ID_7_POS};
	for(uint32_t i=0; i<8; i++)
	{
		//bytes: [unused], [can dlc], [can id high], [can id low] x8 //any can id outside the valid range of 0 to 2047 should be treated as disabled
		data[CanPos[i]]=(0)+(Can_DLCs[i]<<16)+(Can_IDs[i]&0xFFFF);
	}

	//bytes: [unused], [can dlc], [can id high], [can id low] x8 //any can id outside the valid range of 0 to 2047 should be treated as disabled
	/*data[CAN_ID_0_POS]=(0)+(Can_DLCs[0]<<16)+(Can_IDs[0]&0xFFFF);
	data[CAN_ID_1_POS]=(0)+(Can_DLCs[1]<<16)+(Can_IDs[1]&0xFFFF);
	data[CAN_ID_2_POS]=(0)+(Can_DLCs[2]<<16)+(Can_IDs[2]&0xFFFF);
	data[CAN_ID_3_POS]=(0)+(Can_DLCs[3]<<16)+(Can_IDs[3]&0xFFFF);
	data[CAN_ID_4_POS]=(0)+(Can_DLCs[4]<<16)+(Can_IDs[4]&0xFFFF);
	data[CAN_ID_5_POS]=(0)+(Can_DLCs[5]<<16)+(Can_IDs[5]&0xFFFF);
	data[CAN_ID_6_POS]=(0)+(Can_DLCs[6]<<16)+(Can_IDs[6]&0xFFFF);
	data[CAN_ID_7_POS]=(0)+(Can_DLCs[7]<<16)+(Can_IDs[7]&0xFFFF);*/

	Flash_Write(FLASH_PAGE_63, 63, data, 10);
}

void Config_Read_Flash(void)
{
	Digital_In_EN = (0b00011101&(DIGITAL_IN_0>>0)); //bit for PB4 is 0 to ensure it isn't used as PB4 seemed to have hardware problems

	uint32_t CanId[8] = {CAN_ID_0, CAN_ID_1, CAN_ID_2, CAN_ID_3, CAN_ID_4, CAN_ID_5, CAN_ID_6, CAN_ID_7};
	for(uint32_t i=0; i<8; i++)
	{
		Can_IDs[i]=((CanId[i]>>0)&0xFFFF);
		Can_DLCs[i]=((CanId[i]>>16)&0xFF);
	}

	/*Can_IDs[0] = ((CAN_ID_0>>0)&0xFFFF);
	Can_DLCs[0] = ((CAN_ID_0>>16)&0xFF);
	Can_IDs[1] = ((CAN_ID_1>>0)&0xFFFF);
	Can_DLCs[1] = ((CAN_ID_1>>16)&0xFF);
	Can_IDs[2] = ((CAN_ID_2>>0)&0xFFFF);
	Can_DLCs[2] = ((CAN_ID_2>>16)&0xFF);
	Can_IDs[3] = ((CAN_ID_3>>0)&0xFFFF);
	Can_DLCs[3] = ((CAN_ID_3>>16)&0xFF);
	Can_IDs[4] = ((CAN_ID_4>>0)&0xFFFF);
	Can_DLCs[4] = ((CAN_ID_4>>16)&0xFF);
	Can_IDs[5] = ((CAN_ID_5>>0)&0xFFFF);
	Can_DLCs[5] = ((CAN_ID_5>>16)&0xFF);
	Can_IDs[6] = ((CAN_ID_6>>0)&0xFFFF);
	Can_DLCs[6] = ((CAN_ID_6>>16)&0xFF);
	Can_IDs[7] = ((CAN_ID_7>>0)&0xFFFF);
	Can_DLCs[7] = ((CAN_ID_7>>16)&0xFF);*/
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
