#ifndef SERIALISE_HEADER
#define SERIALISE_HEADER

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Constants
#define SENTINEL_1 0xAA
#define SENTINEL_2 0x55

// Enum for message types (only active types retained)
typedef enum {
    SENSOR_DATA = 0,
    STRING_PACKET = 1,
    SERVO_PWM = 2
} MessageType;


// Sensor data struct
typedef struct {
    int32_t acc_x;
    int32_t acc_y;
    int32_t acc_z;
    int32_t gyro_x;
    int32_t gyro_y;
    int32_t gyro_z;
    uint32_t lidar_pwm;
} SensorData;


// Servo data struct
typedef struct {
    uint16_t pwm1;
    uint16_t pwm2;
} ServoData;


// Variable length string packet struct
typedef struct {
    uint8_t length;
    char *data;
} StringPacket;


// Union of data types (only remaining types)
typedef union {
    SensorData sensor_data;
    StringPacket string_packet;
    ServoData servo_data;
} Data;


// Message header structure
typedef struct {
    uint8_t sentinel1;
    uint8_t sentinel2;
    uint16_t message_type;
    uint16_t data_length;
} Header;


// Function to pack data into a buffer for transmission
uint16_t pack_buffer(uint8_t *buffer, MessageType message_type, Data *data);
// Function to unpack the buffer and check for sentinel bytes
bool unpack_buffer(const uint8_t *buffer, Data *output_data, MessageType *output_message_type, uint16_t *output_data_length);


#endif
