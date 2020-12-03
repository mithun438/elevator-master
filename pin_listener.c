/**
 * Program skeleton for the course "Programming embedded systems"
 *
 * Lab 1: the elevator control system
 */

/**
 * Functions listening for changes of specified pins
 */

#include "FreeRTOS.h"
#include "task.h"

#include "pin_listener.h"
#include "assert.h"


static int flag0 = 0, flag1 = 0, flag2 = 0, flag3 = 0, flag7 = 0, flag8 = 0,tmp=0,tmp1=0,tmp2=0;
  static int temp0,temp1,temp01,temp11,temp02,temp12;
enum KeyEvent { PIN_0_PRESSED, PIN_1_PRESSED, PIN_2_PRESSED, PIN_3_PRESSED, PIN_7_PRESSED, PIN_8_PRESSED};


static void pollPin(PinListener *listener,
																xQueueHandle pinEventQueue) {																						
enum KeyEvent event;
						tmp = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0);
										
								switch(tmp)
								{
									
									case 0:									
										temp1=0;
										if (temp0==0)
										{
											temp0=1;
											break;
										}
											
										else if(temp0==1)
										{
											temp0=2;											
											printf("PIN 0 released \n");
											break;
										}
											else if (temp0==2)
											{
												break;	
											}												
										
									case 1:										
										temp0=0;
										if (temp1==0)
										{											
											temp1=1;									
									  	break;
										}
										else if(temp1==1)
										{
											printf("  TO_FLOOR_1 \n");
											event =   TO_FLOOR_1;									
											xQueueSend(pinEventQueue, &event, portMAX_DELAY);
										  temp1=2;
											break;
										}	
										else if (temp1==2)
										{											
											break;
										}
								}
				tmp1 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1);
										
								switch(tmp1)
								{
									
									case 0:									
										temp11=0;
										if (temp01==0)
										{
											temp01=1;
											break;
										}
											
										else if(temp01==1)
										{
											temp01=2;											
											printf("PIN 1 released \n");
											break;
										}
											else if (temp01==2)
											{
												break;	
											}												
										
									case 1:										
										temp01=0;
										if (temp11==0)
										{											
											temp11=1;									
									  	break;
										}
										else if(temp11==1)
										{
											printf("  TO_FLOOR_2 \n");
											event =   TO_FLOOR_2;									
											xQueueSend(pinEventQueue, &event, portMAX_DELAY);
										  temp11=2;
											break;
										}	
										else if (temp11==2)
										{											
											break;
										}
								}
			
			tmp2 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2);
										
								switch(tmp2)
								{
									
									case 0:									
										temp12=0;
										if (temp02==0)
										{
											temp02=1;
											break;
										}
											
										else if(temp02==1)
										{
											temp02=2;											
											printf("PIN 2 released \n");
											break;
										}
											else if (temp02==2)
											{
												break;	
											}												
										
									case 1:										
										temp02=0;
										if (temp12==0)
										{											
											temp12=1;									
									  	break;
										}
										else if(temp12==1)
										{
											printf("TO_FLOOR_3 \n");
											event = TO_FLOOR_3;									
											xQueueSend(pinEventQueue, &event, portMAX_DELAY);
										  temp12=2;
											break;
										}	
										else if (temp12==2)
										{											
											break;
										}
								}

	switch(flag3)
			{
				
				case 0: 
								if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3))
								{
									flag3 = 1;
									printf("stop_flag is %d \n",stop_flag);	
									break;
								}
								break;
				case 1:
								if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3))
								{
									flag3 = 2;
									printf("stop Released \n");
									break;
								}
									break;
			  case 2:
								if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3))
								{
									flag3 = 3;		
									break;
								}
								break;
				case 3:
								if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3))
								{
									flag3 = 0;
									printf("stop Pressed \n");
									event = PIN_3_PRESSED;
									if (stop_flag==0)
									{
										stop_flag=1;
										setCarMotorStopped(1);	
									}
									else
									{
										setCarMotorStopped(0);
										stop_flag=0;	
									}
									break;
								}
									break;
			}			
			switch(flag7)
			{
				case 0: 
								if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))
								{
									flag7 = 1;
									break;
								}
								break;
				case 1:
								if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))
								{
									flag7 = 2;
									printf("PIN 7 Released \n");
									break;
								}
									break;
			  case 2:
								if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))
								{
									flag7 = 3;
									break;
								}
								break;
				case 3:
								if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7))
								{
									flag7 = 0;
									printf("PIN 7 Pressed \n");
									event = PIN_7_PRESSED;
									break;
								}
									break;
			}
		switch(flag8)
			{
				case 0: 
								if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
								{
									flag8 = 1;
									break;
								}
								break;
				case 1:
								if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
								{
									flag8 = 2;
									printf("PIN 8 Released \n");									
									break;
								}
									break;
			  case 2:
								if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
								{
									flag8 = 3;
									break;
								}
								break;
				case 3:
								if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
								{
									flag8 = 0;
									printf("PIN 8 Pressed \n");
									event = PIN_8_PRESSED;
									break;
								}
									break;
			}
			return;
}


static void pollPinsTask(void *params) {
  PinListenerSet listeners = *((PinListenerSet*)params);
  portTickType xLastWakeTime;
  int i; 
  xLastWakeTime = xTaskGetTickCount();

  for (;;) {
    for (i = 0; i < listeners.num; ++i)
	  pollPin(listeners.listeners + i, listeners.pinEventQueue);
    
	vTaskDelayUntil(&xLastWakeTime, listeners.pollingPeriod);
  }
}

void setupPinListeners(PinListenerSet *listenerSet) {
  portBASE_TYPE res;

  res = xTaskCreate(pollPinsTask, "pin polling",
                    100, (void*)listenerSet,
					listenerSet->uxPriority, NULL);
  assert(res == pdTRUE);
}
