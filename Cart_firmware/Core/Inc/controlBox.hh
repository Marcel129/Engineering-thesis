/*
 * controlBox.hh
 *
 *  Created on: Dec 4, 2022
 *      Author: Marcel
 */

#ifndef INC_CONTROLBOX_HH_
#define INC_CONTROLBOX_HH_

#include "BTTransmiterReceiver.hh"
#include "usart.h"

class controlBox {
	UART_HandleTypeDef * cbUART;
public:
	controlBox(UART_HandleTypeDef *);
	bool sendMsg(char *);
};

#endif /* INC_CONTROLBOX_HH_ */
