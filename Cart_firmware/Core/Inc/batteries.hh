/*
 * batteries.hh
 *
 *  Created on: Dec 5, 2022
 *      Author: Marcel
 */

#ifndef INC_BATTERIES_HH_
#define INC_BATTERIES_HH_

#define LOGIC_MIN_SAFE_VOLTAGE 1.8
#define LOGIC_MAX_SAFE_VOLTAGE 2.4
#define MOTOR_MIN_SAFE_VOLTAGE 2.33
#define MOTOR_MAX_SAFE_VOLTAGE 2.72

#include "adc.h"

class batteries {

	ADC_HandleTypeDef * adcHandler;
	ADC_ChannelConfTypeDef * sConfig;

	uint32_t lbatChan, mbatChan;
	uint32_t lbatMesSum, mbatMesSum, lbatMesCount, mbatMesCount;

	double actMbatVol_unconverted, actLbatVol_unconverted;

	uint32_t readADCVal(uint32_t chan);


public:
	batteries(ADC_HandleTypeDef *, uint32_t lbChan, uint32_t mbChan,
			uint32_t rank=1,
			uint32_t samTime=ADC_SAMPLETIME_480CYCLES);

	void setADCChannel(uint32_t chan);

	void intermediateADCMeasure();

	double readMotorBatteryVoltage_volts();
	double readMotorBatteryVoltage_percent();
	double readLogicBatteryVoltage_volts();
	double readLogicBatteryVoltage_percent();
};

#endif /* INC_BATTERIES_HH_ */
