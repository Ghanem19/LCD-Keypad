/*
 * main.c
 *
 *  Created on: Jan 21, 2020
 *      Author: Ghanem
 */

#include "common_retval.h"
#include "interrupt.h"
#include "keypad.h"
#include "lcd.h"
#include "sos.h"


void lcdInitTask(void)
{
	if((ERR_NOT_FINISHED + LCD_BASE ) == LCD_init())
	{
	}
	else
	{
		Sos_DeleteTask(lcdInitTask);
	}
}
void sendStringTask(void)
{
	uint8 string[] = "Pressed Key = ";
	if((ERR_NOT_FINISHED + LCD_BASE ) == LCD_sendString_RowCol(0,3,string))
	{

	}
	else
	{
		Sos_DeleteTask(sendStringTask);
	}

}

void keypadUpdateTask(void)
{
	Keypad_update();
}

void keypadReadTask(void)
{
	uint8 pressedKey = 0;
	Keypad_getPressedKey(&pressedKey);
	if(pressedKey != NO_KEY_PRESSED){
		LCD_displayCharacter(pressedKey);
	}
	else
	{

	}
}
int main(void)
{
	Keypad_Init();
	Sos_Init();
	str_taskInfo_t task1 = {lcdInitTask     ,1,10,0,TASK_WAITING};
	str_taskInfo_t task2 = {keypadUpdateTask,50,5,20,TASK_WAITING};
	str_taskInfo_t task3 = {keypadReadTask  ,50,4,20,TASK_WAITING};
	str_taskInfo_t task4 = {sendStringTask  ,15,1,10,TASK_WAITING};
	Sos_CreateTask(&task1);
	Sos_CreateTask(&task2);
	Sos_CreateTask(&task3);
	Sos_CreateTask(&task4);
	sei();

	Sos_Run();

}
