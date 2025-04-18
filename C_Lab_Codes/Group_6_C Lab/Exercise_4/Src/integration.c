#include "integration.h"


void commandParser(SerialPort *serial_port){

	char command_line[BUFFER_SIZE];
	strcpy(command_line, (char*) serial_port->buffer[serial_port->receiving]);
	char *command = strtok(command_line, " ");
	char *line = strtok(NULL, "");


	if (strcmp(command, "led") == 0) {
		if (!isNumeric(line) || strlen(line) != 8){
			transmitString((uint8_t *)"Invalid Input!\r\n", serial_port);
			return;
		}

		for (int i = 0; i < 8; i++){
			if (line[i] != '0' && line[i] != '1') return;
		}

		uint8_t pattern = strtol(line, NULL, 2);
		setLEDState(pattern);

	} else if (strcmp(command, "serial") == 0) {

		transmitString((uint8_t *)"\r\n", serial_port);
		transmitString((uint8_t *)line, serial_port);

	} else if (strcmp(command, "oneshot") == 0) {

		if (!isNumeric(line)) {
			transmitString((uint8_t *)"Invalid Input!\r\n", serial_port);
			return;
		}

		startOneShotTIM3((uint32_t)atoi(line), &invertLEDs);

	} else if (strcmp(command, "timer") == 0) {

		if (!isNumeric(line)) {
			transmitString((uint8_t *)"Invalid Input!\r\n", serial_port);
			return;
		}

		// resets reather than initiialise:
		if (TIM2->CR1 & TIM_CR1_CEN) {
			resetTIM2((uint32_t)atoi(line));
		} else {
			initialiseTIM2((uint32_t)atoi(line));
			setTIM2callback(&chaseLed);
		}

	} else {

		transmitString((uint8_t *)"Invalid Input!\r\n", serial_port);
		return;

	}
	transmitString((uint8_t *)"\r\n", serial_port);
}

uint8_t isNumeric(const char *str) {
    if (str == NULL || *str == '\0') return 0; // empty or NULL

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return 0;
        }
    }
    return 1;
}

























