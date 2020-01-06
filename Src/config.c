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
	//TODO: check that these work properly
	warn_undervoltage_U5=18000;
	warn_overvoltage_U5=28000;
	warn_undertemperature_U5=0;
	warn_overtemperature_U5=1023;
	warn_undercurrent_U5I0=0;
	warn_overcurrent_U5I0=1000;
	warn_undercurrent_U5I1=0;
	warn_overcurrent_U5I1=1000;
	warn_undervoltage_U6=18000;
	warn_overvoltage_U6=28000;
	warn_undertemperature_U6=0;
	warn_overtemperature_U6=1023;
	warn_undercurrent_U6I0=0;
	warn_overcurrent_U6I0=1000;
	warn_undercurrent_U6I1=0;
	warn_overcurrent_U6I1=1000;
	warn_undervoltage_U7=18000;
	warn_overvoltage_U7=28000;
	warn_undertemperature_U7=0;
	warn_overtemperature_U7=1023;
	warn_undercurrent_U7I0=0;
	warn_overcurrent_U7I0=1000;
	warn_undercurrent_U7I1=0;
	warn_overcurrent_U7I1=1000;



#if TEST_PWM_NOT_INPUT //in this case we are testing pwm outputs

	Digital_In_EN = 0b00000000;

#else //in this case we test digital inputs

	Digital_In_EN = 0b00011101; //bit for PB4 is 0 to ensure it isn't used as PB4 seemed to have hardware problems

#endif

	Default_Switch_State = 0b00000000;

	Can_IDs[0] = 0x0F; Can_IDs[1] = 0x10; Can_IDs[2] = 0x11; Can_IDs[3] = 0x12; Can_IDs[4] = 0x13; Can_IDs[5] = 0x14; Can_IDs[6] = 0x15; Can_IDs[7] = 0x16;
	Can_DLCs[0] = 8; Can_DLCs[1] = 8; Can_DLCs[2] = 7; Can_DLCs[3] = 3; Can_DLCs[4] = 2; Can_DLCs[5] = 3; Can_DLCs[6] = 3; Can_DLCs[7] = 8;

	uint8_t temp_Can_Config_Bytes[8][8]={	{ 1, 1, 1, 1, 1, 1, 1, 1 },
											{ 2, 0, 2, 0, 2, 0, 2, 0 },
											{ 1, 1, 1, 1, 1, 0, 0, 0 },
											{ 1, 1, 1, 0, 0, 0, 0, 0 },
											{ 2, 0, 0, 0, 0, 0, 0, 0 },
											{ 0, 0, 0, 0, 0, 0, 0, 0 },
											{ 0, 0, 0, 0, 0, 0, 0, 0 },
											{ 1, 0, 0, 0, 0, 0, 0, 0 }	};
	uint8_t temp_Can_Config_Datas[8][8]={	{ 1, 1, 1, 1, 1, 1, 1, 1 },
											{ MESS_U5I0, 0, MESS_U5I1, 0, MESS_U6I0, 0, MESS_U6I1, 0 },
											{ MESS_U7I0, MESS_U7I1, MESS_U5T, MESS_U6T, MESS_U7T, 0, 0, 0 },
											{ MESS_U5V, MESS_U6V, MESS_U7V, 0, 0, 0, 0, 0 },
											{ MESS_U5V, 0, 0, 0, 0, 0, 0, 0 },
											{ 0, 0, 0, 0, 0, 0, 0, 0 },
											{ 0, 0, 0, 0, 0, 0, 0, 0 },
											{ MESS_DI, 0, 0, 0, 0, 0, 0, 0 }	};
	for(uint32_t i=0; i<8; i++)
	{
		for(uint32_t j=0; j<8; j++)
		{
			Can_Config_Bytes[i][j]=temp_Can_Config_Bytes[i][j];
			Can_Config_Datas[i][j]=temp_Can_Config_Datas[i][j];
		}
	}
	Can_Sync_Enable = 0b01000000;
	Can_Timed_Enable = 0b10000000; //TODO: make these work without errors
	Can_Interval=1000;
	Can_Sync_Delay=0; //500 corresponds to 5ms delay
}

void Config_1(void)
{
	//TODO: setup code for each node that will be in the car
}


void Config_Write_Flash(void)
{
	uint32_t data[512] = {0};

	//bytes: [underlimit high], [underlimit low], [overlimit high], [overlimit low]
	data[U5_VOLTAGE_WARNING_LIMIT_POS]=((warn_undervoltage_U5<<0)&0xFFFF)+((warn_overvoltage_U5<<16)&0xFFFF0000);
	data[U5_TEMPERATURE_WARNING_LIMIT_POS]=((warn_undertemperature_U5<<0)&0xFFFF)+((warn_overtemperature_U5<<16)&0xFFFF0000);
	data[U5_CURRENT_WARNING_LIMIT_I0_POS]=((warn_undercurrent_U5I0<<0)&0xFFFF)+((warn_overcurrent_U5I0<<16)&0xFFFF0000);
	data[U5_CURRENT_WARNING_LIMIT_I1_POS]=((warn_undercurrent_U5I1<<0)&0xFFFF)+((warn_overcurrent_U5I1<<16)&0xFFFF0000);
	data[U6_VOLTAGE_WARNING_LIMIT_POS]=((warn_undervoltage_U6<<0)&0xFFFF)+((warn_overvoltage_U6<<16)&0xFFFF0000);
	data[U6_TEMPERATURE_WARNING_LIMIT_POS]=((warn_undertemperature_U6<<0)&0xFFFF)+((warn_overtemperature_U6<<16)&0xFFFF0000);
	data[U6_CURRENT_WARNING_LIMIT_I0_POS]=((warn_undercurrent_U6I0<<0)&0xFFFF)+((warn_overcurrent_U6I0<<16)&0xFFFF0000);
	data[U6_CURRENT_WARNING_LIMIT_I1_POS]=((warn_undercurrent_U6I1<<0)&0xFFFF)+((warn_overcurrent_U6I1<<16)&0xFFFF0000);
	data[U7_VOLTAGE_WARNING_LIMIT_POS]=((warn_undervoltage_U7<<0)&0xFFFF)+((warn_overvoltage_U7<<16)&0xFFFF0000);
	data[U7_TEMPERATURE_WARNING_LIMIT_POS]=((warn_undertemperature_U7<<0)&0xFFFF)+((warn_overtemperature_U7<<16)&0xFFFF0000);
	data[U7_CURRENT_WARNING_LIMIT_I0_POS]=((warn_undercurrent_U7I0<<0)&0xFFFF)+((warn_overcurrent_U7I0<<16)&0xFFFF0000);
	data[U7_CURRENT_WARNING_LIMIT_I1_POS]=((warn_undercurrent_U7I1<<0)&0xFFFF)+((warn_overcurrent_U7I1<<16)&0xFFFF0000);

	//bytes: [enable falling edge to can], [enable rising edge to can], [digital in interrupt enable], [digital in enable]
	data[DIGITAL_IN_0_POS]=(Digital_In_EN&0xFF)+((Digital_In_Interrupt_EN&0xFF)<<8)+((Digital_In_Interrupt_Can_Rising&0xFF)<<16)+((Digital_In_Interrupt_Can_Falling&0xFF)<<24); //TODO: set this to be the things it should be for digital_in
	//bytes: [unused], [unused], [enable rising edge switch power], [enable falling edge switch power]
	data[DIGITAL_IN_1_POS]=(0)+(0)+((Digital_In_Interrupt_Power_Rising&0xFF)<<16)+((Digital_In_Interrupt_Power_Falling&0xFF)<<24);
	//TODO: read other stuff from digital in back to flash, make it work in general
	//bytes: [x], [x], [x], [x x U7/1 U7/0 U6/1 U6/0 U5/1 U5/0]
	data[DEFAULT_SWITCH_STATE_POS]=Default_Switch_State&0xFF;

	uint32_t CanPos[8] = {CAN_ID_0_POS, CAN_ID_1_POS, CAN_ID_2_POS, CAN_ID_3_POS, CAN_ID_4_POS, CAN_ID_5_POS, CAN_ID_6_POS, CAN_ID_7_POS};
	for(uint32_t i=0; i<8; i++)
	{
		//bytes: [unused], [can dlc], [can id high], [can id low] x8 //any can id outside the valid range of 0 to 2047 should be treated as disabled
		data[CanPos[i]]=(0)+((Can_DLCs[i]&0xFF)<<16)+(Can_IDs[i]&0xFFFF);
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

	for(uint32_t i=0; i<8; i++)
	{
		//byte: [bytes of specific data]
		data[CAN_BYTES_1ST_POS+i*2]=Can_Config_Bytes[i][0]+(Can_Config_Bytes[i][1]<<8)+(Can_Config_Bytes[i][2]<<16)+(Can_Config_Bytes[i][3]<<24);
		data[CAN_BYTES_1ST_POS+i*2+1]=Can_Config_Bytes[i][4]+(Can_Config_Bytes[i][5]<<8)+(Can_Config_Bytes[i][6]<<16)+(Can_Config_Bytes[i][7]<<24);
		//byte: [id of data to send]
		data[CAN_DATAS_1ST_POS+i*2]=Can_Config_Datas[i][0]+(Can_Config_Datas[i][1]<<8)+(Can_Config_Datas[i][2]<<16)+(Can_Config_Datas[i][3]<<24);
		data[CAN_DATAS_1ST_POS+i*2+1]=Can_Config_Datas[i][4]+(Can_Config_Datas[i][5]<<8)+(Can_Config_Datas[i][6]<<16)+(Can_Config_Datas[i][7]<<24);
	}
	//note that only two messages may be sent on sync, or 3 if absolutely no other messages are being sent (including errors)
	//bytes: [can interval (.1 ms) high], [can interval (.1 ms) low], [send can message on timer], [send can messages on sync]
	data[CAN_SEND_EN_POS]=(Can_Sync_Enable&0xFF)+((Can_Timed_Enable&0xFF)<<8)+((Can_Interval&0xFFFF)<<16);
	//bytes: [can sync delay (10 us) high], [can sync delay (10 us) low], [x], [x]
	data[CAN_SYNC_DELAY_POS]=((Can_Sync_Delay&0xFFFF)<<16);

	Flash_Write(FLASH_PAGE_63, 63, data, 512);
}

void Config_Read_Flash(void)
{
	warn_undervoltage_U5=((U5_VOLTAGE_WARNING_LIMIT>>0)&0xFFFF);
	warn_overvoltage_U5=((U5_VOLTAGE_WARNING_LIMIT>>16)&0xFFFF);
	warn_undertemperature_U5=((U5_TEMPERATURE_WARNING_LIMIT>>0)&0xFFFF);
	warn_overtemperature_U5=((U5_TEMPERATURE_WARNING_LIMIT>>16)&0xFFFF);
	warn_undercurrent_U5I0=((U5_CURRENT_WARNING_LIMIT_I0>>0)&0xFFFF);
	warn_overcurrent_U5I0=((U5_CURRENT_WARNING_LIMIT_I0>>16)&0xFFFF);
	warn_undercurrent_U5I1=((U5_CURRENT_WARNING_LIMIT_I1>>0)&0xFFFF);
	warn_overcurrent_U5I1=((U5_CURRENT_WARNING_LIMIT_I1>>16)&0xFFFF);
	warn_undervoltage_U6=((U6_VOLTAGE_WARNING_LIMIT>>0)&0xFFFF);
	warn_overvoltage_U6=((U6_VOLTAGE_WARNING_LIMIT>>16)&0xFFFF);
	warn_undertemperature_U6=((U6_TEMPERATURE_WARNING_LIMIT>>0)&0xFFFF);
	warn_overtemperature_U6=((U6_TEMPERATURE_WARNING_LIMIT>>16)&0xFFFF);
	warn_undercurrent_U6I0=((U6_CURRENT_WARNING_LIMIT_I0>>0)&0xFFFF);
	warn_overcurrent_U6I0=((U6_CURRENT_WARNING_LIMIT_I0>>16)&0xFFFF);
	warn_undercurrent_U6I1=((U6_CURRENT_WARNING_LIMIT_I1>>0)&0xFFFF);
	warn_overcurrent_U6I1=((U6_CURRENT_WARNING_LIMIT_I1>>16)&0xFFFF);
	warn_undervoltage_U7=((U7_VOLTAGE_WARNING_LIMIT>>0)&0xFFFF);
	warn_overvoltage_U7=((U7_VOLTAGE_WARNING_LIMIT>>16)&0xFFFF);
	warn_undertemperature_U7=((U7_TEMPERATURE_WARNING_LIMIT>>0)&0xFFFF);
	warn_overtemperature_U7=((U7_TEMPERATURE_WARNING_LIMIT>>16)&0xFFFF);
	warn_undercurrent_U7I0=((U7_CURRENT_WARNING_LIMIT_I0>>0)&0xFFFF);
	warn_overcurrent_U7I0=((U7_CURRENT_WARNING_LIMIT_I0>>16)&0xFFFF);
	warn_undercurrent_U7I1=((U7_CURRENT_WARNING_LIMIT_I1>>0)&0xFFFF);
	warn_overcurrent_U7I1=((U7_CURRENT_WARNING_LIMIT_I1>>16)&0xFFFF); //TODO: something here stops can sending, not sure why

	Digital_In_EN = ((DIGITAL_IN_0>>0)&0b00011101); //bit for PB4 is 0 to ensure it isn't used as PB4 seemed to have hardware problems
	Default_Switch_State=((DEFAULT_SWITCH_STATE>>0)&0b00111111);

	uint32_t CanId[8] = {CAN_ID_0, CAN_ID_1, CAN_ID_2, CAN_ID_3, CAN_ID_4, CAN_ID_5, CAN_ID_6, CAN_ID_7};
	for(uint32_t i=0; i<8; i++)
	{
		Can_IDs[i]=((CanId[i]>>0)&0xFFFF);
		Can_DLCs[i]=((CanId[i]>>16)&0xFF);
	}

	for(uint32_t i=0; i<8; i++)
	{
		uint32_t temp_can_bytes_0=*(&CAN_BYTES_1ST+i*2);
		uint32_t temp_can_bytes_1=*(&CAN_BYTES_1ST+i*2+1);
		uint32_t temp_can_datas_0=*(&CAN_DATAS_1ST+i*2);
		uint32_t temp_can_datas_1=*(&CAN_DATAS_1ST+i*2+1);
		for(uint32_t j=0; j<4; j++)
		{
			Can_Config_Bytes[i][j]=(temp_can_bytes_0>>(8*j)) & 0xFF;
			Can_Config_Bytes[i][j+4]=(temp_can_bytes_1>>(8*j)) & 0xFF;
			Can_Config_Datas[i][j]=(temp_can_datas_0>>(8*j)) & 0xFF;
			Can_Config_Datas[i][j+4]=(temp_can_datas_1>>(8*j)) & 0xFF;
		}
	}
	Can_Sync_Enable=(CAN_SEND_EN>>0)&0b11111111;
	Can_Timed_Enable=(CAN_SEND_EN>>8)&0b11111111;
	Can_Interval=(CAN_SEND_EN>>16)&0xFFFF;
	Can_Sync_Delay=(CAN_SYNC_DELAY>>16)*0xFFFF;
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
