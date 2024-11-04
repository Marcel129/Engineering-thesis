/*
 * encoder.h
 *
 *  Created on: Nov 7, 2022
 *      Author: Marcel
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#define ENC_MEAS_COUNT 10
#define IMPULSES_PER_ROVOLUTE 12
#define BELT_GEAR_RATIO (15.0/48.0)
#define MIN_ENCODER_MEASUREMENT_FREQ_HZ 1

#include "tim.h"
#include "i2c.h"
#include "gpio.h"
#include "stdlib.h"

#include "as5600.h"


class incrementalEncoder{
	int prevMeasure, actEncMesIndex;
	int lastMeasurements[ENC_MEAS_COUNT];

	double measFrequency_Hz;

	TIM_HandleTypeDef * encoderTimHandler;
	int encoderTimChannel;
public:
	incrementalEncoder(TIM_HandleTypeDef * timHand, int timChan, double measFreq);
//	void init(TIM_HandleTypeDef * timHand, int timChan);
	double readCurrentWheelSpeed_rpm();
	double readCurrentSpeed_impulses();
};


class absoluteEncoder{
	I2C_HandleTypeDef * encoderI2CHandler;
	uint32_t lastMes;
public:
	absoluteEncoder(I2C_HandleTypeDef *);
	void init(I2C_HandleTypeDef *);
	double readCurrentAngle_deg();
};

#endif /* ENCODER_H_ */
