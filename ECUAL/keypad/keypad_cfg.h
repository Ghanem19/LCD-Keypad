/*
 * keypad_cfg.h
 *
 *  Created on: Jan 20, 2020
 *      Author: eng-m
 */

#ifndef ECUAL_KEYPAD_KEYPAD_CFG_H_
#define ECUAL_KEYPAD_KEYPAD_CFG_H_
#include "Dio.h"

#define NUM_OF_ROWS   (3)
#define NUM_OF_COLUMNS (3)

#define PULL_UP   (HIGH)
#define PULL_DOWN (LOW)

#define PULL_MODE PULL_UP

#define ROWS_PORT GPIOC
#define COLUMNS_PORT GPIOC

#define INPUT_PORT 	COLUMNS_PORT
#define OUTPUT_PORT ROWS_PORT

#define ROW1_PIN  PIN0
#define ROW2_PIN  PIN1
#define ROW3_PIN  PIN2



#define COL1_PIN  PIN3
#define COL2_PIN  PIN4
#define COL3_PIN  PIN5

#define COLUMNS_PINS ((COL1_PIN)|(COL2_PIN)|(COL3_PIN))
#define ROWS_PINS (ROW1_PIN|ROW2_PIN|ROW3_PIN)

#define INPUT_PINS 	COLUMNS_PINS
#define OUTPUT_PINS ROWS_PINS


#endif /* ECUAL_KEYPAD_KEYPAD_CFG_H_ */
