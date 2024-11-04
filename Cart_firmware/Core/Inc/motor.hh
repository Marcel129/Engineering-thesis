/*
 * motor.hh
 *
 *  Created on: Oct 20, 2022
 *      Author: Marcel
 */

#ifndef SRC_MOTOR_HH_
#define SRC_MOTOR_HH_

#define DCMOTOR_STEP_VALUE 30
#define TURNING_DCMOTOR_STEP_VALUE 5

#define MOTOR_ROTARY_DIRECTION_FREE 0
#define MOTOR_ROTARY_DIRECTION_CW 1
#define MOTOR_ROTARY_DIRECTION_CCW 2
#define MOTOR_ROTARY_DIRECTION_STOP 3

#define DCMOTOR_MAX_SPEED_RPM 400
#define DCMOTOR_MIN_SPEED_RPM -400

#define TURNING_DCMOTOR_MAX_ANGLE_DEG 1000
#define TURNING_DCMOTOR_MIN_ANGLE_DEG -1000

#define TURNING_DCMOTOR_MAX_SPEED_RPM 1000
#define TURNING_DCMOTOR_MIN_SPEED_RPM 0

#include "tim.h"
#include "gpio.h"
#include "stdlib.h"

#include "encoder.h"

class motor {
protected:
	//pid and control params
	double pidP, pidI, pidD;
	double settedSpeed_rpm, currentSpeed_rpm, controlValue, pidError;

	//rotary direction pins and their's ports
	uint16_t dirPinLeft, dirPinRight;
	GPIO_TypeDef * dirPinLeft_Port;
	GPIO_TypeDef * dirPinRight_Port;

	TIM_HandleTypeDef * PWMTimHandler;
	uint16_t PWMTimChannel;
public:
	motor();
	virtual void setSpeed_rpm(double value) = 0;
	virtual double readCurrentSpeed_rpm() = 0;
	virtual void refreshControlState(){};

	double getCurrentSpeed_rpm()const;
	void setPIDparams(double, double, double);
	void setRotaryDirection(uint8_t);
};


class DCmotor : public motor {
	incrementalEncoder * enc;
public:
	DCmotor(TIM_HandleTypeDef*, uint16_t timChannel,
			GPIO_TypeDef * dirPinLeft_Port, uint16_t dirPinLeft,
			GPIO_TypeDef * dirPinRight_Port, uint16_t dirPinRight,
			TIM_HandleTypeDef*, uint16_t eTimChannel);
	void setSpeed_rpm(double value) override;
	double readCurrentSpeed_rpm()override;
	void refreshControlState() override;
};


class turningDCmotor : public motor {
	double settedAngle_deg, currentAngle_deg, angleError;
	double angleRefreshPeriod;
	absoluteEncoder * absEnc;
public:
	turningDCmotor(TIM_HandleTypeDef*, uint16_t timChannel,
			GPIO_TypeDef * dirPinLeft_Port, uint16_t dirPinLeft,
			GPIO_TypeDef * dirPinRight_Port, uint16_t dirPinRight,
			I2C_HandleTypeDef*);
	void refreshControlState()override;
	double readCurrentSpeed_rpm() override;
	void setSpeed_rpm(double value) override;
	void setAngle_deg(double angle);
	double getCurrentAngle()const;

	double getAngleRefreshPeriod()const{
		return angleRefreshPeriod;
	}
	void setAngleRefreshPeriod(double p){
		angleRefreshPeriod = p;
	}
};

#endif /* SRC_MOTOR_HH_ */
