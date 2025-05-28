#ifndef SERIAL_PORT_HEADER
#define SERIAL_PORT_HEADER

/* layout of file:
 *
 * - Definitions
 * - Initialise USARTs
 * - USART Specific Functions
 * - Initialise Interrupts
 * - MISC functionality
 * - PTU serialisation
 */

#include <stdint.h>
#include <stdio.h>

#include "stm32f303xc.h"

// ---------- DEFINITIONS ----------


// buffer size as 128 bytes.
#define BUFFER_SIZE 128
#define TERMINATION_CR '\r'
#define TERMINATION_LF '\n'


// Forward declaration of the SerialPort struct
typedef struct _SerialPort SerialPort;

// Callback function type definition
typedef void (*callback_t)(SerialPort *serial_port);

// External SerialPort instance
extern SerialPort USART1_PORT;

// Supported baud rate enumeration
enum {
  BAUD_9600,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_115200
};



// ---------- INITIALISATION ----------
 
void serialInitialise(uint32_t baudRate, SerialPort *serial_port, callback_t callback_function);


// ---------- USART SPECIFIC FUNCTIONS ----------

void receiveString(SerialPort *serial_port);

void transmitString(uint8_t *buffer, SerialPort *serial_port);


// ---------- INTERUPT INITIALISATION ----------

void serialReceiveInterrupt(SerialPort *serial_port);

void serialTransmitInterrupt(SerialPort *serial_port);

void USART1_IRQHandler(void);

void serialReceiveIRQHandler(SerialPort *serial_port);

void serialTransmitIRQHandler(SerialPort *serial_port);

// ---------- MISC ----------

void transmitStringInfo(SerialPort *serial_port);

void myRxCallback(SerialPort *serial_port);

const char *serialGetReceivedBuffer(SerialPort *serial_port);


// --------- PTU SERIALISATION FUNCTIONS ----------

void serialOutputBuffer(uint8_t *buffer, uint16_t buffer_length, SerialPort *serial_port);


// ---------- Serial Packet -----------

uint16_t serialInputPacketHeader(char *buffer, SerialPort *serial_port);

uint16_t serialInputDataPacket(char *buffer, int length, SerialPort *serial_port);


// ---------- Sending Data -------------

void sendSensorData(int16_t filtered_roll, int16_t filtered_pitch, int16_t filtered_yaw,
                      int16_t filtered_acc_x, int16_t filtered_acc_y, int16_t filtered_acc_z,
                      uint16_t filtered_lidar, SerialPort *serial_port);
#endif
