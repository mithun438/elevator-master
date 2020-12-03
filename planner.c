/**
 * Program skeleton for the course "Programming embedded systems"
 *
 * Lab 1: the elevator control system
 */

/**
 * The planner module, which is responsible for consuming
 * pin/key events, and for deciding where the elevator
 * should go next
 */

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pin_listener.h"
#include "global.h"
#include "planner.h"
#include "assert.h"

#define MOTOR_UPWARD   (TIM3->CCR1)
#define MOTOR_DOWNWARD (TIM3->CCR2)
#define MOTOR_STOPPED  (!MOTOR_UPWARD && !MOTOR_DOWNWARD)
#define floor_1_pos 0
#define floor_2_pos 400
#define floor_3_pos 800

enum KeyEvent {PIN_1_PRESSED, PIN_2_PRESSED, PIN_3_PRESSED, PIN_7_PRESSED, PIN_8_PRESSED};
enum KeyEvent data;

void Close_Doors(void);
void OpenDoors(void);

void OpenDoors(void)
{
	GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
	printf("DOORS OPENED \n");
	vTaskDelay(1200 / portTICK_RATE_MS);
}

void Close_Doors(void)
{

	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);

		printf("door closed \n");
	}
}


static void plannerTask(void *params) {
	int current_task,current_position;
	bool enable=1;
	enum KeyEvent data,queue_data;
	for(;;)
	{
		xQueueReceive(pinEventQueue, &queue_data, portMAX_DELAY);
	if(enable==1)
	{
		switch(queue_data)
		{
			case TO_FLOOR_1:
				Close_Doors();
				enable=0;		
				setCarTargetPosition(floor_1_pos);			
					printf("going to 1 \n");
				current_position=getCarPosition();			
			f1:
			if ((current_position>=(floor_1_pos)) && (current_position<(floor_1_pos+1)))
			{				
				OpenDoors();
					enable=1;								
			}
			else
			{				
				current_position = getCarPosition();					
				goto f1;
			}

				enable=1;
			break;

				break;
		
			case TO_FLOOR_2:
				Close_Doors();
				enable=0;
				setCarTargetPosition(floor_2_pos);
				printf("going to 2 \n");
			current_position = getCarPosition();
			f2: 
			{		
			if ((current_position>(floor_2_pos-0.5)) && (current_position<(floor_2_pos+1)))
				{				
					OpenDoors();
					enable=1;
				}	
			else
			{
				current_position = getCarPosition();
				goto f2;
			}
			
			enable=1;			
			break;
	
			case TO_FLOOR_3:
				Close_Doors();
				enable=0;
				setCarTargetPosition(floor_3_pos);
				printf("going to 3 \n");
				current_position = getCarPosition();
			f3:
			if ((current_position>(floor_3_pos-1)) && (current_position<=floor_3_pos))
				{				
						OpenDoors();
					enable=1;
				}
			else
			{
				current_position = getCarPosition();
				goto f3;
			} 
			enable=1;
			break;
			
			default:
				printf("queue empty \n");
				enable=1;
			break;
		}
	}
}
	else			
				printf("else statement running \n");
}
}

void setupPlanner(unsigned portBASE_TYPE uxPriority) {
  xTaskCreate(plannerTask, "planner", 100, NULL, uxPriority, NULL);
}
