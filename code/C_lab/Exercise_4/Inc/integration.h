#ifndef INTEGRATION_H_
#define INTEGRATION_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "serial.h"
#include "gpio.h"
#include "timer.h"


uint8_t isNumeric(const char *str);


void commandParser(SerialPort *serial_port);

#endif /* INTEGRATION_H_ */
