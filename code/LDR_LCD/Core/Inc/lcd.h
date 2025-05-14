#ifndef __LCD_H__
#define __LCD_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f3xx_hal.h"
#include <stdint.h>
#include "main.h"


// Pin configuration for the LCD (ensure these match with your main.h configuration)
#define LCD_RS_GPIO_Port GPIOD
#define LCD_RS_Pin       GPIO_PIN_8

#define LCD_RW_GPIO_Port GPIOD
#define LCD_RW_Pin       GPIO_PIN_9  // RW is typically tied to ground, as you're only writing to the LCD

#define LCD_EN_GPIO_Port GPIOD
#define LCD_EN_Pin       GPIO_PIN_10

#define LCD_D4_GPIO_Port GPIOD
#define LCD_D4_Pin       GPIO_PIN_11
#define LCD_D5_GPIO_Port GPIOD
#define LCD_D5_Pin       GPIO_PIN_12
#define LCD_D6_GPIO_Port GPIOD
#define LCD_D6_Pin       GPIO_PIN_13
#define LCD_D7_GPIO_Port GPIOD
#define LCD_D7_Pin       GPIO_PIN_14

// Function prototypes for LCD control
void delay_us(uint32_t us);       					//delay in us

void LCD_SendCMD(uint8_t cmd);						//sending command to LCD

void LCD_SendData(uint8_t data);					//send character to LCD

void LCD_Init();									//initialize LCD in 4-bit mode

void LCD_Clear();									//clear LCD

void LCD_SendStr(char* str);						//send string to LCD

void LCD_SetCursor(uint8_t row, uint8_t col);		//set the cursor to the desired row & column

void LCD_ShiftRight();								//shift the display to the right

void LCD_ShiftLeft();								//shift the display to the left



#endif // __LCD_H__
