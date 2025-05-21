# MTRX2700_Group3_ThursAM_Final_Assignment

## Group Members
Paul Akle 530483944 <br>
Max Yip 520434307 <br>
Siya Sharma 510451145 <br>
Shuvayu Banerjee 540752353 <br>

## Assignment Purpose

## Roles and Responsibilities
Paul Akle <br>
Max Yip <br>
Siya Sharma <br>
Shuvayu Banerjee <br>


## Project Overview
<details>
  <summary>Challenge 1</summary>
  
  ### Triangulating Planets with PTU
  
</details>
<details>
  <summary>Challenge 2</summary>
  
  ### Asteroid Shooting
  
</details>
<details>
  <summary>Challenge 3</summary>
  
  ### Map Coordinates with Flexible Potentiometer
  
</details>

<details>
  <summary>Challenge 4</summary>

  ### Assembling the Lightsaber

</details>


## Modules
<details>
  <summary>GUI interface</summary>
  
  ### GUI
  - briefly explain what the gui is for
  - how to connect to the board and set it up
  - buttons and controls available
  
</details>

<details>
  <summary>PTU</summary>
  
  ### PTU module
  - briefly explain the challenge
  - how to connect to the board and set it up
  - buttons and controls available

  <summary>PTU servo drivers</summary>
  
  ### PTU module
  - briefly explain the challenge
  - how to connect to the board and set it up
  - buttons and controls available
  
  <summary>PTU lidar</summary>
  
  ### PTU module
  - briefly explain the challenge
  - how to connect to the board and set it up
  - buttons and controls available

</details>

<details>
  <summary>Flex pot sensor</summary>
  
  ### Summary
  This module configures ADC1 on PC0 (IN6) and PC2 (IN8) to read two flexible potentiometers. It provides a simple API to initialise the ADC hardware and to perform one-shot conversions on a given channel, mapping the 12-bit raw value into a discrete “position” from 1 to 6 or 0 for no-touch.
  
  ### Usage
  At first ‘FlexPot_Init();’ is called to enable clocks, calibrate the ADC, and initialise the flexible potentiometers. Whenever a reading on one of the potentiometers is required, the file calls ‘uint8_t pos = FlexPot_GetPosition(channel);’ where channel is 6 for PC0 or 8 for PC2. Therefore, the returning value position will be 0 if no touch (raw < NO_TOUCH_THRESHOLD) or a value 1 to 6 corresponding to changing positions of pressure along the potentiometer. 
  
  ### Valid Input
  The channel must be one of 6, which reads PC0 (ADC_IN6) or 8 which reads PC2 (ADC_IN8). Internally, any raw ADC counts < 100 are treated as “no-touch”. Raw counts ≥ 100 are linearly mapped to positions 1 to 6 along the length of the potentiometer – 1 being at the base and 6 being at the top.
  
  ### Functions and Modularity
  The ‘void FlexPot_Init(void)’ function enables GPIOC & ADC12 clocks and sets PC0 and PC2 to analog mode. It also powers up, calibrates, and enables analog to digital conversion to occur (ADC1).
  Following this ‘uint8_t FlexPot_GetPosition(uint8_t channel)’ selects the channel in SQR1 and starts each ADC conversion and waiting for the end of the conversion begin the next one. The it reads ADC1->DR which applies a threshold check and maps the 4095 flexible potentiometer steps into 7 values, 0 to 6.
  
  ### Testing
  In order to test edge cases, the potentiometer is left untouched where it should read 0. Then for the other values the potentiometer is touched at endpoints where it should read 1 or 6.
  To test the core codes integration with the main file for the puzzle design print both X/Y positions over UART every interval and confirm the displayed numbers match your physical presses.

  
</details>

<details>
  <summary>Break Beam Sensor </summary>
  
  ### Summary
  This module configures PC1 as an EXTI1 (falling‐edge) interrupt input to detect when an IR beam is broken. Each beam break increments a counter (up to 8) and lights the corresponding Discovery-board LED (PE8…PE15). A simple API lets you query or reset the count (and LEDs).
  
  ### Usage
  At the start of the file ‘Beam_Init();’ is called to enable GPIOC/E clocks, configure PC1 for falling‐edge EXTI, and enable the EXTI1 interrupt. On every beam break the HAL ISR callback will increment beam_count and set the next LED. In the application for the puzzle design ‘uint8_t n = Beam_GetCount();’ is used to read how many breaks have occurred and displays this on the LEDs on the STM board.
  
  ### Valid Input
  The user must pass an object through the sensor which increments ‘beam_count’ only on a falling edge of PC1 (beam broken) and saturates at 8. Also, if there are no inputs the player will lose the game.
  
  ### Functions and Modularity
  First, the module ‘void Beam_Init(void)’ enables the GPIOC clock and configures PC1 as EXTI1 falling‐edge. The module also enables the GPIOE clock (for LEDs) and assumes PE8 to PE15 are outputs for each time the beam is interfered by an object.
  The function ‘uint8_t Beam_GetCount(void)’ is used to returns the current beam_count. ‘void Beam_ResetCount(void)’ then clears beam_count and turns turns off the LEDs on the STM discovery board.
  The module ‘void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)’ is called by HAL on any EXTI line and when GPIO_Pin == IR_PIN and the pin reads LOW, increments beam_count if they are not all on already and lights the next LED.
  
  ### Testing
  To test the boundary conditions the beam breaks more than eight times which should result in no further LEDs lighting up or overflow to occur.
  To test integration the main file code with the flex-pot code is tested to ensure the two modules operate without interfering (e.g. ADC vs EXTI).
  

  
</details>

<details>
  <summary>LDR and LCD display</summary>
  
  ### PTU module
  - briefly explain the challenge
  - how to connect to the board and set it up
  - buttons and controls available
  
</details>

<details>
  <summary>Servo Motor</summary>

  ### Summary


  ### Usage


  ### Valid Input


  ### Functions and Modularity


  ### Testing 


  ### Notes

</details>

Code from MTRX2700-2025 was used and refactored, and code from ThursAM_Group6_CLab was used in this assignment.
