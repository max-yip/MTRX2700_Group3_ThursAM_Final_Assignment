/*
 * beam.c
 *
 *  Created on: Apr 25, 2025
 *      Author: Paul Akle
 */

// beam.c
#include "beam.h"
#include "gpio.h"
#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"

/*
beam.c
Beam-break sensor module: counts IR beam interruptions
on PC1 and lights corresponding LEDs on PE8..PE15.
*/

// IR beam sensor pin definition
#define IR_PORT   GPIOC
#define IR_PIN    GPIO_PIN_1

// Discovery board LEDs mapping for beam count
#define LED_PORT  GPIOE
#define LED_MASK  (GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 \
                  | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)

// Static counter for number of beam breaks (0 to 8)
static volatile uint8_t beam_count = 0;

/*
Initialise the beam sensor and LEDs.
     - Enables required GPIO clocks
     - Configures PC1 as falling-edge EXTI input
     - Configures LEDs on PE8..PE15 as outputs
     - Enables and sets priority for EXTI1 interrupt
*/
void Beam_Init(void) {
    enable_beam_clocks();         // turn on GPIO clocks
    configure_beam_sensor_gpio(); // setup PC1 EXTI
    configure_beam_led_gpio();    // setup LED outputs

    // Configure NVIC: EXTI1 interrupt, preempt priority 2
    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

/*
Get the current number of beam breaks detected.
return uint8_t number of breaks (0..8)
*/
uint8_t Beam_GetCount(void) {
    return beam_count;
}

/*
Reset the beam break count and turn off all LEDs.
*/
void Beam_ResetCount(void) {
    beam_count = 0;
    HAL_GPIO_WritePin(LED_PORT, LED_MASK, GPIO_PIN_RESET);
}

/*
- EXTI callback invoked by HAL on any EXTI line.

- GPIO_Pin the pin number that triggered the interrupt
  We filter for PC1 falling edge to detect beam breaks.
  Each break increments beam_count and lights the next LED.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == IR_PIN &&                 // only handle PC1
        HAL_GPIO_ReadPin(IR_PORT, IR_PIN) == GPIO_PIN_RESET) {
        if (beam_count < 8) {
            beam_count++;
            // Determine which LED to light: PE8 + index
            uint16_t this_led = (uint16_t)(GPIO_PIN_8 << (beam_count - 1));
            HAL_GPIO_WritePin(LED_PORT, this_led, GPIO_PIN_SET);
        }
    }
}
