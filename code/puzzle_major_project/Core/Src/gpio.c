// gpio.c
#include "stm32f303xc.h"
#include "stm32f3xx_hal.h"
#include "gpio.h"

// ----- Servo GPIO Helpers -----

/*
Enable clocks for servo PWM: GPIOA and TIM2.
*/
void enable_servo_clocks(void) {
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;    // PA15 pin
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;    // Timer 2
}

/*
Configure PA15 as AF1 for TIM2_CH1 (servo PWM output).
*/
void configure_servo_gpio(void) {
    // Set PA15 to AF mode (10)
    GPIOA->MODER = (GPIOA->MODER & ~(3U << (15*2)))
                |  (2U << (15*2));
    // Select AF1 (TIM2_CH1) for PA15: AFR[1] bits [31:28]
    GPIOA->AFR[1] = (GPIOA->AFR[1] & ~(0xFU << 28))
                  |  (1U    << 28);
}

// ----- Beam Sensor GPIO Helpers -----

/*
Enable clocks for beam sensor (PC1) and Discovery LEDs (PE8..PE15).
*/
void enable_beam_clocks(void) {
    __HAL_RCC_GPIOC_CLK_ENABLE();  // IR sensor PC1
    __HAL_RCC_GPIOE_CLK_ENABLE();  // LEDs PE8..PE15
}

/*
Configure PC1 as EXTI1 falling-edge input with pull-up.
*/
void configure_beam_sensor_gpio(void) {
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin  = GPIO_PIN_1;
    gpio.Mode = GPIO_MODE_IT_FALLING;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &gpio);
}

/*
Configure PE8..PE15 as push-pull outputs for LEDs.
*/
void configure_beam_led_gpio(void) {
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin   = GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11
               | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &gpio);
}


/*
Enable clocks for GPIOC and ADC12 (flexpot inputs).
*/
void enable_flexpot_clocks(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN    // clock for PC0 & PC2
                 | RCC_AHBENR_ADC12EN;   // clock for ADC1/2
}

/*
Configure PC0 and PC2 as analog inputs for ADC channels IN6 & IN8.
*/
void configure_flexpot_gpio(void) {
    // PC0 (IN6) and PC2 (IN8) set to analog mode
    GPIOC->MODER |= (3U << (0 * 2))      // PC0
                  | (3U << (2 * 2));     // PC2
}
