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
//struct _SerialPort {
//	USART_TypeDef *UART;
//	GPIO_TypeDef *GPIO;
//	volatile uint32_t MaskAPB2ENR;
//	volatile uint32_t MaskAPB1ENR;
//	volatile uint32_t MaskAHBENR;
//	volatile uint32_t SerialPinModeValue;
//	volatile uint32_t SerialPinSpeedValue;
//	volatile uint32_t SerialPinAlternatePinValueLow;
//	volatile uint32_t SerialPinAlternatePinValueHigh;
//
//	callback_t callback_function;
//
//	uint8_t buffer[2][BUFFER_SIZE];
//	volatile uint8_t buf_len[2];
//	volatile uint8_t receiving;
//
//	volatile uint8_t tx_idx;
//};


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

void SerialInitialise(uint32_t baudRate, SerialPort *p, callback_t callback) {

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	RCC->AHBENR |= p->MaskAHBENR;
	p->GPIO->MODER = p->SerialPinModeValue;
	p->GPIO->OSPEEDR = p->SerialPinSpeedValue;
	p->GPIO->AFR[0] |= p->SerialPinAlternatePinValueLow;
	p->GPIO->AFR[1] |= p->SerialPinAlternatePinValueHigh;
	RCC->APB1ENR |= p->MaskAPB1ENR;
	RCC->APB2ENR |= p->MaskAPB2ENR;

	// Get a pointer to the 16 bits of the BRR register that we want to change
	uint16_t *baud_rate_config = (uint16_t*)&p->UART->BRR; // only 16 bits used!

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

	p->callback_function = callback;

	p->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

	p->tx_idx = 0;
	p->buf_len[0] = 0;
	p->buf_len[1] = 0;
	p->receiving = 0;
}

// ---------- UASRT SPECIFCI FUNCTIONALITY ----------

// RECEIVE STRING

// SerialInputString - receive input string with termination character '\r' or '\n' from the serial port and save to buffer
// Input: pointer to buffer, serial port, pointer to callback function
void receiveString(SerialPort *p) {
    uint16_t i = 0;  // Buffer index

    while (1) {
        // Wait for RXNE flag (data available)
        while ((p->UART->ISR & USART_ISR_RXNE) == 0){ }

        uint8_t received_char = p->UART->RDR;

        if ((received_char == TERMINATION_CR)||(received_char == TERMINATION_LF)) {
            break;
        }

        p->buffer[p->receiving][i] = received_char;
        i++;

        // Prevent buffer overflow
        if (i >= BUFFER_SIZE - 1) {
            break;
        }
    }

    // Null-terminate the string
    p->buffer[p->receiving][i] = '\0';
    p->buf_len[p->receiving] = i;

    if (p->callback_function) {
    	p->callback_function(p);
    }
}


// TRANSMIT STRING

// SerialOutputString - output a NULL TERMINATED string to the serial port
// Input: pointer to a NULL-TERMINATED string (if not null terminated, there will be problems)
void transmitString(uint8_t *buffer, SerialPort *p) {
	// while the buffer is not empty
	while(*buffer) {
		// print out the character of the buffer pointer to the serial port
		while((p->UART->ISR & USART_ISR_TXE) == 0) { }

		p->UART->TDR = *buffer;
		buffer++;
	}
}


// ---------- INTERRUPT INITIALISATION ----------


// SerialEnableInterrupt - initialising interrupt for the given UART line and initialise the callback function
// inputs: serial port pointer, callback function pointer
void serialReceiveInterrupt(SerialPort *p){
	__disable_irq();

	// Enable RXNE interrupt
	p -> UART -> CR1 |= USART_CR1_RXNEIE;

	// Enable NVIC interrupt for USART1
	if (p->UART == USART1){
		// Tell the NVIC module that USART1 interrupts should be handled
		NVIC_SetPriority(USART1_IRQn, USART1PRIORITY);  // Set Priority
		NVIC_EnableIRQ(USART1_IRQn);
	}

	__enable_irq();

}

void serialTransmitInterrupt(SerialPort *p) {
	__disable_irq();

	p->UART->CR1 |= USART_CR1_TXEIE;

	// Enable NVIC interrupt for USART1
	if (p->UART == USART1){
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
    SerialPort *p = &USART1_PORT;

    // -------- transmit path: TXE

    if ((p->UART->ISR & USART_ISR_TXE) && (p->UART->CR1 & USART_CR1_TXEIE)) {
    	serialTransmitIRQHandler(p);
    }

    // -------- transmit path: TC

    if ((p->UART->ISR & USART_ISR_TC) && (p->UART->CR1 & USART_CR1_TCIE)) {
        p->UART->CR1 &= ~USART_CR1_TCIE;
        // NOTHING TO BE DONE
    }

    // -------- receive path: RXNE

    if ((p->UART->ISR & USART_ISR_RXNE) && (p->UART->CR1 & USART_CR1_RXNEIE)) {
    	SerialReceiveIRQHandler(p);
    }
}

// SerialReceiveIRQHandler - store the incoming characters to the rx buffer until termination character or buffer limit reached
// Input: serial port pointer
void SerialReceiveIRQHandler(SerialPort *p) {
	uint8_t ch = p->UART->RDR;

	if (ch == TERMINATION_CR || ch == TERMINATION_LF) {

		uint8_t rx = p->receiving;
		p->buffer[rx][p->buf_len[rx]] = '\0';

		if (p->callback_function) p->callback_function(p);

		p->receiving ^= 1;
		p->buf_len[p->receiving] = 0;
		p->tx_idx = 0;
		if (p->UART->CR1 & USART_CR1_TCIE) p->UART->CR1 |= USART_CR1_TXEIE;

	} else if (p->buf_len[p->receiving] < BUFFER_SIZE - 1) {

		p->buffer[p->receiving][p->buf_len[p->receiving]++] = ch;
	}
}


void serialTransmitIRQHandler(SerialPort *p) {
	uint8_t txbuf = p->receiving ^ 1;

	if (p->tx_idx < p->buf_len[txbuf]) {
		p->UART->TDR = p->buffer[txbuf][p->tx_idx++];
	} else {
		p->UART->CR1 &= ~USART_CR1_TXEIE;
		p->UART->CR1 |=  USART_CR1_TCIE;
		p->buf_len[txbuf] = 0;
		p->tx_idx = 0;
	}
}


// ---------- MISC SHIT ----------


void transmitStringInfo(SerialPort *p) {
	uint8_t callback_info[BUFFER_SIZE];
	sprintf((char*)callback_info, "%s: %d\r\n", &p->buffer[p->receiving][0], p->buf_len[p->receiving]);
	transmitString(&callback_info[0], p);
}


static const char *name[2] = { "A", "B" };

void my_rx_callback(SerialPort *p) {
	uint8_t output[BUFFER_SIZE];
	sprintf((char*)output, "[RX done → buffer %s]\r\n", name[p->receiving ^ 1]);
	transmitString(output, p);
}





























