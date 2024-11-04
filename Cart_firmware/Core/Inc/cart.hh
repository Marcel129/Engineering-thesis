/*
 * cart.hh
 *
 *  Created on: 1 gru 2022
 *      Author: Marcel
 */

#ifndef INC_CART_HH_
#define INC_CART_HH_

#include "motor.hh"
#include "stdio.h"
#include "string.h"

#define LEFT_MOTOR 'l'
#define RIGHT_MOTOR 'r'

class cart {
	DCmotor *rightMotor, *leftMotor;
	turningDCmotor * turningMotor;

public:
	cart( DCmotor *rm, DCmotor * lm, turningDCmotor * tm);

	void refreshLinearSpeedState();
	void refreshAngleState();

	void setMotorPIDParams(double p, double i, double d, char motor);
	void setMovingSpeed(double s);
	void setTurningAngle(double a);

	double getRotationalSpeed(char);
};

#endif /* INC_CART_HH_ */
