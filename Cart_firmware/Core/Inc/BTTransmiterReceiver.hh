/*
 * BTTransmiterReceiver.h
 *
 *  Created on: Oct 21, 2022
 *      Author: Marcel
 */

#ifndef BTTRANSMITERRECEIVER_H_
#define BTTRANSMITERRECEIVER_H_

#include "usart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#define SEND_FULL_RECEIVED_MSG

#define BT_MSG_BUFFER_SIZE 30
#define BT_MSG_LEN 14
#define BT_OK_MSG "OK\n"
#define BT_NOK_MSG "NOK\n"

const char endOfMsg = '\n';
const char dataSplitChar = ' ';

#define MAX_MSG_TIMEOUT 1000
#define MSG_PRESCALER 10.0

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define BT_PARAMS_PER_COMMAND_MAX_COUNT 6
#define BT_PARAMS_MAX_LENGHT 20

/*
 * frame shape:
 * Xx [Znn] CRC\n
 * X - command name, G or M, mandatory
 * x - command number, mandatory
 * Znn - parameter name and value, optional or mandatory, depends on command type.
 * It could be more than 1 parameter
 * CRC - control sum
 * (space) - argument separator
 * \n - end of frame character
 */
enum commandName {
	UNDEFINED,G1,G4,G10,G11,G12,G13,G14,G15,G28
};

struct parameter{
	char name;
	double value;

	parameter(){
		name = 0;
		value = 0;
	}
};

struct command {
	commandName cmdName;
	uint8_t paramsCount;
	//without cmd name and crc
	parameter params[BT_PARAMS_PER_COMMAND_MAX_COUNT - 2];

	command();
	void clear();
};

struct msgData{
	char cmdParams[BT_PARAMS_PER_COMMAND_MAX_COUNT][BT_PARAMS_MAX_LENGHT];
	uint8_t paramsCount;

	msgData();
	void clear();
};

class BTTransmiterReceiver {
private:
	UART_HandleTypeDef * btUART;

	bool newMsg, crcCorrect;
	uint8_t buff, iter, cmdNum;
	msgData m;

	bool parseMsg(command &cmd);
public:
	BTTransmiterReceiver(UART_HandleTypeDef *);
	void init();
	bool sendMsg(char *);

	void getNewMsg();
	bool readNewMsg(command &cmd);
	bool isNewMsg()const;
	void clearBuffer();

	void sendReceivedMsg();
	unsigned long int crc16(char*);

	UART_HandleTypeDef * getUARTHandler()const;
};

#endif /* BTTRANSMITERRECEIVER_H_ */
