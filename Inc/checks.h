#ifndef CHECKS_H_
#define CHECKS_H_

//rolling average definitions
#define I_ROLLING_AVERAGE 32
#define T_ROLLING_AVERAGE 128
#define V_ROLLING_AVERAGE 128


//function prototypes
void Check_I0_Warn();
void Check_I1_Warn();
void Check_T_Warn();
void Check_V_Warn();

void Check_I0_Switch();
void Check_I1_Switch();


//external variables
extern uint32_t U5I0[I_ROLLING_AVERAGE];
extern uint32_t U5I0_real;
extern uint32_t U5I1[I_ROLLING_AVERAGE];
extern uint32_t U5I1_real;
extern uint32_t U5T[T_ROLLING_AVERAGE];
extern uint32_t U5T_real;
extern uint32_t U5V[V_ROLLING_AVERAGE];
extern uint32_t U5V_real;
extern uint32_t U5GNDV[V_ROLLING_AVERAGE];

extern uint32_t U6I0[I_ROLLING_AVERAGE];
extern uint32_t U6I0_real;
extern uint32_t U6I1[I_ROLLING_AVERAGE];
extern uint32_t U6I1_real;
extern uint32_t U6T[T_ROLLING_AVERAGE];
extern uint32_t U6T_real;
extern uint32_t U6V[V_ROLLING_AVERAGE];
extern uint32_t U6V_real;
extern uint32_t U6GNDV[V_ROLLING_AVERAGE];

extern uint32_t U7I0[I_ROLLING_AVERAGE];
extern uint32_t U7I0_real;
extern uint32_t U7I1[I_ROLLING_AVERAGE];
extern uint32_t U7I1_real;
extern uint32_t U7T[T_ROLLING_AVERAGE];
extern uint32_t U7T_real;
extern uint32_t U7V[V_ROLLING_AVERAGE];
extern uint32_t U7V_real;
extern uint32_t U7GNDV[V_ROLLING_AVERAGE];

extern uint8_t U5I0_active;
extern uint8_t U5I1_active;
extern uint8_t U6I0_active;
extern uint8_t U6I1_active;
extern uint8_t U7I0_active;
extern uint8_t U7I1_active;

#endif /* CHECKS_H_ */
