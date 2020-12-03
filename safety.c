/**
 * Program skeleton for the course "Programming embedded systems"
 *
 * Lab 1: the elevator control system
 */

/**
 * This file defines the safety module, which observes the running
 * elevator system and is able to stop the elevator in critical
 * situations
 */

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>

#include "global.h"
#include "assert.h"

#define POLL_TIME (10 / portTICK_RATE_MS)

#define MOTOR_UPWARD   (TIM3->CCR1)
#define MOTOR_DOWNWARD (TIM3->CCR2)
#define MOTOR_STOPPED  (!MOTOR_UPWARD && !MOTOR_DOWNWARD)

#define STOP_PRESSED  GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)
#define AT_FLOOR      GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)
#define DOORS_CLOSED  GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)

static portTickType xLastWakeTime;


static void check(u8 assertion, char *name) {
  if (!assertion) {
    printf("SAFETY REQUIREMENT %s VIOLATED: STOPPING ELEVATOR\n", name);
    for (;;) {
	  setCarMotorStopped(1);
  	  vTaskDelayUntil(&xLastWakeTime, POLL_TIME);
    }
  }
}

static void safetyTask(void *params) {
  s16 timeSinceStopPressed = -1;
	s16 timeSinceArrivedAtFloor = -1;	
	s16 timeSinceLastCheck = -1;
	s16 motorspeed = 0;
	s16 timeSinceDoorOpen = -1;
	s16 timeSinceElevatorMoving = 0;
	static int elv_postn, current_postn=0, prev_postn=0, postn,flag=0,flag2=0,start=1;

  xLastWakeTime = xTaskGetTickCount();

	
  for (;;) {
		// assumption 2: The elevator moves at a maximum speed of 50cm/s	
		if (timeSinceLastCheck < 0)
		{
			timeSinceLastCheck = 0;
			current_postn = getCarPosition();	
		}
		else
		{
			if (timeSinceLastCheck >= 1000)
			{					
				motorspeed = abs(current_postn - prev_postn)/5;
						check(motorspeed <= 50, "env2");
				prev_postn = current_postn;
				timeSinceLastCheck = -1;
			}
			else
			{
				timeSinceLastCheck += POLL_TIME;
			}
		}	
				/*		// assumption 3: If the ground floor is put at 0cm in an absolute coordinate system,
								the second floor is at 400cm and the third floor at 800cm 
									(the at-floor sensor reports a floor with a threshold of +-0.5cm) */
	if(AT_FLOOR)
	{
		elv_postn = getCarPosition();	
		check(((elv_postn >= -1 && elv_postn <= 1) || (elv_postn >= 399 && elv_postn <= 401) || (elv_postn >= 799 && elv_postn <= 801 )), "env 3");
	}
		// assumption 4: Once the door has been opened, it should remain open for atleast 1 seconds.
		if (!DOORS_CLOSED)
		{
			flag=1;
			if (timeSinceDoorOpen < 0)
				timeSinceDoorOpen = 0;
			else 
			{
				timeSinceDoorOpen += POLL_TIME;
			}	
		}

	if (DOORS_CLOSED && flag==1)
			{
				check(timeSinceDoorOpen * portTICK_RATE_MS >= 1000, "env4");
				flag=0;			
			}
	else 
			timeSinceDoorOpen = -1;

    // System requirement 1: if the stop button is pressed, the motor is
	//                       stopped within 1s

	if (STOP_PRESSED) {
	  if (timeSinceStopPressed < 0)
	    timeSinceStopPressed = 0;
      else
	    timeSinceStopPressed += POLL_TIME;

      check(timeSinceStopPressed * portTICK_RATE_MS <= 1000 || MOTOR_STOPPED,"req1");
	} else {
	  timeSinceStopPressed = -1;
	}


    // System requirement 2: the motor signals for upwards and downwards
	//                       movement are not active at the same time
		
    check(!MOTOR_UPWARD || !MOTOR_DOWNWARD,"req2");

	// safety requirement 3: The elevator may not pass the end positions, 
	//													that is, go through the roof or the floor
	elv_postn = getCarPosition();
	check(elv_postn >= 0 && elv_postn <= 800, "req3"); 

	//	safety requirement 6: The motor should not be moving without stopping for more than 120 seconds.
	
	if(!MOTOR_STOPPED)
	{
			if(timeSinceElevatorMoving<0)
				timeSinceElevatorMoving = 0;
			else
				timeSinceElevatorMoving += POLL_TIME;
			check(timeSinceElevatorMoving * portTICK_RATE_MS <= 120000, "req6");		
	}
	else
			timeSinceElevatorMoving = -1;	
	

	// fill in safety requirement 7
	check(1, "req7");
	vTaskDelayUntil(&xLastWakeTime, POLL_TIME);
  }
}


void setupSafety(unsigned portBASE_TYPE uxPriority) {
  xTaskCreate(safetyTask, "safety", 100, NULL, uxPriority, NULL);
}
