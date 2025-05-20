/*
 * beam.c
 *
 *  Created on: Apr 25, 2025
 *      Author: Paul Akle
 */

// beam.c
#include "beam.h"
#include "stm32f303xc.h"   // or your CubeMX-generated stm32f3xx.h

// the beam sensor lives on PC1
#define IR_PORT   GPIOC
#define IR_PIN    GPIO_PIN_1

// the Discovery LEDs are PE8..PE15 (LD3..LD10)
#define LED_PORT  GPIOE
#define LED_MASK  (GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 \
                  | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)

// keep track of how many times the beam’s been broken
static volatile uint8_t beam_count = 0;

void Beam_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    // enable GPIOC clock for the IR sensor pin
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // PC1 = EXTI1 falling‐edge only, pull-up
    gpio.Pin  = IR_PIN;
    gpio.Mode = GPIO_MODE_IT_FALLING;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(IR_PORT, &gpio);

    // Make sure GPIOE clock is on (so LEDs work)
    __HAL_RCC_GPIOE_CLK_ENABLE();
    // You should also have MX_GPIO_Init() configure PE8–PE15 as outputs elsewhere

    // Enable EXTI1 interrupt at priority level 2
    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

uint8_t Beam_GetCount(void)
{
    return beam_count;
}

void Beam_ResetCount(void)
{
    beam_count = 0;
    // turn off all 8 LEDs
    HAL_GPIO_WritePin(LED_PORT, LED_MASK, GPIO_PIN_RESET);
}

// Called by HAL on any EXTI1 event; we only light on the falling edge:
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == IR_PIN &&
        HAL_GPIO_ReadPin(IR_PORT, IR_PIN) == GPIO_PIN_RESET)
    {
        if (beam_count < 8) {
            beam_count++;
            // compute which LED: PE8 + (beam_count-1)
            uint16_t this_led = (uint16_t)(GPIO_PIN_8 << (beam_count - 1));
            HAL_GPIO_WritePin(LED_PORT, this_led, GPIO_PIN_SET);
        }
    }
}
