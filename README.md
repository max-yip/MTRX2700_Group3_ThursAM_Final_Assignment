# MTRX2700_Group3_ThursAM_Final_Assignment

## Group Members
Paul Akle 530483944 <br>
Max Yip 520434307 <br>
Siya Sharma 510451145 <br>
Shuvayu Banerjee 540752353 <br>

## Assignment Purpose
To design and implement proof-of-concept technology for a high-tech treasure hunt experience by incorporating a number of different
microcontroller subsystems, sensors, and an actuator.

## Roles and Responsibilities
Paul Akle <br>
Max Yip <br>
Siya Sharma <br>
Shuvayu Banerjee <br>


## Project Overview
<details>
  <summary>Challenge 1</summary>
  
  ### Triangulating the hidden planet location

  The player is tasked to locate the hidden planet by measuring the distances of the nearest 3 stars before the fuel runs out.
  
  ### How it works
  1. Users will be required to control the servos of the PTU through the GUI to adjust the lidar to point towards the correct planet.
  2. As the user moves the PTU, the GUI will display the distance and indicate whether the angle of the PTU is correct. The user will have to get the coordinates before moving onto the next planet.
  3. Once the player gets the distance or the timer runs out, the STM32 will signal the GUI to move onto the next planet.
  4. Once all the distances are obtained, it will reveal coordinates to the hidden planet.
  5. Move on to the next challenge: during the travel, the user encounters an asteroid field and is required to shoot them down.

  ### Flowchart
  *(Insert flowchart image here)*

  ### Additional Features
  - **Pop-up windows**  
    - At the beginning, to give the user the background of the game  
    - User feedback on whether the distances measured from the PTU are correct  
    - Final pop-up window directing the user to the next challenge  

  - **Chatbot**  
    - Friendly cat companion that gives hints to guide the user throughout the entire treasure hunt
  
</details>

<details>
  <summary>Challenge 2</summary>
  
  ### Asteroid Attack
  After locating the planet the player experiences an astroid field right before entering the planets atmosphere where 8 astroids must be sucessfully hit to make it to the planets surface.

  ### How it works
  1. The user will aim a laser beam and flick towards the astroid
  2. The IR beam breaker sensor will be placed on either side of the lasers path and will detect the object passing through
  3. Each time this occure one of the eight LEDs on the STM discovery board will light up untill all 8 astroids are destroyed
![Puzzle2-Paul drawio](https://github.com/user-attachments/assets/7ae7bd23-1bf2-461a-beda-083f33714b6a)


  
</details>
<details>
  <summary>Challenge 3</summary>
  
  ### Map Coordinates with Flexible Potentiometer
  After the user makes it to the surface of the planet they must now determine the correct coordinates of the crystal 

  ### How it works
  1. On a locating device there are two flexible potentiometers, one represents the x-ccordinate and the other represents the y-coordinates
  2. By applying pressure on different spots along the potentiometer will generate different values
  3. The interface will tell the user if they are closer or further from the true coordinates compared with their previous attempt
  4. After landing on the correct coordinates the display will tell the user to go to those coordinates on the grid
  5. A servo is intigrated with the determination of the correct coordinates, which when achieved will rotate 90 degrees to open the lid of the correct grid space
  6. An LDR inside this gridspace will detect the light and display a message on a LCD to retreive the crytal and place it in the hilt of the lightsaber

  ### Sersors + Actuators used
  1. 2x Flexible potentiometers
  2. Directional servo motor
  3. Light detecting resistor (LDR)
  4. Liquid-crystal Display (LCD)
  ![Puzzle3-Paul drawio](https://github.com/user-attachments/assets/e20776da-87a5-43a6-8d5b-a12ad4e82a18)

</details>

<details>
  <summary>Challenge 4</summary>

  ### Assembling the Lightsaber
  The final step in this treasure hunt is to verify that the crystal obtained from challenge 3 is the correct one.

  ### How it works
  1. User will insert the crystal into the hole inside the lightsaber hilt.
  2. The colour detection system will determine the colour of the crystal.
  3. When the user presses a button, the LED will either glow green (if a green crystal is inserted), red (if a red crystal is inserted), or remain off if no colour is detected.

</details>


## Modules
<details>
<summary>GUI interface</summary>

### Summary
This module serves as the **GUI control panel** for user interaction with the **Pan-Tilt Unit (PTU)** in Challenge 1. It provides both **control** and **feedback** for PTU servo positions via UART communication, and handles **game logic** such as planet progression, fuel consumption, and hints.

### File Structure
```
main.py                 # Entry point to launch GUI
ui/
  ├── main_window.py    # Top-level window layout
  ├── center_panel.py   # Central panel with coordinate logic
  ├── left_panel.py     # Planet info and sliders
  └── right_panel.py    # Chatbot and status indicators
core/
  ├── logic.py          # Game logic handling (planet state, validation)
  └── serial_handler.py # Serial communication with STM32
assets/
  └── images/           # UI image resources
```


### Usage
1. **Connect** STM32 board with PTU attached via USB.
2. Ensure PyQT6 installed, build version is using python3.12.1
2. In `main_window.py` (around line 31), **ensure the correct COM port** is specified for the STM32 board.
3. Run the GUI:
  ```bash
  python3 main.py
  ```

### Functionalities

- **Sliders**:  
  - Two sliders control **pan** and **tilt** angles of the servos.
  
- **Next Planet Button**:  
  - Advances to the next planet once current target is complete.

- **Calculate Coordinate Button**:  
  - Validates all 3 distances and PTU orientation for accuracy.

- **Fuel Bar**:  
  - Decreases if the player inputs incorrect distance/orientation.

- **Chatbot**:  
  - Offers hints during the treasure hunt when prompted by user.

### Valid Input

- Ensure **serial format** in `serial_handler.py` matches the STM32 firmware.
- Correct **COM port** is configured in the GUI.
- Servo positions (pan/tilt) must match pre-defined **target coordinates** (to be added).

### Functions and Modularity

- `main.py`  
  - Initializes and launches the main GUI window.

- `serial_handler.py`  
  - Handles UART communication with STM32.

- `logic.py`  
  - Contains game logic, position validation, planet progression.

- `main_window.py`  
  - Assembles all UI panels and manages layout.

- `left_panel.py`  
  - Contains sliders and planet visuals.

- `center_panel.py`  
  - Displays coordinates, fuel bar, and validation controls.

- `right_panel.py`  
  - Manages chatbot interaction and game messages.


### Testing

- **Slider Limitations**:  
  - Cannot set slider values by clicking—must be **manually dragged** to send values to STM32 correctly.

- **Chatbot Testing**:  
  - Ask for hints multiple times to confirm chatbot randomness and reliability.

- **Functional Testing**:  
  - Interact with all controls:
    - Adjust pan/tilt sliders
    - Validate coordinates
    - Use next planet button
    - Trigger fuel bar reduction
    - Request hints via chatbot
    - Ensure serial communication and game logic run smoothly

</details>

<details>
  <summary>PTU</summary>

### Summary
This module enables interfacing with the Pan-Tilt Unit through UART. It controls:
- 2 servos (pan, tilt)
- 1 LIDAR-Lite v3 distance sensor
- 3-axis gyroscope (L3G4200D)
- 3-axis accelerometer (ADXL345)
- 3-axis magnetometer (HMC5883L)

This is the main interface for Challenge 1 where users must orient servos to take accurate distance measurements.

---

### Sub-modules

#### Servo Drivers
**File**: `ptu_servo.h`

##### Usage
- `HAL_StatusTypeDef initialise_ptu_pwm(TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2)`  
  Initializes timers 1 & 2 for the two servos.

- `void setServoPWM(uint16_t vertical_PWM, uint16_t horizontal_PWM)`  
  Sets pan and tilt servo PWM values.

- `void servo_command_parser(SerialPort *serial_port)`  
  Debugging parser (no serialization) to directly set PWM via serial.

##### Functionalities and Features
- Servo changes are triggered by serial interrupts from the GUI.
- Both servos can be controlled simultaneously, but the GUI allows one at a time.

##### Testing
- Print statements on GUI confirm servo positioning.
- Command parser allows standalone testing (bypassing serialization).
- PWM values are clamped to slider bounds.

---

#### LIDAR
**File**: `ptu_lidar.h`

##### Usage
- `void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)`  
  Timer callback for PWM input (auto-triggered; do not call manually).

- `uint16_t last_period`  
  Stores the most recent measured distance in mm.

##### Functionalities and Features
- Distance is updated via interrupt on each pulse from LIDAR.
- Includes median filtering for noise reduction.
- Max reading capped at 4000 mm (4 meters).

##### Testing
- Test using objects at known distances (e.g. ruler).
- Validate units (mm) and output via GUI.
- Confirm proper serialisation to GUI.

---

#### Gyroscope and Accelerometer
**File**: `ptu_i2c.h`

##### Usage
- `void initialise_ptu_i2c(I2C_HandleTypeDef *i2c)`  
  Initializes I2C interface for sensors.

- `void read_gyro_data(I2C_HandleTypeDef *i2c, int16_t *yaw, int16_t *pitch, int16_t *roll)`  
  Reads gyroscope data.

- `void read_accel_data(I2C_HandleTypeDef *i2c, int16_t *acc_x, int16_t *acc_y, int16_t *acc_z)`  
  Reads accelerometer data.

##### Notes
- Currently unused, but intended for future Kalman filtering to estimate tilt.
- Will replace GUI dials with real-world orientation estimates.

##### Testing
- Output raw data via serial for verification.
- Connect to GUI and confirm readable, filtered data.
- Filter test: output should be ~0 if signal frequency < half window size.

---

#### Helper Modules

**`filters.h`**
- `void initFilters(Filter *filters, uint16_t init_value)`  
  Initializes sliding window filters.

- `uint16_t getMedian(Filter* filter, uint16_t new_value)`  
  Computes median-filtered output.

- `uint16_t getMovingAverage(Filter* filter, uint16_t new_value)`  
  Computes moving average output.

**`ptu_definitions.h`**
- Contains all sensor register mappings and I2C constants.

**`serial.h and serialisation.h`**
- Packs all sensor data and sends it to the GUI.
- Uses sentinel bytes and headers for reliable parsing.

</details>


<details>
  <summary>Flex pot sensor</summary>
  
### Flexible Potentiometer (flexpot.c)
### Summary
This module configures ADC1 on PC0 (ADC_IN6) and PC2 (ADC_IN8) to read input from two flexible potentiometers. It provides a simple API to initialize the ADC hardware and perform one-shot conversions on a specified channel, returning a discrete position value (1–6) or 0 to indicate no touch.

### Usage
1. Initialization
Call:
`FlexPot_Init();`

This:
  - Enables clocks for GPIOC and ADC1
  - Configures PC0 and PC2 in analog mode
  - Calibrates and enables ADC1

2. Reading a Position
To read from a specific potentiometer channel:
`uint8_t pos = FlexPot_GetPosition(channel);`
Where:
  - `channel` is `6` for PC0 or `8` for PC2
  - Return value pos is:
    - `0` if no touch (`raw < 100`)
    - `1–6` depending on pressure/location along the strip

### Valid Input

- channel must be:
    - `6` → PC0 → ADC_IN6
    - `8` → PC2 → ADC_IN8

- Internally:
    - Raw ADC values < 100 are considered no touch and return 0
    - Values ≥ 100 are linearly mapped to positions 1–6
      (1 = bottom, 6 = top of the strip)

### Functions and Modularity

- `void FlexPot_Init(void)`
 Initializes GPIOC and ADC1:
    - Enables clocks
    - Sets PC0 and PC2 to analog mode
    - Calibrates and powers on ADC1

- `uint8_t FlexPot_GetPosition(uint8_t channel)`
Reads the flexible potentiometer:
  - Sets the selected ADC channel (6 or 8)
  - Starts and waits for ADC conversion
  - Reads `ADC1->DR`
  - Applies a "no-touch" threshold
  - Maps 12-bit ADC value (0–4095) to discrete values `0–6`


### Testing

- No Touch Test:
Leave the potentiometer untouched → should return 0

- Endpoint Test:
Press firmly at the start or end of the strip → should return 1 or 6

- Integration Test:
In your puzzle game, read X/Y positions using:


`uint8_t x = FlexPot_GetPosition(6);
uint8_t y = FlexPot_GetPosition(8);`

Send both values over UART periodically and confirm the output matches physical input location.


  
</details>

<details>
  <summary>Break Beam Sensor </summary>
  
### IR Beam Breaker Sensor (beam.c)
  
### Summary
This module configures PC1 as an EXTI1 (falling‐edge) interrupt input to detect when an IR beam is broken. Each beam break increments a counter (`beam_count`, up to 8) and lights the corresponding Discovery-board LED (PE8…PE15). A simple API lets you query or reset the count (and LEDs).
### Usage
1. Initialization
At the start of your program, call `Beam_Init();`
This:
- Enables GPIOC and GPIOE clocks
- Configures PC1 as an EXTI1 input with falling-edge interrupt
- Enables EXTI1 interrupt in NVIC
- Sets up PE8–PE15 as outputs for LED indication

2. Reading the Count
In your application code, use:
`uint8_t n = Beam_GetCount();`
This returns the number of beam breaks detected so far (max 8).

2. Resetting the Count and LEDs
To reset the counter and turn off LEDs:
`Beam_ResetCount();`

### Valid Input
- An object must break the IR beam, pulling PC1 low and triggering a falling edge.
- Only transitions from high to low increment `beam_count` (debounced via hardware or design).
- The counter saturates at 8; additional breaks do not increase the count or light further LEDs.
- No input (i.e., beam never broken) is treated as a failure condition in the game.


### Functions and Modularity
- `void Beam_Init(void)`
Initializes GPIOC and GPIOE. Configures PC1 for EXTI1 (falling edge) and sets PE8–PE15 as LED outputs.
- `uint8_t Beam_GetCount(void)`
Returns the current value of beam_count.
- `void Beam_ResetCount(void)`
Resets beam_count to 0 and turns off all LEDs.
- `void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)`
Called automatically by HAL on EXTI interrupts. If the pin is PC1 and it reads low:
  - Increments `beam_count` (if < 8)
  - Lights the corresponding LED (PE8 + count index)


### Testing
To test the boundary conditions the beam breaks more than eight times which should result in no further LEDs lighting up or overflow to occur.
To test integration the main file code with the flex-pot code is tested to ensure the two modules operate without interfering (e.g. ADC vs EXTI).
1. Boundary Test:
Trigger the beam more than 8 times. The count should stop incrementing and no extra LEDs should light.

2. Integration Test:
Run the beam module alongside other sensor modules (e.g., Flex Potentiometer using ADC). Verify:
    - EXTI and ADC operate independently.
    - No conflicts or race conditions between modules.



  
</details>

<details>
  <summary>LCD display</summary>
  
  ### Summary
  
  This module provides an interface for controlling a 16x2 HD44780-compatible LCD over I2C using STM32 HAL libraries. It uses an I2C I/O expander (e.g., PCF8574) to communicate with the LCD in 4-bit mode. This reduces GPIO usage and simplifies wiring, ideal for STM32CubeMX-based projects.

The module allows for easy initialization, cursor control, string display, and command-level manipulation. 

The LCD is connected to GPIO port D using the following pin mapping:
- GND --> GND
- VDD --> 5V
- SCL --PA6
- SDA --> PA7

The module is designed for portability and ease of integration with STM32CubeMX-based projects.

  ### Usage

#### 1. Initialize GPIO in STM32CubeMX

Enable GPIOD and configure PD8–PD14 as output push-pull with no pull-up/pull-down.

#### 2. Include the module
In main.c or any relevant source file:
`#include "lcd.h"`


#### 3. Include the module
Initialize LCD in your main function
`lcd_init();
lcd_clear();
lcd_print("Place cystal on handle!");`


  ### Valid Input
1. Rows: 0 or 1
2. Columns: 0 to 15
3. Characters: Standard ASCII text (the LCD does not support Unicode)
4. Strings: Null-terminated (char *), max 16 characters per line (extra characters wrap if not managed)

  ### Functions 

- `void lcd_init(void)`
Initializes the LCD in 4-bit mode.

- `void lcd_clear(void)`
Clears the LCD display.

- `void lcd_set_cursor(uint8_t row, uint8_t col)`
Sets the cursor to the specified row and column.

- `void lcd_print(char *str)`
Prints a null-terminated string to the LCD.

- `void lcd_cmd(uint8_t cmd)`
Sends a raw command byte directly to the LCD controller.

- `void lcd_data(uint8_t data)`
Sends a raw data byte (ASCII character) to be displayed.




  ### Modularity
All functions are isolated and well-documented in `lcd.c` and `lcd.h`. The module does not depend on other parts of the project and uses only HAL GPIO functions.

  ### Testing 
1. Confirm the LCD initializes and displays static text using lcd_print().
2. Test cursor positioning using lcd_set_cursor(row, col).

3. Clear the display and update with new values dynamically.

#### Debug Tips
1. Check contrast voltage on V0 (usually via potentiometer).
2. Ensure RW is grounded for write-only mode or correctly toggled.
3. Use an oscilloscope or logic analyzer to confirm Enable (EN) pulse timing if LCD does not respond.


</details>
<details>
  <summary>LDR</summary>

### Summary
This module uses an LDR (Light Dependent Resistor) for two purposes:
1. **Light Detection**  
   - Measures ambient brightness.
   - Compares against a preset threshold to trigger logic changes.

2. **Colour Detection**  
   - Detects the amount of reflected light when red, green, and blue LEDs are sequentially illuminated.
   - Determines the dominant reflected wavelength to identify the surface colour.

---

### Sub-modules

#### Colour Detection State Machine  
**File**: `colour_detector.h`

##### Description
A state machine implemented via `switch-case` inside a timer callback:
- **States**:  
  `INIT → RED_ON → READ_RED → GREEN_ON → READ_GREEN → BLUE_ON → READ_BLUE → DETECT_COLOUR`

- **Calibration**:  
  On first run (`calibrate = 0`), records initial ambient brightness to normalize readings.

- **Debugging**:  
  When colour is detected, an optional final state transmits a string over serial for monitoring.

##### Usage
- Timer interrupt calls the `handleStateMachine()` function at a fixed interval.
- LED GPIOs are toggled within each state to control colour sequencing.

---

#### ADC Functions  
**File**: `adc.c`

##### Core Functions
- `void initialiseSingleADC()`  
  Manually configures ADC1 to single-shot mode.

- `uint16_t singleReadADC()`  
  Performs a one-time read of the ADC value, used in the colour detection state machine.

##### Optional Utilities
- `void continuousReadSingleChannelADC()`  
  Continuously polls a single ADC channel in a blocking loop using the `ISR` register.

- `void SingleReadMultiChannelADC()`  
  Reads multiple channels on the same ADC (currently unused in this module but available for expansion).

---

### Functionalities and Features
- Ambient light detection with threshold-triggered variable setting.
- RGB LED-controlled surface reflection sensing to classify colours.
- Compact state machine for cyclic LED control and ADC reading.
- Lightweight implementation using timer-driven ADC sampling.

---

### Usage

```c
int main(void) {
    enableGPIODLEDS();
    enableGPIOClocks();

    serialInitialise(BAUD_115200, &USART1_PORT, 0x00);

    initialiseSingleADC();

    // Start colour detection state machine on timer callback
    setTIM2Callback(&handleStateMachine);
    init_TIM2(50); // Timer interval set to 50ms
}
```

---

### Testing
- Use serial output to confirm detection states and transitions.
- Calibrate under various lighting conditions to test ambient compensation.
- Use known-colour surfaces and compare detection accuracy.
- Evaluate ADC signal stability under continuous and single-shot reads.

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


Code from **MTRX2700-2025** was used and refactored, and code from **ThursAM_Group6_CLab** was used in this assignment.
