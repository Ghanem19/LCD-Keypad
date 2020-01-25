/*-
 * keypad.c
 *
 *  Created on: Jan 20, 2020
 *      Author: eng-m
 */

/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "Dio.h"
#include "common_retval.h"
#include "keypad_cfg.h"
#include "keypad.h"
/************************************************************************/
/*		                        Defines                 		        */
/************************************************************************/

#define RELEASED 	  (enum_keypadButtonStates_t)(0U)
#define PREPRESSED    (enum_keypadButtonStates_t)(1U)
#define PRESSED 	  (enum_keypadButtonStates_t)(2U)
#define PRERELEASED   (enum_keypadButtonStates_t)(3U)
/* function like macros */
#define GET_KEY_INDEX(ROW,COL)  (COL + (ROW*NUM_OF_ROWS))
/************************************************************************/
/*		                   	IMPLICIT TYPES              		        */
/************************************************************************/
typedef uint8 enum_keypadButtonStates_t; /* represent the state of keypad keys */
/************************************************************************/
/*		                       Global Variables           		        */
/************************************************************************/

enum_initState_t genum_keypadState = UNINITIALIZED;

uint8 gau8_keypadButtonsValues[NUM_OF_ROWS*NUM_OF_COLUMNS] = {0}; /* last read value for each button */

/* state of each button because the state machine depend on current state and input */
enum_keypadButtonStates_t gaenum_buttonsStatus[NUM_OF_ROWS*NUM_OF_COLUMNS] = {0};

/* pins of rows and column to move between them on the loops */
const uint8 gau8_outputPins[NUM_OF_ROWS] = {ROW1_PIN,ROW2_PIN,ROW3_PIN};
const uint8 gau8_inputPins[NUM_OF_COLUMNS] = {COL1_PIN,COL2_PIN,COL3_PIN};

/************************************************************************/
/*		              	EXTERN FUNCTIONS' IMPLEMENTATION   		        */
/************************************************************************/
/*
 * Function: Keypad_Init
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: add the passed task to buffer to be executed within certain periodic.
 */
ERR_STATUS Keypad_Init(void)
{
	ERR_STATUS keypadInitErr = ERR_SUCCESS;
	uint8 u8_index = 0;
	if(genum_keypadState == INITIALIZED)
	{
		/* can't initialize the keypad as it's already initialized */
		keypadInitErr = ERR_ALREADY_INITIALIZED;
	}
	else
	{
		/* DIO pins initialization */
		DIO_Cfg_s str_keypadInputs =
								{ INPUT_PORT,
								  INPUT_PINS,
								  INPUT };
		DIO_Cfg_s str_keypadOuputs =
								{ OUTPUT_PORT,
								  OUTPUT_PINS,
								  OUTPUT };
		if(E_NOK == DIO_init(&str_keypadOuputs))
		{
			keypadInitErr = ERR_DIO;
		}
		else
		{

			if(E_NOK == DIO_init(&str_keypadInputs))
			{
				keypadInitErr = ERR_DIO;
			}
			else
			{
				/* configure the input pins if PULL_UP or not by writing to them HIGH or LOW*/
				if(E_NOK == DIO_Write(str_keypadInputs.GPIO, str_keypadInputs.pins, PULL_MODE))
				{
					keypadInitErr = ERR_DIO;
				}
				else
				{
					/* writing to output pins depending on the selected pull mode*/
					if(E_NOK == DIO_Write(str_keypadOuputs.GPIO, str_keypadOuputs.pins,	PULL_MODE))
					{
						keypadInitErr = ERR_DIO;

					}
					else
					{
						/* initialize the whole global variables */
						for(u8_index = 0;u8_index < NUM_OF_ROWS*NUM_OF_COLUMNS; u8_index++)
						{
							gau8_keypadButtonsValues[u8_index] = HIGH;
							gaenum_buttonsStatus[u8_index] = RELEASED;

						}
						/* indicate to other functions that keypad initialized successfully */
						genum_keypadState = INITIALIZED;
					}
				}
			}
		}
	}

	return keypadInitErr + KEYPAD_BASE;
}
/*
 * Function: Keypad_getPressedKey.
 *
 * Outputs:	*ptru8_pressedKey -> the index of pressed key would be passed through this pointer.
 *
 * Return: 	ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: loop on the buttons to get the pressed one the value of ptru8_pressedKey would be zero if non of keys is pressed.
 */
ERR_STATUS Keypad_getPressedKey(uint8* ptru8_pressedKey)
{
	/* local variables initialization */
	uint8 u8_keyIndex = 0;
	ERR_STATUS keypadGetKeyErr = ERR_SUCCESS;
	/* check initialization */
	if(genum_keypadState == UNINITIALIZED)
	{
		keypadGetKeyErr = ERR_NOTINITIALIZED;
	}
	else
	{
		/* NULL pointer check */
		if(ptru8_pressedKey == NULL)
		{
			keypadGetKeyErr = ERR_NULL_PTR;
		}
		else
		{
			/* loop on the keys
			 * update their status
			 * detect if their any one pressed */
			for(u8_keyIndex = 0; u8_keyIndex<NUM_OF_ROWS*NUM_OF_COLUMNS; u8_keyIndex++)
			{
				/* initialized pressed key by zero to detect if it's changed */
				*ptru8_pressedKey = NO_KEY_PRESSED;
				switch(gaenum_buttonsStatus[u8_keyIndex])
					{
						case RELEASED:
							if(gau8_keypadButtonsValues[u8_keyIndex] == PULL_MODE)
							{
								/* at released state without any pressing stay in same state */
							}
							else
							{
								/* move to to pre_pressed state if logic 1 is detected */
								gaenum_buttonsStatus[u8_keyIndex] = PREPRESSED;
							}
							break;

						case PREPRESSED:
							if(gau8_keypadButtonsValues[u8_keyIndex] == PULL_MODE)
							{
								/* if logic 0 have been detected that's mean the first press is bouncing */
								gaenum_buttonsStatus[u8_keyIndex] = PRERELEASED;
							}
							else
							{
								/* if logic 1 have been detected that's mean it was real press so move to PRESSED state */
								gaenum_buttonsStatus[u8_keyIndex] = PRESSED;
								/*store the key index as hear to get the multiple presses */
								*ptru8_pressedKey = u8_keyIndex + ASCII_SHIFT;
							}
							break;

						case PRESSED:
							if(gau8_keypadButtonsValues[u8_keyIndex] == PULL_MODE)
							{
								/* move to to PRERELEASED state if logic 0 is detected */
								gaenum_buttonsStatus[u8_keyIndex] = PRERELEASED;
							}
							else
							{
								/* stay on pressed state as logic 1 is detected */
								/* suppose to start timer here to detect if it's long press and send the number again */
							}
							break;

						case PRERELEASED:
							if(gau8_keypadButtonsValues[u8_keyIndex] == PULL_MODE)
							{
								/* if logic 0 have been detected that's ensure button is released */
								gaenum_buttonsStatus[u8_keyIndex] = RELEASED;
							}
							else
							{
								/* logic 1 detected which indicate that the previous logic 0 is bouncing */
								gaenum_buttonsStatus[u8_keyIndex] = PREPRESSED;
							}
							break;

						default:
							keypadGetKeyErr = ERR_LOGICAL;
						break;
					}

				if(*ptru8_pressedKey != 0)
				{
					break;
				}
				else
				{

				}
			}
		}

	 }
	return keypadGetKeyErr + KEYPAD_BASE ;

}
/*
 * Function: Keypad_update.
 *
 * Return: 	ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: write one the rows and if any button pressed the it's column would the written value on row
 * and vice versa.
 */
ERR_STATUS Keypad_update(void)
{
	/* local variables initialization */
	ERR_STATUS keypadUpdateErr = ERR_SUCCESS;
	uint8 u8_rowIndex = 0,u8_colIndex = 0;

	/* check initialization */
	if(genum_keypadState == UNINITIALIZED)
	{
		keypadUpdateErr = ERR_NOTINITIALIZED;
	}
	else
	{
		/* loop on the rows */
		for(u8_rowIndex = 0;u8_rowIndex < NUM_OF_ROWS; u8_rowIndex++)
		{
			/* set one of rows low and the others is high depending on pull mode */
			DIO_Write(OUTPUT_PORT, OUTPUT_PINS, PULL_MODE);
			DIO_Write(OUTPUT_PORT, gau8_outputPins[u8_rowIndex], (uint8)(~PULL_MODE));

			for(u8_colIndex = 0;u8_colIndex < NUM_OF_COLUMNS; u8_colIndex++)
			{
				/* check for each column and store the value of their pins */
				DIO_Read(COLUMNS_PORT, gau8_inputPins[u8_colIndex],\
					&gau8_keypadButtonsValues[GET_KEY_INDEX(u8_rowIndex,u8_colIndex)]);

			}

		}
	}

	return keypadUpdateErr + KEYPAD_BASE;
}
