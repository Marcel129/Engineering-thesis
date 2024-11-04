/*
 * batteries.cpp
 *
 *  Created on: Dec 5, 2022
 *      Author: Marcel
 */

#include <batteries.hh>

batteries::batteries(ADC_HandleTypeDef  *nADCHand, uint32_t lbChan, uint32_t mbChan, uint32_t rank, uint32_t samTime) {
	if(nADCHand == nullptr) Error_Handler();

	adcHandler = nADCHand;

	lbatChan = lbChan;
	mbatChan = mbChan;

	lbatMesSum = mbatMesSum = lbatMesCount = mbatMesCount = 0;
	actLbatVol_unconverted = actMbatVol_unconverted = 0;

	ADC_ChannelConfTypeDef sC = {0};

	sC.Channel = lbChan;
	sC.Rank = rank;
	sC.SamplingTime = samTime;

	sConfig = &sC;

	if(HAL_ADC_ConfigChannel(adcHandler, sConfig) != HAL_OK) Error_Handler();

}

void batteries::intermediateADCMeasure(){

	if(lbatMesSum < 4294960000){//max uint32_t size
		setADCChannel(lbatChan);
		HAL_ADC_Start(&hadc1);
		while(HAL_ADC_PollForConversion(&hadc1, 5) != HAL_OK){};
		lbatMesSum += HAL_ADC_GetValue(adcHandler);
		++lbatMesCount;
	}

	if(mbatMesSum < 4294960000){//max uint32_t size
		setADCChannel(mbatChan);
		HAL_ADC_Start(&hadc1);
		while(HAL_ADC_PollForConversion(&hadc1, 5) != HAL_OK){};
		mbatMesSum += HAL_ADC_GetValue(adcHandler);
		++mbatMesCount;
	}

}

void batteries::setADCChannel(uint32_t chan){
	sConfig->Channel = chan;

		if(chan == mbatChan){
			sConfig->Rank = 2;
		}
		else if(chan == lbatChan){
			sConfig->Rank = 1;
		}

	if (HAL_ADC_ConfigChannel(adcHandler, sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

uint32_t batteries::readADCVal(uint32_t chan){

	uint32_t retVal = 0;

	if(chan == lbatChan){
		retVal = actLbatVol_unconverted = lbatMesSum/lbatMesCount;
		lbatMesSum = lbatMesCount = 0;
	}
	else if(chan == mbatChan){
		retVal = actMbatVol_unconverted = mbatMesSum/mbatMesCount;
		mbatMesSum = mbatMesCount = 0;
	}

	return retVal;
}

double batteries::readMotorBatteryVoltage_volts(){
	return (((double)readADCVal(mbatChan)/4096.0)*3.3);
}
double batteries::readMotorBatteryVoltage_percent(){
	double rvol = ((actMbatVol_unconverted/4096.0)*3.3),
			retVal = (((rvol-MOTOR_MIN_SAFE_VOLTAGE)*100.0)/(MOTOR_MAX_SAFE_VOLTAGE-MOTOR_MIN_SAFE_VOLTAGE));
	return retVal > 0 ? retVal : 0.0;
}
double batteries::readLogicBatteryVoltage_volts(){
	return (((double)readADCVal(lbatChan)/4096.0)*3.3);
}
double batteries::readLogicBatteryVoltage_percent(){
	double rvol = ((actLbatVol_unconverted/4096.0)*3.3),
			retVal = (((rvol-LOGIC_MIN_SAFE_VOLTAGE)*100.0)/(LOGIC_MAX_SAFE_VOLTAGE-LOGIC_MIN_SAFE_VOLTAGE));
	return retVal > 0 ? retVal : 0.0;
}

