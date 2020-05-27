#include "main.h"
#include "checks.h"


//global variables
uint8_t U5I0_active_counter;
uint8_t U5I1_active_counter;
uint8_t U6I0_active_counter;
uint8_t U6I1_active_counter;
uint8_t U7I0_active_counter;
uint8_t U7I1_active_counter;

void Check_I0_Warn()
{
	//for currents calculation is done in the interrupt
	if ((U5I0_real>warn_overcurrent_U5I0) && (U5I0_active) && U5I0_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_OVERCURR_U5I0);
	}
	if (U5I0_real<warn_undercurrent_U5I0 && warn_undercurrent_U5I0>0 && U5I0_active && U5I0_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_UNDERCURR_U5I0);
	}
	else if (U5I0_active && U5I0_active_counter<=I_ROLLING_AVERAGE)
	{
		U5I0_active_counter++;
	}
	else if (U5I0_active_counter)
	{
		U5I0_active_counter=0;
	}

	if ((U6I0_real>warn_overcurrent_U6I0) && (U6I0_active) && U6I0_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_OVERCURR_U6I0);
	}
	if (U6I0_real<warn_undercurrent_U6I0 && warn_undercurrent_U6I0>0 && U6I0_active && U6I0_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_UNDERCURR_U6I0);
	}
	else if (U6I0_active && U6I0_active_counter<=I_ROLLING_AVERAGE)
	{
		U6I0_active_counter++;
	}
	else if (U6I0_active_counter)
	{
		U6I0_active_counter=0;
	}

	if ((U7I0_real>warn_overcurrent_U7I0) && (U7I0_active) && U7I0_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_OVERCURR_U7I0);
	}
	if (U7I0_real<warn_undercurrent_U7I0 && warn_undercurrent_U7I0>0 && U7I0_active && U7I0_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_UNDERCURR_U7I0);
	}
	else if (U7I0_active && U7I0_active_counter<=I_ROLLING_AVERAGE)
	{
		U7I0_active_counter++;
	}
	else if (U7I0_active_counter)
	{
		U7I0_active_counter=0;
	}
}

void Check_I1_Warn()
{
	//for currents calculation is done in the interrupt
	if ((U5I1_real>warn_overcurrent_U5I1) && (U5I1_active) && U5I1_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_OVERCURR_U5I1);
	}
	if (U5I1_real<warn_undercurrent_U5I1 && warn_undercurrent_U5I1>0 && U5I1_active && U5I1_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_UNDERCURR_U5I1);
	}
	else if (U5I1_active && U5I1_active_counter<=I_ROLLING_AVERAGE)
	{
		U5I1_active_counter++;
	}
	else if (U5I1_active_counter)
	{
		U5I1_active_counter=0;
	}

	if ((U6I1_real>warn_overcurrent_U6I1) && (U6I1_active) && U6I1_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_OVERCURR_U6I1);
	}
	if (U6I1_real<warn_undercurrent_U6I1 && warn_undercurrent_U6I1>0 && U6I1_active && U6I1_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_UNDERCURR_U6I1);
	}
	else if (U6I1_active && U6I1_active_counter<=I_ROLLING_AVERAGE)
	{
		U6I1_active_counter++;
	}
	else if (U6I1_active_counter)
	{
		U6I1_active_counter=0;
	}

	if ((U7I1_real>warn_overcurrent_U7I1) && (U7I1_active) && U7I1_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_OVERCURR_U7I1);
	}
	if (U7I1_real<warn_undercurrent_U7I1 && warn_undercurrent_U7I1>0 && U7I1_active && U7I1_active_counter>I_ROLLING_AVERAGE)
	{
		Set_Error(WARN_UNDERCURR_U7I1);
	}
	else if (U7I1_active && U7I1_active_counter<=I_ROLLING_AVERAGE)
	{
		U7I1_active_counter++;
	}
	else if (U7I1_active_counter)
	{
		U7I1_active_counter=0;
	}
}

void Check_T_Warn()
{
	uint32_t U5T_raw=0; uint32_t U6T_raw=1; uint32_t U7T_raw=0;
	for(uint32_t i=0; i<T_ROLLING_AVERAGE; i++) //this has possibility to overflow if ROLLING_AVERAGE > 2^(32-10) (reading 10 bit value)
	{
		U5T_raw+=U5T[i];
		U6T_raw+=U6T[i];
		U7T_raw+=U7T[i];
	}
	U5T_raw/=T_ROLLING_AVERAGE; U6T_raw/=T_ROLLING_AVERAGE; U7T_raw/=T_ROLLING_AVERAGE;
	U5T_real=Parse_Temperature(U5T_raw); U6T_real=Parse_Temperature(U6T_raw); U7T_real=Parse_Temperature(U7T_raw);

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
}

void Check_V_Warn()
{
	uint32_t U5V_raw=0; uint32_t U6V_raw=0; uint32_t U7V_raw=0; uint32_t U5GNDV_raw=0; uint32_t U6GNDV_raw=0; uint32_t U7GNDV_raw=0;
	for(uint32_t i=0; i<V_ROLLING_AVERAGE; i++) //this has possibility to overflow if ROLLING_AVERAGE > 2^(32-10) (reading 10 bit value)
	{
		U5V_raw+=U5V[i]; U5GNDV_raw+=U5GNDV[i];
		U6V_raw+=U6V[i]; U6GNDV_raw+=U6GNDV[i];
		U7V_raw+=U7V[i]; U7GNDV_raw+=U7GNDV[i];
	}
	U5V_raw/=V_ROLLING_AVERAGE; U6V_raw/=V_ROLLING_AVERAGE; U7V_raw/=V_ROLLING_AVERAGE; U5GNDV_raw/=V_ROLLING_AVERAGE; U6GNDV_raw/=V_ROLLING_AVERAGE; U7GNDV_raw/=V_ROLLING_AVERAGE;
	U5V_real=Parse_Voltage(U5V_raw, U5GNDV_raw); U6V_real=Parse_Voltage(U6V_raw, U6GNDV_raw); U7V_real=Parse_Voltage(U7V_raw, U7GNDV_raw);

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
}


void Check_I0_Switch()
{
	//for currents calculation is done in the interrupt
	if ((U5I0_real>cutoff_overcurrent_U5I0) && (U5I0_active) && U5I0_active_counter>I_ROLLING_AVERAGE)
	{
		U5I0_error=1;
		HAL_GPIO_WritePin(U5IN0.PORT, U5IN0.PIN, 0);
		U5I0_active=0;
		Set_Error(ERROR_OVERCURR_TRIP_U5_0);
	}
	if ((U6I0_real>cutoff_overcurrent_U6I0) && (U6I0_active) && U6I0_active_counter>I_ROLLING_AVERAGE)
	{
		U6I0_error=1;
		HAL_GPIO_WritePin(U6IN0.PORT, U6IN0.PIN, 0);
		U6I0_active=0;
		Set_Error(ERROR_OVERCURR_TRIP_U6_0);
	}
	if ((U7I0_real>cutoff_overcurrent_U7I0) && (U7I0_active) && U7I0_active_counter>I_ROLLING_AVERAGE)
	{
		U7I0_error=1;
		HAL_GPIO_WritePin(U7IN0.PORT, U7IN0.PIN, 0);
		U7I0_active=0;
		Set_Error(ERROR_OVERCURR_TRIP_U7_0);
	}
}

void Check_I1_Switch()
{
	//for currents calculation is done in the interrupt
	if ((U5I1_real>cutoff_overcurrent_U5I1) && (U5I1_active) && U5I1_active_counter>I_ROLLING_AVERAGE)
	{
		U5I1_error=1;
		HAL_GPIO_WritePin(U5IN1.PORT, U5IN1.PIN, 0);
		U5I1_active=0;
		Set_Error(ERROR_OVERCURR_TRIP_U5_1);
	}
	if ((U6I1_real>cutoff_overcurrent_U6I1) && (U6I1_active) && U6I1_active_counter>I_ROLLING_AVERAGE)
	{
		U6I1_error=1;
		HAL_GPIO_WritePin(U6IN1.PORT, U6IN1.PIN, 0);
		U6I1_active=0;
		Set_Error(ERROR_OVERCURR_TRIP_U6_1);
	}
	if ((U7I1_real>cutoff_overcurrent_U7I1) && (U7I1_active) && U7I1_active_counter>I_ROLLING_AVERAGE)
	{
		U7I1_error=1;
		HAL_GPIO_WritePin(U7IN1.PORT, U7IN1.PIN, 0);
		U7I1_active=0;
		Set_Error(ERROR_OVERCURR_TRIP_U7_1);
	}
}
