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
  
  ### Summary
  This module configures **ADC1** on `PC0 (IN6)` and `PC2 (IN8)` to read two flexible potentiometers. It provides an API to initialize the ADC hardware and perform one-shot conversions on a given channel, mapping the 12-bit raw value into a discrete **position** from **1 to 6**, or **0** for no touch.

  ### Usage
  - Call `FlexPot_Init();` to:
  - Enable clocks
  - Calibrate the ADC
  - Initialize flexible potentiometers

  - To get a reading:
    ```c
    uint8_t pos = FlexPot_GetPosition(channel);
    ```
    where `channel` is `6` for `PC0` or `8` for `PC2`.  
    The return value is:
    - `0` if raw ADC < `NO_TOUCH_THRESHOLD`
    - `1` to `6` for valid touches (mapped linearly)


  ### Valid Input
  - Valid `channel` values: `6 (PC0)` or `8 (PC2)`
  - Raw ADC values:
    - `< 100` → interpreted as "no-touch" (`0`)
    - `≥ 100` → mapped to positions `1–6` (base to top)
  
  ### Functions and Modularity
  - `void FlexPot_Init(void)`  
  - Enables GPIOC & ADC12 clocks  
  - Sets PC0 and PC2 to analog mode  
  - Powers up, calibrates, and enables ADC1

  - `uint8_t FlexPot_GetPosition(uint8_t channel)`  
  - Selects the channel in `SQR1`  
  - Starts ADC conversion and waits for completion  
  - Reads `ADC1->DR`, applies thresholding, maps to 0–6

  ### Testing
  - **Edge Case**: Leave potentiometer untouched → should return `0`
  - **Endpoint Test**: Touch potentiometer at base/top → expect `1` or `6`
  - **Integration Test**: Print X/Y positions over UART at regular intervals to verify correctness with physical input

  
</details>

<details>
  <summary>Break Beam Sensor </summary>
  
  ### Summary
  This module configures `PC1` as an **EXTI1 (falling-edge)** interrupt input to detect when an IR beam is broken. Each beam break:
  - Increments a counter (`beam_count`, max 8)
  - Lights corresponding LED (`PE8` to `PE15`)

  A simple API is provided to:
  - Query the count
  - Reset the count and LEDs

  ### Usage
  - Call `Beam_Init();` to:
    - Enable GPIOC/E clocks
    - Configure `PC1` for EXTI (falling-edge)
    - Enable EXTI1 interrupt

  - In your puzzle application:
    ```c
    uint8_t n = Beam_GetCount();
    ```
  ### Valid Input
  - Beam must be **interrupted (falling-edge)** at `PC1`
  - `beam_count`:
    - Increments up to **8**
    - Saturates (no overflow beyond 8)
  - If **no input** is detected, the player **loses** the game
  
  ### Functions and Modularity
  - `void Beam_Init(void)`  
    - Configures `PC1` for EXTI1 interrupt  
    - Enables GPIOE for LED control (`PE8` to `PE15`)

  - `uint8_t Beam_GetCount(void)`  
    - Returns the current `beam_count`

  - `void Beam_ResetCount(void)`  
    - Clears `beam_count`  
    - Turns off all LEDs

  - `void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)`  
    - Called on EXTI event  
    - If `GPIO_Pin == IR_PIN` and pin is `LOW`:
      - Increments `beam_count` (up to 8)
      - Lights corresponding LED

  
  ### Testing
  - **Overflow Test**: Break the beam >8 times → no overflow or additional LEDs should light
  - **Integration Test**: Confirm beam module works in parallel with flex-pot module (e.g., ADC and EXTI don’t interfere)


  
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


Code from **MTRX2700-2025** was used and refactored, and code from **ThursAM_Group6_CLab** was used in this assignment.