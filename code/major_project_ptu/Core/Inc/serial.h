#ifndef SERIAL_PORT_HEADER
#define SERIAL_PORT_HEADER

/* layout of file:
 *
 * - Definitions
 * - Initialise USARTs
 * - Initialise USART functions
 * 		- Specific register set ups
 * - USART Specific Functions
 * - Initialise Interrupts
 * - Initialise Interrupt Handler
 * - MISC functionality
 * - Major Initialise
 */

#include <stdint.h>
#include <stdio.h>

#include "stm32f303xc.h"

// ---------- DEFINITIONS ----------


// buffer size as 128 bytes.
#define BUFFER_SIZE 128
#define TERMINATION_CR '\r'
#define TERMINATION_LF '\n'


//struct _SerialPort;
//typedef struct _SerialPort SerialPort;
//extern SerialPort USART1_PORT;

typedef struct _SerialPort SerialPort;

typedef void (*callback_t)(SerialPort *serial_port);

extern SerialPort USART1_PORT;

enum {
  BAUD_9600,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_115200
};




// ---------- INITIALISATION ----------
 
void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, callback_t callback_function);


// ---------- USART SPECIFIC FUNCTIONS ----------

void receiveString(SerialPort *serial_port);

void transmitString(uint8_t *buffer, SerialPort *serial_port);


// ---------- INTERUPT INITIALISATION ----------

void serialReceiveInterrupt(SerialPort *serial_port);

void serialTransmitInterrupt(SerialPort *serial_port);

// ---------- INTERUPT HANDLER ----------

void USART1_EXTI25_IRQHandler(void);

// Interrupt Helpers

void SerialReceiveIRQHandler(SerialPort *serial_port);

void serialTransmitIRQHandler(SerialPort *serial_port);

// ---------- MISC ----------

void transmitStringInfo(SerialPort *serial_port);

void my_rx_callback(SerialPort *serial_port);

const char *SerialGetReceivedBuffer(SerialPort *serial_port);



void SerialOutputChar(uint8_t data, SerialPort *serial_port);

void SerialOutputBuffer(uint8_t *buffer, uint16_t buffer_length, SerialPort *serial_port);

uint8_t SerialReceiveChar(SerialPort *serial_port, uint8_t *received_char);
// ---------- Serial Packet -----------

uint16_t SerialInputPacketHeader(char *buffer, SerialPort *serial_port);

uint16_t SerialInputDataPacket(char *buffer, int length, SerialPort *serial_port);

#endif
