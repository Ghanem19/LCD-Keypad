/*
 * keypad.h
 *
 *  Created on: Jan 20, 2020
 *      Author: eng-m
 */

#ifndef ECUAL_KEYPAD_KEYPAD_H_
#define ECUAL_KEYPAD_KEYPAD_H_

#include "std_types.h"


#define NO_KEY_PRESSED 		(0U)
#define ASCII_SHIFT			(49U)


ERR_STATUS Keypad_Init(void);
ERR_STATUS Keypad_getPressedKey(uint8* u8_pressedKey);
ERR_STATUS Keypad_update(void);

#endif /* ECUAL_KEYPAD_KEYPAD_H_ */
