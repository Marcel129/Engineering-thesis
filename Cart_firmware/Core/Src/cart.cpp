/*
 * cart.cpp
 *
 *  Created on: 1 gru 2022
 *      Author: Marcel
 */

#include <cart.hh>

cart::cart( DCmotor *rm,DCmotor *lm,turningDCmotor* tm){

	if(rm != nullptr && lm != nullptr && tm != nullptr){
		rightMotor = rm;
		leftMotor = lm;
		turningMotor = tm;
	}
	else{
		while(1){};
	}

	turningMotor->setSpeed_rpm(500);
	turningMotor->setAngleRefreshPeriod(20);
	turningMotor->setRotaryDirection(MOTOR_ROTARY_DIRECTION_CW);

	leftMotor->setRotaryDirection(MOTOR_ROTARY_DIRECTION_CCW);
	rightMotor->setRotaryDirection(MOTOR_ROTARY_DIRECTION_CCW);
}

void cart::refreshLinearSpeedState(){
	leftMotor->refreshControlState();
	rightMotor->refreshControlState();
}

void cart::refreshAngleState(){
	turningMotor->refreshControlState();
}

void cart::setMovingSpeed(double s){
	leftMotor->setSpeed_rpm(s);
	rightMotor->setSpeed_rpm(s);

}
void cart::setTurningAngle(double a){
	turningMotor->setAngle_deg(a);
}

void cart::setMotorPIDParams(double p, double i, double d, char motor){
	switch(motor){
	case LEFT_MOTOR:
		leftMotor->setPIDparams(p,i,d);
		break;
	case RIGHT_MOTOR:
		rightMotor->setPIDparams(p,i,d);
		break;
	}
}

double cart::getRotationalSpeed(char motor){
	switch(motor){
	case LEFT_MOTOR:
		return leftMotor->readCurrentSpeed_rpm();
	case RIGHT_MOTOR:
		return rightMotor->readCurrentSpeed_rpm();
	default:
		return -1;
	}
}


