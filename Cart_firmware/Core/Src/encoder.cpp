/*
 * encoder.cpp
 *
 *  Created on: Nov 7, 2022
 *      Author: Marcel
 */

#include "encoder.h"

incrementalEncoder::incrementalEncoder(TIM_HandleTypeDef * timHand, int timChan, double mesFreq){
	if(timHand != nullptr){
		encoderTimHandler = timHand;
		encoderTimChannel = timChan;

		if(HAL_TIM_Encoder_Start(encoderTimHandler, encoderTimChannel)!=HAL_OK){
			while(1){};
		}
	}
	else{
		while(1){};
	}

	for(int i = 0; i< ENC_MEAS_COUNT;i++)lastMeasurements[i] = 0;
	prevMeasure = 0;
	actEncMesIndex = 0;

	if(mesFreq > MIN_ENCODER_MEASUREMENT_FREQ_HZ){
		measFrequency_Hz = mesFreq;
	}
	else{
		measFrequency_Hz = MIN_ENCODER_MEASUREMENT_FREQ_HZ;
	}

}

double incrementalEncoder::readCurrentSpeed_impulses(){
	int tmpMes = __HAL_TIM_GET_COUNTER(encoderTimHandler);
	double actEncMes = 0;

	if(abs(prevMeasure-tmpMes)>2500){
		//counter overload
		if(prevMeasure-tmpMes > 0){
			lastMeasurements[actEncMesIndex] = 2999 - prevMeasure + tmpMes;
		}
		else{
			lastMeasurements[actEncMesIndex] = 2999 - tmpMes + prevMeasure;
		}
	}
	else{
		lastMeasurements[actEncMesIndex] = tmpMes - prevMeasure;
	}

	for(int i = 0; i<ENC_MEAS_COUNT; i++){
		actEncMes += (double)lastMeasurements[i];
	}

	actEncMes = actEncMes/(double)ENC_MEAS_COUNT;

	actEncMesIndex = (actEncMesIndex +1)%ENC_MEAS_COUNT;
	prevMeasure = tmpMes;

	return actEncMes;
}

double incrementalEncoder::readCurrentWheelSpeed_rpm(){
	return (this->readCurrentSpeed_impulses()/IMPULSES_PER_ROVOLUTE)*BELT_GEAR_RATIO*measFrequency_Hz*60;
}












absoluteEncoder::absoluteEncoder(I2C_HandleTypeDef * i2cHand){
	init(i2cHand);
}

void absoluteEncoder::init(I2C_HandleTypeDef * i2cHand){
	if(i2cHand != nullptr){
		encoderI2CHandler = i2cHand;
		lastMes = 0;
	}
	else{
		while(1){};
	}
}
