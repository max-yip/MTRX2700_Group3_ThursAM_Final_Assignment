#include "stm32f3xx_hal.h"
#include "lcd.h"
#include <string.h>

// LCD Enable Pulse
static void LCD_EnablePulse(void) {
    HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
}

// Send 4 bits to LCD
static void LCD_Send4Bits(uint8_t data) {
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (data >> 0) & 0x01);
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (data >> 1) & 0x01);
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (data >> 2) & 0x01);
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (data >> 3) & 0x01);
    LCD_EnablePulse();
}

// Send a command to the LCD
void LCD_SendCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);  // RS = 0 for command
    LCD_Send4Bits(cmd >> 4);  // Send the high nibble
    LCD_Send4Bits(cmd & 0x0F);  // Send the low nibble
    HAL_Delay(2);  // Small delay for command execution
}

// Send data (character) to the LCD
void LCD_SendData(uint8_t data) {
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);  // RS = 1 for data
    LCD_Send4Bits(data >> 4);  // Send the high nibble
    LCD_Send4Bits(data & 0x0F);  // Send the low nibble
    HAL_Delay(2);  // Small delay for data processing
}

// Send a string to the LCD
void LCD_SendString(char* str) {
    while (*str) {
        LCD_SendData(*str++);  // Send each character
    }
}

// Clear the LCD screen
void LCD_Clear(void) {
    LCD_SendCommand(0x01);  // Clear command
    HAL_Delay(2);  // Delay for the LCD to clear
}

// Initialize the LCD
void LCD_Init(void) {
    HAL_Delay(40);  // Wait for LCD to power up
    LCD_Send4Bits(0x03);  // Initialize in 8-bit mode first
    HAL_Delay(5);
    LCD_Send4Bits(0x03);  // Repeat to make sure
    HAL_Delay(1);
    LCD_Send4Bits(0x03);  // Repeat one more time
    LCD_Send4Bits(0x02);  // Switch to 4-bit mode

    LCD_SendCommand(0x28);  // Function set: 4-bit, 2 lines, 5x8 font
    LCD_SendCommand(0x0C);  // Display ON, Cursor OFF
    LCD_SendCommand(0x06);  // Entry mode: Auto increment, No shift
    LCD_Clear();  // Clear display for a fresh start
}

// Write a single character to the LCD
void LCD_WriteChar(char c) {
    LCD_SendData(c);  // Just send the character as data
}
