//#include "serial.h"
//
//
//// NOTE: these are stored as pointers because they
////       are const values so we can't store them directly
////       in the struct
//struct _SerialPort {
//	volatile uint32_t *BaudRate;
//	volatile uint32_t *ControlRegister1;
//	volatile uint32_t *FlagClearRegister;
//	volatile uint32_t *StatusRegister;
//	volatile uint16_t *DataOutputRegister;
//	volatile uint16_t *DataInputRegister;
//	volatile uint32_t *TimerEnableRegister;
//	volatile uint32_t TimerEnableMask;
//	volatile uint32_t SerialPortGPIO;
//	volatile uint32_t *SerialPinModeRegister;
//	volatile uint32_t SerialPinModeValue;
//	volatile uint32_t *SerialPinSpeedRegister;
//	volatile uint32_t SerialPinSpeedValue;
//	volatile uint8_t *SerialPinAlternatePinRegister;
//	volatile uint8_t SerialPinAlternatePinValue;
//	void (*completion_function)(uint32_t);
//
//	uint8_t buffer[2][BUFFER_SIZE];
//	volatile uint8_t buf_len[2];
//	volatile uint8_t receiving;
//
//	volatile uint8_t tx_idx;
//};
//
//
//// The different serial ports require different GPIO ports
//enum {
//	SERIAL_GPIO_A,
//	SERIAL_GPIO_B,
//	SERIAL_GPIO_C,
//	SERIAL_GPIO_D,
//	SERIAL_GPIO_E
//};
//
//
//
//// instantiate the serial port parameters
////   note: the complexity is hidden in the c file
//SerialPort USART1_PORT = {
//		&(USART1->BRR),
//		&(USART1->CR1),
//		&(USART1->ICR),
//		&(USART1->ISR),
//		&(USART1->TDR),
//		&(USART1->RDR),
//		&(RCC->APB2ENR),
//		RCC_APB2ENR_USART1EN,
//		SERIAL_GPIO_C,
//		&(GPIOC->MODER),
//		0xA00,
//		&(GPIOC->OSPEEDR),
//		0xF00,
//		((uint8_t*)&(GPIOC->AFR[0])) + 2,
//		0x77,
//
//		{ { 0 }, { 0 } },
//		{ 0, 0 },
//		0
//
//};
//
//
//// InitialiseSerial - Initialise the serial port
//// Input: baudRate is from an enumerated set
//void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t)) {
//
//	serial_port->completion_function = completion_function;
//
//	// enable clock power, system configuration clock and GPIOC
//	// common to all UARTs
//	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
//	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
//
//	switch(serial_port->SerialPortGPIO) {
//	case SERIAL_GPIO_C:
//		RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
//		break;
//	default:
//		break;
//	}
//
//	// set pin mode
//	*(serial_port->SerialPinModeRegister) = serial_port->SerialPinModeValue;
//
//	// enable high speed clock for GPIOC
//	*(serial_port->SerialPinSpeedRegister) = serial_port->SerialPinSpeedValue;
//
//	// set alternate function to enable USART to an external pin
//	*(serial_port->SerialPinAlternatePinRegister) = serial_port->SerialPinAlternatePinValue;
//
//	*(serial_port->TimerEnableRegister) |= serial_port->TimerEnableMask;
//
//	uint16_t *baud_rate_config = ((uint16_t*)serial_port->BaudRate); // only 16 bits used!
//
//	// Baud rate calculation from datasheet
//	switch(baudRate){
//	case BAUD_9600:
//		// NEED TO FIX THIS !
//		*baud_rate_config = 0x341 * 0x06;  // 9600 at 48MHz
//		break;
//	case BAUD_19200:
//		// NEED TO FIX THIS !
//		*baud_rate_config = 0x1A1 * 0x06;  // 19200 at 8MHz
//		break;
//	case BAUD_38400:
//		// NEED TO FIX THIS !
//		*baud_rate_config = 0xD0 * 0x06;  // 38400 at 8MHz
//		break;
//	case BAUD_57600:
//		// NEED TO FIX THIS !
//		*baud_rate_config = 0x8B * 0x06;  // 57600 at 8MHz
//		break;
//	case BAUD_115200:
//		*baud_rate_config = 0x46 * 0x06;  // 115200 at 8MHz
//		break;
//	}
//
//
//	// enable serial port for tx and rx
//	*(serial_port->ControlRegister1) |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
//
//	serial_port->tx_idx = 0;
//	serial_port->buf_len[0] = 0;
//	serial_port->buf_len[1] = 0;
//	serial_port->receiving = 0;
//}
//
//
//// ---------- INTERRUPT INITIALISATION ----------
//
//// SerialEnableInterrupt - initialising interrupt for the given UART line and initialise the callback function
//// inputs: serial port pointer, callback function pointer
//void serialReceiveInterrupt(SerialPort *serial_port){
//	__disable_irq();
//
//	// Enable RXNE interrupt
//	serial_port->UART -> CR1 |= USART_CR1_RXNEIE;
//
//	// Enable NVIC interrupt for USART1
//	if (serial_port->UART == USART1){
//		// Tell the NVIC module that USART1 interrupts should be handled
//		NVIC_SetPriority(USART1_IRQn, USART1PRIORITY);  // Set Priority
//		NVIC_EnableIRQ(USART1_IRQn);
//	}
//
//	__enable_irq();
//}
//
//
//
//void serialTransmitInterrupt(SerialPort *serial_port) {
//	__disable_irq();
//
//	serial_port->UART->CR1 |= USART_CR1_TXEIE;
//
//	// Enable NVIC interrupt for USART1
//	if (serial_port->UART == USART1){
//		// Tell the NVIC module that USART1 interrupts should be handled
//		NVIC_SetPriority(USART1_IRQn, USART1PRIORITY);  // Set Priority
//		NVIC_EnableIRQ(USART1_IRQn);
//	}
//
//	__enable_irq();
//}
//
//
//// ---------- INTERRUPT HANDLER ----------
//
//
//// USART1_EXTI25_IRQHandler - interrupt handler function that calls the customized SerialReceiveIRQHandler function for the USART1 port
//// DO NOT CHANGE THE FUNCTION NAME
//// inputs: none
//void USART1_EXTI25_IRQHandler(void)
//{
//    SerialPort *serial_port = &USART1_PORT;
//
//    // -------- transmit path: TXE
//
//    if ((serial_port->UART->ISR & USART_ISR_TXE) && (serial_port->UART->CR1 & USART_CR1_TXEIE)) {
//    	serialTransmitIRQHandler(serial_port);
//    }
//
//    // -------- transmit path: TC
//
//    if ((serial_port->UART->ISR & USART_ISR_TC) && (serial_port->UART->CR1 & USART_CR1_TCIE)) {
//    	serial_port->UART->CR1 &= ~USART_CR1_TCIE;
//        // NOTHING TO BE DONE
//    }
//
//    // -------- receive path: RXNE
//
//    if ((serial_port->UART->ISR & USART_ISR_RXNE) && (serial_port->UART->CR1 & USART_CR1_RXNEIE)) {
//    	SerialReceiveIRQHandler(serial_port);
//    }
//}
//
//
//
//void SerialOutputChar(uint8_t data, SerialPort *serial_port) {
//	while((*(serial_port->StatusRegister) & USART_ISR_TXE) == 0){
//	}
//
//	*(serial_port->DataOutputRegister) = data;
//}
//
//
//
//void SerialOutputString(uint8_t *pt, SerialPort *serial_port) {
//
//	uint32_t counter = 0;
//	while(*pt) {
//		SerialOutputChar(*pt, serial_port);
//		counter++;
//		pt++;
//	}
//
//	if (serial_port->completion_function != 0x00)
//		serial_port->completion_function(counter);
//}
//
//
//
//// returns 1 if valid char, 0 if timeout
//uint8_t SerialReceiveChar(SerialPort *serial_port, uint8_t *received_char)
//{
//	uint8_t latest_character = 0;
//
//	uint16_t timeout = 0xffff;
//  while (1) {
//	  timeout--;
//	  if (timeout == 0)
//		  return 0;
//
//	if (*(serial_port->StatusRegister) & USART_ISR_ORE || *(serial_port->StatusRegister) & USART_ISR_FE) {
//		*(serial_port->FlagClearRegister) |= USART_ICR_ORECF | USART_ICR_FECF;
//	}
//
//	if (*(serial_port->StatusRegister) & USART_ISR_RXNE) { // Wait for RXNE flag to be set
//		break;
//	}
//  }
//  *received_char = *(serial_port->DataInputRegister);
//  return 1;
//}
//
//
//
//void SerialOutputBuffer(uint8_t *buffer, uint16_t buffer_length, SerialPort *serial_port) {
//	uint32_t counter = 0;
//
//	while(counter <= buffer_length) {
//		SerialOutputChar(*buffer, serial_port);
//		counter++;
//		buffer++;
//	}
//
//	// This code was added as the python reading program was not picking
//	//  up the next sentinel properly. This should be removed when the
//	//  python code is fixed.
//	uint8_t complete_buffer = '\0';
//	SerialOutputChar(&complete_buffer, serial_port);
//
////	if (serial_port->completion_function != 0x00)
////		serial_port->completion_function(counter);
//}

#include "serial.h"

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

// ---------- DEFINITIONS ----------

#define USART1PRIORITY 3

// We store the pointers to the GPIO and USART that are used
//  for a specific serial port. To add another serial port
//  you need to select the appropriate values.
struct _SerialPort {
	USART_TypeDef *UART;
	GPIO_TypeDef *GPIO;
	volatile uint32_t MaskAPB2ENR;
	volatile uint32_t MaskAPB1ENR;
	volatile uint32_t MaskAHBENR;
	volatile uint32_t SerialPinModeValue;
	volatile uint32_t SerialPinSpeedValue;
	volatile uint32_t SerialPinAlternatePinValueLow;
	volatile uint32_t SerialPinAlternatePinValueHigh;

	callback_t callback_function;

	uint8_t buffer[2][BUFFER_SIZE];
	volatile uint8_t buf_len[2];
	volatile uint8_t receiving;

	volatile uint8_t tx_idx;
};


// instantiate the serial port parameters
SerialPort USART1_PORT = {
		USART1,
		GPIOC,
		RCC_APB2ENR_USART1EN,
		0x00,
		RCC_AHBENR_GPIOCEN,
		0xA00,
		0xF00,
		0x770000,
		0x00,
		0x00,
		{ { 0 }, { 0 } },
		{ 0, 0 },
		0
};


// ---------- INITIALISATIONS ----------

void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, callback_t callback) {

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	RCC->AHBENR |= serial_port->MaskAHBENR;
	serial_port->GPIO->MODER = serial_port->SerialPinModeValue;
	serial_port->GPIO->OSPEEDR = serial_port->SerialPinSpeedValue;
	serial_port->GPIO->AFR[0] |= serial_port->SerialPinAlternatePinValueLow;
	serial_port->GPIO->AFR[1] |= serial_port->SerialPinAlternatePinValueHigh;
	RCC->APB1ENR |= serial_port->MaskAPB1ENR;
	RCC->APB2ENR |= serial_port->MaskAPB2ENR;

	// Get a pointer to the 16 bits of the BRR register that we want to change
	uint16_t *baud_rate_config = (uint16_t*)&serial_port->UART->BRR; // only 16 bits used!

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

	serial_port->callback_function = callback;

	serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

	serial_port->tx_idx = 0;
	serial_port->buf_len[0] = 0;
	serial_port->buf_len[1] = 0;
	serial_port->receiving = 0;
}

// ---------- UASRT SPECIFIC FUNCTIONALITY ----------

// RECEIVE STRING

// receiveString - receive input string with termination character '\r' or '\n' from the serial port and save to buffer
// Input: serial port
void receiveString(SerialPort *serial_port) {
    uint16_t i = 0;  // Buffer index

    while (1) {
        // Wait for RXNE flag (data available)
        while ((serial_port->UART->ISR & USART_ISR_RXNE) == 0){ }

        uint8_t received_char = serial_port->UART->RDR;

        if ((received_char == TERMINATION_CR)||(received_char == TERMINATION_LF)) {
            break;
        }

        serial_port->buffer[serial_port->receiving][i] = received_char;
        i++;

        // Prevent buffer overflow
        if (i >= BUFFER_SIZE - 1) {
            break;
        }
    }

    // Null-terminate the string
    serial_port->buffer[serial_port->receiving][i] = '\0';
    serial_port->buf_len[serial_port->receiving] = i;

    if (serial_port->callback_function) {
    	serial_port->callback_function(serial_port);
    }
}


// TRANSMIT STRING

// transmitString - output a NULL TERMINATED string to the serial port
// Input: pointer to a NULL-TERMINATED string (if not null terminated, there will be problems)
void transmitString(uint8_t *buffer, SerialPort *serial_port) {
	// while the buffer is not empty
	while(*buffer) {
		// print out the character of the buffer pointer to the serial port
		while((serial_port->UART->ISR & USART_ISR_TXE) == 0) { }

		serial_port->UART->TDR = *buffer;
		buffer++;
	}
}


// ---------- INTERRUPT INITIALISATION ----------


// SerialReceiveInterrupt - initialising receive interrupt for the given UART line
// inputs: serial port pointer
void serialReceiveInterrupt(SerialPort *serial_port){
	__disable_irq();

	// Enable RXNE interrupt
	serial_port->UART -> CR1 |= USART_CR1_RXNEIE;

	// Enable NVIC interrupt for USART1
	if (serial_port->UART == USART1){
		// Tell the NVIC module that USART1 interrupts should be handled
		NVIC_SetPriority(USART1_IRQn, USART1PRIORITY);  // Set Priority
		NVIC_EnableIRQ(USART1_IRQn);
	}

	__enable_irq();
}


// SerialTransmitInterrupt - initialising transmit interrupt for the given UART line
// inputs: serial port pointer
void serialTransmitInterrupt(SerialPort *serial_port) {
	__disable_irq();

	serial_port->UART->CR1 |= USART_CR1_TXEIE;

	// Enable NVIC interrupt for USART1
	if (serial_port->UART == USART1){
		// Tell the NVIC module that USART1 interrupts should be handled
		NVIC_SetPriority(USART1_IRQn, USART1PRIORITY);  // Set Priority
		NVIC_EnableIRQ(USART1_IRQn);
	}

	__enable_irq();
}


// ---------- INTERRUPT HANDLER ----------


// USART1_IRQHandler - interrupt handler function that calls the customized SerialReceiveIRQHandler function for the USART1 port
// DO NOT CHANGE THE FUNCTION NAME
// inputs: none
void USART1_IRQHandler(void)
{
    SerialPort *serial_port = &USART1_PORT;

    // -------- transmit path: TXE

    if ((serial_port->UART->ISR & USART_ISR_TXE) && (serial_port->UART->CR1 & USART_CR1_TXEIE)) {
    	serialTransmitIRQHandler(serial_port);
    }

    // -------- transmit path: TC

    if ((serial_port->UART->ISR & USART_ISR_TC) && (serial_port->UART->CR1 & USART_CR1_TCIE)) {
    	serial_port->UART->CR1 &= ~USART_CR1_TCIE;
        // NOTHING TO BE DONE
    }

    // -------- receive path: RXNE

    if ((serial_port->UART->ISR & USART_ISR_RXNE) && (serial_port->UART->CR1 & USART_CR1_RXNEIE)) {
    	SerialReceiveIRQHandler(serial_port);
    }
}


// SerialReceiveIRQHandler - store the incoming characters to the rx buffer until either termination character or buffer limit reached
// Input: serial port pointer
void SerialReceiveIRQHandler(SerialPort *serial_port) {
	uint8_t ch = serial_port->UART->RDR;

	if (ch == TERMINATION_CR || ch == TERMINATION_LF) {

		uint8_t rx = serial_port->receiving;
		serial_port->buffer[rx][serial_port->buf_len[rx]] = '\0';

		if (serial_port->callback_function) {
			serial_port->callback_function(serial_port);
		}

		serial_port->receiving ^= 1;
		serial_port->buf_len[serial_port->receiving] = 0;
		serial_port->tx_idx = 0;

		if (serial_port->UART->CR1 & USART_CR1_TCIE){
			serial_port->UART->CR1 |= USART_CR1_TXEIE;
		}

	} else if (serial_port->buf_len[serial_port->receiving] < BUFFER_SIZE - 1) {

		uint8_t index = serial_port->buf_len[serial_port->receiving]++;
		serial_port->buffer[serial_port->receiving][index] = ch;
	}
}



void serialTransmitIRQHandler(SerialPort *serial_port) {
	uint8_t txbuf = serial_port->receiving ^ 1;

	if (serial_port->tx_idx < serial_port->buf_len[txbuf]) {
			serial_port->UART->TDR = serial_port->buffer[txbuf][serial_port->tx_idx];
			serial_port->tx_idx++;
	}

	else {
		serial_port->UART->CR1 &= ~USART_CR1_TXEIE;
		serial_port->UART->CR1 |=  USART_CR1_TCIE;
		serial_port->buf_len[txbuf] = 0;
		serial_port->tx_idx = 0;
	}
}


// ---------- MISC SHIT ----------


void transmitStringInfo(SerialPort *serial_port) {
	// Prepare a buffer to hold the formatted callback information
	uint8_t callback_info[BUFFER_SIZE];

	// Format the received string and its length into the callback_info buffer
	sprintf((char*)callback_info, "%s: %d\r\n",
	        &serial_port->buffer[serial_port->receiving][0],
	        serial_port->buf_len[serial_port->receiving]);

	// Transmit the formatted string over UART
	transmitString(&callback_info[0], serial_port);
}

const char *SerialGetReceivedBuffer(SerialPort *serial_port) {
    return (const char*)serial_port->buffer[serial_port->receiving];
}



static const char *name[2] = { "A", "B" };

void my_rx_callback(SerialPort *serial_port) {
	uint8_t output[BUFFER_SIZE];
	sprintf((char*)output, "[RX done → buffer %s]\r\n", name[serial_port->receiving ^ 1]);
	transmitString(output, serial_port);
}




void SerialOutputChar(uint8_t data, SerialPort *serial_port) {
	while((serial_port->UART->ISR & USART_ISR_TXE) == 0){
	}

	serial_port->UART->TDR = data;
}


void SerialOutputBuffer(uint8_t *buffer, uint16_t buffer_length, SerialPort *serial_port) {
	uint32_t counter = 0;

	while(counter < buffer_length) {
		SerialOutputChar(*buffer, serial_port);
		counter++;
		buffer++;
	}

	// This code was added as the python reading program was not picking
	//  up the next sentinel properly. This should be removed when the
	//  python code is fixed.
	uint8_t complete_buffer = '\0';
	SerialOutputChar(complete_buffer, serial_port);

//	if (serial_port->completion_function != 0x00)
//		serial_port->completion_function(counter);
}

// returns 1 if valid char, 0 if timeout
uint8_t SerialReceiveChar(SerialPort *serial_port, uint8_t *received_char)
{
    uint8_t latest_character = 0;
    uint16_t timeout = 0xffff;

    while (1) {
        timeout--;
        if (timeout == 0)
            return 0;  // Timeout if no data received within the timeout period

        // Check for overrun or framing errors and clear the flags
        if (serial_port->UART->ISR & USART_ISR_ORE || serial_port->UART->ISR & USART_ISR_FE) {
            serial_port->UART->ICR |= USART_ICR_ORECF | USART_ICR_FECF;  // Clear error flags
        }

        // Wait for data to be available in the receiver (RXNE flag)
        if (serial_port->UART->ISR & USART_ISR_RXNE) {
            // Read the received character
            *received_char = serial_port->UART->RDR;
            return 1;  // Return success
        }
    }
}



// Constants
//#define SENTINEL_1 0xAA
//#define SENTINEL_2 0x55

#include "serialise.h"

uint16_t SerialInputPacketHeader(char *buffer, SerialPort *serial_port)
{
  //uint8_t latest_characters[2] = {0};
  // wait for sentinels
   while (1) {
	   buffer[0] = buffer[1];
	   uint8_t success = SerialReceiveChar(serial_port, &buffer[1]);

	   if (success == 0)
		   return 0;

	  if (buffer[0] == SENTINEL_1 && buffer[1] == SENTINEL_2) {
			  break;
	  }
  }

   uint16_t index = 2;

  while (index < sizeof(Header))
  {
	  uint8_t receivedChar;
	  uint8_t success = SerialReceiveChar(serial_port, &receivedChar);
	  if (success == 0)
		  return 0;

      buffer[index++] = receivedChar;
  }

  return index;
}


uint16_t SerialInputDataPacket(char *buffer, int length, SerialPort *serial_port) {
	uint16_t index = 0;

  while (index < length)
  {
	  uint8_t receivedChar;
	  uint8_t success = SerialReceiveChar(serial_port, &receivedChar);
	  if (success == 0)
		  return 0;

	  buffer[index++] = receivedChar;
  }

  return index;

}
