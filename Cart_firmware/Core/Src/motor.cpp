/*
 * motor.cpp
 *
 *  Created on: Oct 20, 2022
 *      Author: Marcel
 */

#include "motor.hh"

//enum motorRotaryDirection {free, cw, ccw, stop};

motor::motor(){
	pidP = 0;
	pidI = 0;
	pidD = 0;
	settedSpeed_rpm = 0;
	currentSpeed_rpm = 0;
	controlValue = 0;
	pidError = 0;

	dirPinLeft = 0;
	dirPinRight = 0;
	dirPinLeft_Port = nullptr;
	dirPinRight_Port = nullptr;

	PWMTimHandler = nullptr;
	PWMTimChannel = 0;
}

double motor::getCurrentSpeed_rpm()const{
	return currentSpeed_rpm;
}

void motor::setPIDparams(double P, double I, double D){
	P>0 ? pidP = P : pidP = 0;
	I>0 ? pidI = I : pidI = 0;
	D>0 ? pidD = D : pidD = 0;
}

/****************************** DCMOTOR  CLASS *****************************/

DCmotor::DCmotor(TIM_HandleTypeDef* timHand, uint16_t timChannel,
		GPIO_TypeDef * dPLPort, uint16_t dPL,
		GPIO_TypeDef * dPRPort, uint16_t dPR,
		TIM_HandleTypeDef* eTimHand, uint16_t eTimChannel)
{
	//assign PWM timer handler and channel
	if(timHand == nullptr || dPLPort == nullptr ||
			dPRPort == nullptr || eTimHand == nullptr){
		while(1){};
	}
	PWMTimHandler = timHand;
	PWMTimChannel = timChannel;

	if(HAL_TIM_PWM_Start(PWMTimHandler, PWMTimChannel) != HAL_OK){
		while(1){};
	}

//	//assign incremental encoder timer handler and channel
//	incrementalEncoder mEnc(eTimHand, eTimChannel);
//	enc = &mEnc;

	//assign control pins and their ports
	dirPinLeft_Port = dPLPort;
	dirPinLeft = dPL;
	dirPinRight_Port = dPRPort;
	dirPinRight = dPR;

	//reset pin's states
	HAL_GPIO_WritePin(dirPinLeft_Port, dirPinLeft, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(dirPinRight_Port, dirPinRight, GPIO_PIN_RESET);
}

void DCmotor::setSpeed_rpm(double value){

	if(value > DCMOTOR_MAX_SPEED_RPM){
		settedSpeed_rpm = DCMOTOR_MAX_SPEED_RPM;
		return;
	}
	else if(value < DCMOTOR_MIN_SPEED_RPM){
		settedSpeed_rpm = DCMOTOR_MIN_SPEED_RPM;
		return;
	}
	else
		settedSpeed_rpm = value;
}

void motor::setRotaryDirection(uint8_t dir){
	switch(dir){
	case MOTOR_ROTARY_DIRECTION_CW:
		HAL_GPIO_WritePin(dirPinLeft_Port, dirPinLeft, GPIO_PIN_SET);
		HAL_GPIO_WritePin(dirPinRight_Port, dirPinRight, GPIO_PIN_RESET);
		return;
	case MOTOR_ROTARY_DIRECTION_CCW:
		HAL_GPIO_WritePin(dirPinLeft_Port, dirPinLeft, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(dirPinRight_Port, dirPinRight, GPIO_PIN_SET);
		return;
	case MOTOR_ROTARY_DIRECTION_FREE:
		HAL_GPIO_WritePin(dirPinLeft_Port, dirPinLeft, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(dirPinRight_Port, dirPinRight, GPIO_PIN_RESET);
		return;
	case MOTOR_ROTARY_DIRECTION_STOP:
		HAL_GPIO_WritePin(dirPinLeft_Port, dirPinLeft, GPIO_PIN_SET);
		HAL_GPIO_WritePin(dirPinRight_Port, dirPinRight, GPIO_PIN_SET);
		return;
	default:
		HAL_GPIO_WritePin(dirPinLeft_Port, dirPinLeft, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(dirPinRight_Port, dirPinRight, GPIO_PIN_RESET);
	}
}

void DCmotor::refreshControlState(){

	pidError = settedSpeed_rpm - currentSpeed_rpm;

	if(pidError < 0){
		if(pidError < -DCMOTOR_STEP_VALUE){
			controlValue = currentSpeed_rpm - DCMOTOR_STEP_VALUE;
		}
		else{
			controlValue = currentSpeed_rpm + pidError;
		}
	}
	else if(pidError > 0){
		if(pidError > DCMOTOR_STEP_VALUE){
			controlValue = currentSpeed_rpm + DCMOTOR_STEP_VALUE;
		}
		else{
			controlValue = currentSpeed_rpm + pidError;
		}
	}

	if(controlValue >= 0){
		setRotaryDirection(MOTOR_ROTARY_DIRECTION_CCW);
	}
	else if(controlValue < 0){
		setRotaryDirection(MOTOR_ROTARY_DIRECTION_CW);
	}

	__HAL_TIM_SET_COMPARE(PWMTimHandler, PWMTimChannel, abs(controlValue));

	currentSpeed_rpm = controlValue;
}

double DCmotor::readCurrentSpeed_rpm(){
	return enc->readCurrentWheelSpeed_rpm();
}


/**************************** TURNING  DCMOTOR  CLASS *****************************/

turningDCmotor::turningDCmotor(TIM_HandleTypeDef* PWM_Hand, uint16_t PWM_Chan,
		GPIO_TypeDef * dPL_port, uint16_t dPL,
		GPIO_TypeDef * dPR_port, uint16_t dPR,
		I2C_HandleTypeDef* I2CHand){

	if(PWM_Hand == nullptr || dPL_port == nullptr ||
			dPR_port == nullptr || I2CHand == nullptr){
		while(1){};
	}

	settedAngle_deg = 0;
	currentAngle_deg = 0;

	//assign motor's control pins and their ports
	dirPinLeft = dPL;
	dirPinRight = dPR;
	dirPinLeft_Port = dPL_port;
	dirPinRight_Port = dPR_port;

	//reset pin's states
	HAL_GPIO_WritePin(dirPinLeft_Port, dirPinLeft, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(dirPinRight_Port, dirPinRight, GPIO_PIN_RESET);

	PWMTimHandler = PWM_Hand;
	PWMTimChannel = PWM_Chan;

	if(HAL_TIM_PWM_Start(PWMTimHandler, PWMTimChannel)!= HAL_OK){
		while(1){};
	}

//	absoluteEncoder mEnc(I2CHand);
//	absEnc = &mEnc;
}

void turningDCmotor::setAngle_deg(double angle){
	if(angle > TURNING_DCMOTOR_MAX_ANGLE_DEG){
		settedAngle_deg = TURNING_DCMOTOR_MAX_ANGLE_DEG;
		return;
	}
	else if(angle < TURNING_DCMOTOR_MIN_ANGLE_DEG){
		settedAngle_deg = TURNING_DCMOTOR_MIN_ANGLE_DEG;
		return;
	}
	else
		settedAngle_deg = angle;
}

double turningDCmotor::getCurrentAngle()const{
	return currentAngle_deg;
}

void turningDCmotor::setSpeed_rpm(double value){
	if(value > TURNING_DCMOTOR_MAX_SPEED_RPM){
		settedSpeed_rpm = TURNING_DCMOTOR_MAX_SPEED_RPM;
		return;
	}
	else if(value < TURNING_DCMOTOR_MIN_SPEED_RPM){
		settedSpeed_rpm = TURNING_DCMOTOR_MIN_SPEED_RPM;
		return;
	}
	else
		settedSpeed_rpm = value;
}

double turningDCmotor::readCurrentSpeed_rpm(){
	//reads current rotary speed using absolute encoder
}

void turningDCmotor::refreshControlState(){
	angleError = settedAngle_deg - currentAngle_deg;

	if(angleError > 0){
		setRotaryDirection(MOTOR_ROTARY_DIRECTION_CW);
	}
	else if(angleError < 0){
		setRotaryDirection(MOTOR_ROTARY_DIRECTION_CCW);
	}

	if(angleError < 0){
		if(angleError < -TURNING_DCMOTOR_STEP_VALUE){
			controlValue = currentAngle_deg - TURNING_DCMOTOR_STEP_VALUE;
			__HAL_TIM_SET_COMPARE(PWMTimHandler, PWMTimChannel, settedSpeed_rpm);
		}
		else{
			//			controlValue = currentAngle_deg + angleError;
			__HAL_TIM_SET_COMPARE(PWMTimHandler, PWMTimChannel, 0);
		}
	}
	else if(angleError > 0){
		if(angleError > TURNING_DCMOTOR_STEP_VALUE){
			controlValue = currentAngle_deg + TURNING_DCMOTOR_STEP_VALUE;
			__HAL_TIM_SET_COMPARE(PWMTimHandler, PWMTimChannel, settedSpeed_rpm);
		}
		else{
			//			controlValue = currentAngle_deg + angleError;
			__HAL_TIM_SET_COMPARE(PWMTimHandler, PWMTimChannel, 0);
		}
	}

	//__HAL_TIM_SET_COMPARE(PWMTimHandler, PWMTimChannel, settedSpeed_rpm);
	currentAngle_deg = controlValue;
}

