#include "serial.h"

#include "stm32f303xc.h"

/*
Internal structure capturing all register pointers and GPIO configuration
needed for a given UART port instance.
*/
struct _SerialPort {
	volatile uint32_t *BaudRate;
	volatile uint32_t *ControlRegister1;
	volatile uint32_t *StatusRegister;
	volatile uint16_t *DataOutputRegister;
	volatile uint32_t *TimerEnableRegister;
	volatile uint32_t TimerEnableMask;
	volatile uint32_t SerialPortGPIO;
	volatile uint32_t *SerialPinModeRegister;
	volatile uint32_t SerialPinModeValue;
	volatile uint32_t *SerialPinSpeedRegister;
	volatile uint32_t SerialPinSpeedValue;
	volatile uint8_t *SerialPinAlternatePinRegister;
	volatile uint8_t SerialPinAlternatePinValue;
	void (*completion_function)(uint32_t);
};


// The different serial ports require different GPIO ports
enum {
	SERIAL_GPIO_A,
	SERIAL_GPIO_B,
	SERIAL_GPIO_C,
	SERIAL_GPIO_D,
	SERIAL_GPIO_E
};



// instantiate the serial port parameters

SerialPort USART1_PORT = {&(USART1->BRR),
		&(USART1->CR1),
		&(USART1->ISR),
		&(USART1->TDR),
		&(RCC->APB2ENR),
		RCC_APB2ENR_USART1EN,
		SERIAL_GPIO_C,
		&(GPIOC->MODER),
		0xA00,
		&(GPIOC->OSPEEDR),
		0xF00,
		((uint8_t*)&(GPIOC->AFR[0])) + 2,
		0x77};


// InitialiseSerial - Initialise the serial port
// Input: baudRate is from an enumerated set
void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t)) {

	serial_port->completion_function = completion_function;

	// enable clock power, system configuration clock and GPIOC
	// common to all UARTs
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	switch(serial_port->SerialPortGPIO) {
	case SERIAL_GPIO_C:
		RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
		break;
	default:
		break;
	}

	// set pin mode
	*(serial_port->SerialPinModeRegister) = serial_port->SerialPinModeValue;

	// enable high speed clock for GPIOC
	*(serial_port->SerialPinSpeedRegister) = serial_port->SerialPinSpeedValue;

	// set alternate function to enable USART to an external pin
	*(serial_port->SerialPinAlternatePinRegister) = serial_port->SerialPinAlternatePinValue;

	*(serial_port->TimerEnableRegister) |= serial_port->TimerEnableMask;

	uint16_t *baud_rate_config = ((uint16_t*)serial_port->BaudRate); // only 16 bits used!

	// Baud rate calculation from datasheet
	switch(baudRate){
	case BAUD_9600:
		*baud_rate_config = 0x341*0x06;  // 9600 at 48MHz
		break;
	case BAUD_19200:
		*baud_rate_config = 0x1A1*0x06;  // 19200 at 48MHz
		break;
	case BAUD_38400:
		*baud_rate_config = 0xD0*0x06;  // 38400 at 48MHz
		break;
	case BAUD_57600:
		*baud_rate_config = 0x8B*0x06;  // 57600 at 48MHz
		break;
	case BAUD_115200:
		*baud_rate_config = 0x46*0x06;  // 115200 at 48MHz
		break;
	}

	// enable serial port for tx and rx
	*(serial_port->ControlRegister1) |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

/*
Transmit a single byte, blocking until the TXE flag is set.
data - Byte to transmit.
serial_port - SerialPort instance to use.
*/
void SerialOutputChar(uint8_t data, SerialPort *serial_port) {
    // Wait until Data Register Empty
	while((*(serial_port->StatusRegister) & USART_ISR_TXE) == 0){
	}

	*(serial_port->DataOutputRegister) = data;      // Write data to TDR
}


/*
Transmit a null-terminated string, then invoke callback if set.
pt - Pointer to a C-string.
serial_port - SerialPort instance to use.
*/
void SerialOutputString(uint8_t *pt, SerialPort *serial_port) {

	uint32_t counter = 0;
	while(*pt) {
		SerialOutputChar(*pt, serial_port);
		counter++;
		pt++;
	}
    // Call completion callback if provided
	if (serial_port->completion_function != 0x00)
		serial_port->completion_function(counter);
}

