/*****************************************************************************
** 
**	Timer16 Driver for the self wake up example
** 
*****************************************************************************/

#ifndef __TIMER_16_H
#define __TIMER_16_H

#include "LPC11xx.h"
/*****************************************************************************
**	Function:	Init_Timer	Sets up basic counting functions
**	Parameters:	TimerNum	Select (0) for Timer0 and (1) for Timer1
**				SystemFreq	Specify the system clock freq (output of AHBCLKDIV)
**				MatchOutputInterval	Specify the interval of the Match Output in ms
**					matchOutputInterval should be in multiples of 10ms or greater!
**	Return:		-None-
*****************************************************************************/
void Init_Timer( uint32_t TimerNum, uint32_t SystemFreq, uint32_t matchOutputInterval);

/*****************************************************************************
**	Function:	DeInit_Timer	Deinitializes (turns off) the Timer
**	Parameters:	TimerNum		Select (0) for Timer0 and (1) for Timer1
**	Return:		-None-
*****************************************************************************/
void DeInit_Timer( uint32_t TimerNum );

/*****************************************************************************
**	Function:	Enable_Timer	Starts the timer from 0
**	Parameters:	TimerNum		Select (0) for Timer0 and (1) for Timer1
**	Return:		-None-
*****************************************************************************/
void Enable_Timer ( uint32_t TimerNum );

/*****************************************************************************
**	Function:	Disable_Timer	Stops and Reset the Timer
**	Parameters:	TimerNum		Select (0) for Timer0 and (1) for Timer1
**	Return:		-None-
*****************************************************************************/
void Disable_Timer ( uint32_t TimerNum );

/*****************************************************************************
**	Function:	Clear_MAT0		Clears the External match register 0 output
**	Parameters:	TimerNum		Select (0) for Timer0 and (1) for Timer1
**	Return:		-None-
*****************************************************************************/
void Clear_MAT0( uint32_t TimerNum );


#endif /* __TIMER_16_H */
