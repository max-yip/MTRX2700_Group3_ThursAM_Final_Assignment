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
  
  ### Asteroid Shooting
  After locating the planet the player experiences an astroid field right before entering the planets atmosphere where 8 astroids must be sucessfully hit to make it to the planets surface.

  ### How it works
  1. The user will aim a laser beam and flick towards the astroid
  2. The IR beam breaker sensor will be placed on either side of the lasers path and will detect the object passing through
  3. Each time this occure one of the eight LEDs on the STM discovery board will light up untill all 8 astroids are destroyed


  
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
  
### IR Beam Breaker Sensor (beam.c)
  
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
  <summary>LCD display</summary>
  
  ### PTU module
  - briefly explain the challenge
  - how to connect to the board and set it up
  - buttons and controls available
  
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
