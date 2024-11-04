/*
 * controlBox.cpp
 *
 *  Created on: Dec 4, 2022
 *      Author: Marcel
 */

#include <controlBox.hh>

controlBox::controlBox(UART_HandleTypeDef * cbUARTHand) {
	if(cbUARTHand != nullptr){
		cbUART = cbUARTHand;
	}
}

bool controlBox::sendMsg(char * msg){
	return HAL_UART_Transmit(cbUART, (uint8_t*)msg, strlen(msg), 100) == HAL_OK;
}

