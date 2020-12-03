/**
 * Program skeleton for the course "Programming embedded systems"
 *
 * Lab 1: the elevator control system
 */

/**
 * Class for keeping track of the car position.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "position_tracker.h"
#include "assert.h"

static void positionTrackerTask(void *params) 
{
	static int lastsensorvalue=0;
	PositionTracker *tracker = (PositionTracker*)params;
	 int cp=0;
	static int positionsensorstatus = 0;
	
 for(;;)
	{
		positionsensorstatus = GPIO_ReadInputDataBit(tracker->gpio, tracker->pin);
		if (positionsensorstatus==0)
			{
				lastsensorvalue=0;
			}
			else if (positionsensorstatus==1)
			{
			 xSemaphoreTake(tracker->lock, portMAX_DELAY);
			 if (lastsensorvalue==0)
			 {
				if (tracker->direction == Up)
				{
				 tracker->position = tracker->position + 1;
				 cp = 	tracker->position;
				}
				else if (tracker->direction == Down)
				{
				 tracker->position = tracker->position - 1;
				 cp = 	tracker->position;
				}
				lastsensorvalue = 1;
			}
			else if (lastsensorvalue==1)
			{			
				lastsensorvalue = 1;			
			}		
				xSemaphoreGive(tracker->lock);								
		 }	 
				vTaskDelay(tracker->pollingPeriod);
	}
}

void setupPositionTracker(PositionTracker *tracker,
                          GPIO_TypeDef * gpio, u16 pin,
						  portTickType pollingPeriod,
						  unsigned portBASE_TYPE uxPriority) {
  portBASE_TYPE res;

  tracker->position = 0;
  tracker->lock = xSemaphoreCreateMutex();
  assert(tracker->lock != NULL);
  tracker->direction = Unknown;
  tracker->gpio = gpio;
  tracker->pin = pin;
  tracker->pollingPeriod = pollingPeriod;
  res = xTaskCreate(positionTrackerTask, "position tracker",
                    80, (void*)tracker, uxPriority, NULL);
  assert(res == pdTRUE);
							
}

void setDirection(PositionTracker *tracker, Direction dir) {
	xSemaphoreTake(tracker->lock, portMAX_DELAY);
	tracker->direction = dir;	
	xSemaphoreGive(tracker->lock);		

}

s32 getPosition(PositionTracker *tracker) {
	vs32 position = 0;
		xSemaphoreTake(tracker->lock, portMAX_DELAY);
	  position = tracker->position;		
		xSemaphoreGive(tracker->lock);	

  return position;
}

