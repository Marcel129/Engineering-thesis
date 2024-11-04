/*
 * BTTransmiterReceiver.cpp
 *
 *  Created on: Oct 21, 2022
 *      Author: Marcel
 */

/*It is possible to send only 1 command per msg.
 *
 * G1 S[SPEED_M/S] A[ROTATION_ANGLE_DEG] - set linear vehicle speed and rotary angle of turning axis
 * G4 S[SPEED_M/S] - set max speed
 * G28 - base axes, same as G1 S0 A0
 *
 * Communication with the control box:
 * G10 L[LOGIC BATTERY LVL_%] M[MOTOR BATTERY VOLATAGE %] - set voltage values on the displays (in %)
 * 		exp: G10 L50 M70 - logic battery is 50% charged, motor battery is 70%
 * G11 - set the bt communication diode on/off
 * G12 - blink the bt com diode
 * G13 - set the error diode on/off
 * G14 - blink the error diode
 */

#include "BTTransmiterReceiver.hh"

const char* commandNamesLookUpTable[] = {"G1","G4","G10","G11","G12",
		"G13","G14","G15","G28"
};

msgData::msgData(){
	paramsCount=0;

	for(int i=0;i<BT_PARAMS_PER_COMMAND_MAX_COUNT;i++){
		for(int j=0;j<BT_PARAMS_MAX_LENGHT;j++){
			cmdParams[i][j] = 0;
		}
	}
}

void msgData::clear(){
	paramsCount=0;

	for(int i=0;i<BT_PARAMS_PER_COMMAND_MAX_COUNT;i++){
		for(int j=0;j<BT_PARAMS_MAX_LENGHT;j++){
			cmdParams[i][j] = 0;
		}
	}
}

command::command(){
	clear();
}

void command::clear(){
	cmdName = UNDEFINED;
	paramsCount = 0;
	int i = 0;
	while(i<BT_PARAMS_PER_COMMAND_MAX_COUNT - 2){
		params[i].name = 0;
		params[i].value = 0;
		++i;
	}
}

BTTransmiterReceiver::BTTransmiterReceiver(UART_HandleTypeDef * uartHandler) {
	// TODO Auto-generated constructor stub
	buff = iter = cmdNum = 0;
	newMsg = crcCorrect = false;
	btUART = uartHandler;
}

void BTTransmiterReceiver::init(){
	//	HAL_UART_Receive_IT(btUART, (uint8_t*)btRXbuffer, BT_MSG_LEN);
	HAL_UART_Receive_IT(btUART, &buff, 1);
}

UART_HandleTypeDef * BTTransmiterReceiver::getUARTHandler()const{
	return btUART;
}

void BTTransmiterReceiver::getNewMsg(){

	switch(buff){
	case ' ':
		if(m.paramsCount < BT_PARAMS_PER_COMMAND_MAX_COUNT){
			cmdNum++;
			m.paramsCount = cmdNum+1;
			iter = 0;
		}
		break;
	case '\n':
		newMsg = true;
		cmdNum = 0;
		iter = 0;
		break;
	default:
		if(cmdNum<BT_PARAMS_PER_COMMAND_MAX_COUNT && iter<BT_PARAMS_MAX_LENGHT-1
				&&cmdNum>=0&&iter>=0){
			m.cmdParams[cmdNum][iter++] = (char)buff;
		}
	}

	HAL_UART_Receive_IT(btUART, &buff, 1);
}

bool BTTransmiterReceiver::isNewMsg()const{
	return newMsg;
}

void BTTransmiterReceiver::clearBuffer(){
	newMsg = false;
	buff = iter = cmdNum = 0;
	m.clear();
}

bool BTTransmiterReceiver::sendMsg(char * msg){
	return HAL_UART_Transmit(btUART, (uint8_t*)msg, strlen(msg), 100) == HAL_OK;
}

bool BTTransmiterReceiver::readNewMsg(command &cmd){
	//there is no new message
	if(!newMsg){
		return false;
	}

	//string for crc
	char tmpBuff[BT_PARAMS_PER_COMMAND_MAX_COUNT * BT_PARAMS_MAX_LENGHT] = {0};
	for(int i =0; i<m.paramsCount-1;i++){
		strcat(tmpBuff, (const char*)m.cmdParams[i]);
		if(i<m.paramsCount-2){
			strcat(tmpBuff, " ");
		}
	}

	//check crc
	crcCorrect = atoi(m.cmdParams[m.paramsCount-1]) == crc16(tmpBuff);

#ifdef SEND_FULL_RECEIVED_MSG
	sendReceivedMsg();

	if(crcCorrect)sendMsg(", CRC ok\n");
	else{
		char tmpb2[80] = {0};
		sprintf(tmpb2, "CRC not ok: received: %d, calculated: %d\n",
				atoi(m.cmdParams[m.paramsCount-1]), crc16(tmpBuff));
		sendMsg(tmpb2);
	}
#else
	if(crcCorrect){
		sendMsg((char*)BT_OK_MSG);
	}
	else{
		sendMsg((char*)BT_NOK_MSG);
		clearBuffer();
		return false;
	}
#endif

	return parseMsg(cmd);
}


bool BTTransmiterReceiver::parseMsg(command &cmd){

	//clear previous cmd
	int i=0;
	cmd.clear();

	//find cmd name in lookup table
	while(i<sizeof(commandNamesLookUpTable)/sizeof(commandNamesLookUpTable[0])){
		if(strcmp(m.cmdParams[0],commandNamesLookUpTable[i]) == 0){
			cmd.cmdName = (commandName)(i+1);
			break;
		}
		if(cmd.cmdName != UNDEFINED) break;
		++i;
	}
	//cmd name not found
	if(cmd.cmdName == UNDEFINED){
		clearBuffer();
		return false;
	}

	//read params
	i=1;
	while(i<m.paramsCount-1){
		if(atof(m.cmdParams[i])==0){
			char *tmpChar = m.cmdParams[i]+1;
			cmd.params[i-1].name = m.cmdParams[i][0];
			cmd.params[i-1].value = atof(tmpChar);
		}
		else{
			cmd.params[i-1].value = atof(m.cmdParams[i]);
		}
		++i;
	}
	cmd.paramsCount = m.paramsCount - 2;

	clearBuffer();

	return true;
}


void BTTransmiterReceiver::sendReceivedMsg(){
	char tmpBuff[100] = {0};

	for(int i =0; i<m.paramsCount-1;i++){
		strcat(tmpBuff, (const char*)m.cmdParams[i]);
		if(i<m.paramsCount-2){
			strcat(tmpBuff, " ");
		}
	}

	sendMsg(tmpBuff);
}


unsigned long int BTTransmiterReceiver::crc16(char* s){
	char *ptr = s;
	int count = strlen(ptr);

	unsigned long int  crc;
	char iss;
	crc = 0;
	while (--count >= 0)
	{
		crc = crc ^ (int) *ptr++ << 8;
		iss = 8;
		do
		{
			if (crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc = crc << 1;
		} while(--iss);
	}
	return crc;
}

