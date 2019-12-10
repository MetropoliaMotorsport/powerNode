#include "main.h"
#include "commands.h"


const pinPort *switches[] = {&U5IN0, &U5IN1, &U6IN0, &U6IN1, &U7IN0, &U7IN1};

void Switch_Power(uint8_t enableSwitching, uint8_t newState)
{
	for(uint32_t i=0; i<6; i++)
	{
		if ((1<<i) & enableSwitching)
		{
			HAL_GPIO_WritePin(switches[i]->PORT, switches[i]->PIN, ((1<<i)*newState)>>i);
		}
	}
}
