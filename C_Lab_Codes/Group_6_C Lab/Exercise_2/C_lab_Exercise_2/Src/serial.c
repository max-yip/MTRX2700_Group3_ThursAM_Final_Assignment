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

void serialInitialise(uint32_t baudRate, SerialPort *serial_port, callback_t callback) {

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
		*baud_rate_config = 0x341;  // 9600 at 8MHz
		break;
	case BAUD_19200:
		*baud_rate_config = 0x1A1;  // 19200 at 8MHz
		break;
	case BAUD_38400:
		*baud_rate_config = 0xD0;  // 38400 at 8MHz
		break;
	case BAUD_57600:
		*baud_rate_config = 0x8B;  // 57600 at 8MHz
		break;
	case BAUD_115200:
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	}

	serial_port->callback_function = callback;

	serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

	serial_port->tx_idx = 0;
	serial_port->buf_len[0] = 0;
	serial_port->buf_len[1] = 0;
	serial_port->receiving = 0;
}

// ---------- UASRT SPECIFCI FUNCTIONALITY ----------

// RECEIVE STRING

// SerialInputString - receive input string with termination character '\r' or '\n' from the serial port and save to buffer
// Input: pointer to buffer, serial port, pointer to callback function
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

// SerialOutputString - output a NULL TERMINATED string to the serial port
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


// serialReceiveInterrupt - initialising interrupt for the given UART line and initialise the callback function
// inputs: serial port pointer, callback function pointer
void serialReceiveInterrupt(SerialPort *serial_port){
	__disable_irq();

	// Enable RXNE interrupt
	serial_port->UART->CR1 |= USART_CR1_RXNEIE;

	// Enable NVIC interrupt for USART1
	if (serial_port->UART == USART1){
		// Tell the NVIC module that USART1 interrupts should be handled
		NVIC_SetPriority(USART1_IRQn, USART1PRIORITY);  // Set Priority
		NVIC_EnableIRQ(USART1_IRQn);
	}

	__enable_irq();

}

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


// USART1_EXTI25_IRQHandler - interrupt handler function that calls the customized SerialReceiveIRQHandler function for the USART1 port
// DO NOT CHANGE THE FUNCTION NAME
// inputs: none
void USART1_EXTI25_IRQHandler(void)
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
    	serialReceiveIRQHandler(serial_port);
    }
}



// SerialReceiveIRQHandler - store the incoming characters to the rx buffer until termination character or buffer limit reached
// Input: serial port pointer
void serialReceiveIRQHandler(SerialPort *serial_port) {
	uint8_t ch = serial_port->UART->RDR;

	if (ch == TERMINATION_CR || ch == TERMINATION_LF) {

	    // Null-terminate the received string
	    uint8_t rx = serial_port->receiving;
	    serial_port->buffer[rx][serial_port->buf_len[rx]] = '\0';

	    // Call the registered callback function if it exists
	    if (serial_port->callback_function) {
	        serial_port->callback_function(serial_port);
	    }

	    // Switch to the other buffer for receiving
	    serial_port->receiving ^= 1;

	    // Reset the new receiving buffer's length and TX index
	    serial_port->buf_len[serial_port->receiving] = 0;
	    serial_port->tx_idx = 0;

	    // Enable transmit interrupt if transmission complete interrupt is already enabled
	    if (serial_port->UART->CR1 & USART_CR1_TCIE) {
	        serial_port->UART->CR1 |= USART_CR1_TXEIE;
	    }
	}

	else if (serial_port->buf_len[serial_port->receiving] < BUFFER_SIZE - 1) {

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


static const char *name[2] = { "A", "B" };

void my_rx_callback(SerialPort *serial_port) {
	// output a confirmation message stating which buffer we are using
	uint8_t output[BUFFER_SIZE];
	uint8_t prev_port = serial_port->receiving ^ 1;
	sprintf((char*)output, "[RX done → buffer %s]\r\n", name[prev_port]);
	transmitString(output, serial_port);
}
