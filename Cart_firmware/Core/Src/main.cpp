/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.hh"
#include "BTTransmiterReceiver.hh"
#include "controlBox.hh"
#include "stdio.h"
#include "string.h"
#include "cart.hh"
#include "batteries.hh"

//#define DEV_MODE
//#define BT_CONNECTION_LOST_PROTECTION

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
BTTransmiterReceiver bt(&huart3);
controlBox cb(&huart5);

batteries bat(&hadc1, ADC_CHANNEL_11, ADC_CHANNEL_10);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == bt.getUARTHandler()){
		bt.getNewMsg();
	}
}



//void ADC_Select_CH0 (void)
//{
//	ADC_ChannelConfTypeDef sConfig = {0};
//	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//	 */
//	sConfig.Channel = ADC_CHANNEL_10;
//	sConfig.Rank = 1;
//	sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
//	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
//}
//
//void ADC_Select_CH1 (void)
//{
//	ADC_ChannelConfTypeDef sConfig = {0};
//	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//	 */
//	sConfig.Channel = ADC_CHANNEL_11;
//	sConfig.Rank = 1;
//	sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
//	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//	{
//		Error_Handler();
//	}
//}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM2_Init();
	MX_ADC1_Init();
	MX_I2C1_Init();
	MX_TIM3_Init();
	MX_TIM1_Init();
	MX_UART5_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */

	DCmotor rightMotor(&htim1, TIM_CHANNEL_3,
			RMOT_DIR_L_GPIO_Port, RMOT_DIR_L_Pin,
			RMOT_DIR_R_GPIO_Port, RMOT_DIR_R_Pin,
			&htim3, TIM_CHANNEL_ALL);
	DCmotor leftMotor(&htim1, TIM_CHANNEL_4,
			LMOT_DIR_L_GPIO_Port, LMOT_DIR_L_Pin,
			LMOT_DIR_R_GPIO_Port, LMOT_DIR_R_Pin,
			&htim2, TIM_CHANNEL_ALL);
	turningDCmotor turningMotor(&htim1, TIM_CHANNEL_2,
			ROT_MOT_L_GPIO_Port, ROT_MOT_L_Pin,
			ROT_MOT_R_GPIO_Port, ROT_MOT_R_Pin,
			&hi2c1);

	turningMotor.setSpeed_rpm(900);


	//	cart mCart(&rightMotor, &leftMotor, &turningMotor);

	command cmd;

	bt.init();

	//		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)batteries, 2);


	unsigned long int currentProcTime, prevProcTime, refreshDispVoltage,
	prevProcTime_dcmot, prevProcTime_turningMot, prevProcTime_Msg, incEncMesTime;
	currentProcTime = prevProcTime = prevProcTime_dcmot = incEncMesTime =
			prevProcTime_turningMot = prevProcTime_Msg = refreshDispVoltage = HAL_GetTick();

	unsigned int dcMotControlValueRefreshPeriod = 100, sendingMsgPeriod = 1000,
			IncEncMesPeriodTime = 100, refreshDispVoltagePeriod = 2000,
			turningMotControlValueRefreshPeriod = 20;

	const uint8_t quickMsgSize = 70;
	char quickMsg[quickMsgSize] = {0};

	incrementalEncoder enc(&htim2, TIM_CHANNEL_ALL, 1000.0/(double)IncEncMesPeriodTime);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
#ifndef DEV_MODE
		currentProcTime = HAL_GetTick();

		//read new message, if it's available
		if(bt.isNewMsg()){
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			bt.readNewMsg(cmd);

			prevProcTime = HAL_GetTick();
		}

#ifdef BT_CONNECTION_LOST_PROTECTION
		//connection lost, stop cart
		if(currentProcTime - prevProcTime > MAX_MSG_TIMEOUT){
			m.DCMotorSettedSpeed_rpm = 0;
			prevProcTime = HAL_GetTick();
		}
#endif

		switch(cmd.cmdName){
		case G1:
			for(int i = 0;i<sizeof(cmd.params)/sizeof(parameter);i++){
				switch(cmd.params[i].name){
				case 'L':
					//					mCart.setMovingSpeed(cmd.params[i].value);
					rightMotor.setSpeed_rpm(cmd.params[i].value);
					leftMotor.setSpeed_rpm(cmd.params[i].value);

					break;
				case 'A':
					turningMotor.setAngle_deg(cmd.params[i].value);
					break;
				}
			}
			break;
		case G4:
			for(int i = 0;i<sizeof(cmd.params)/sizeof(parameter);i++){
				switch(cmd.params[i].name){
				case 'A':
					turningMotor.setSpeed_rpm(cmd.params[i].value);
					turningMotControlValueRefreshPeriod = 8000/(unsigned int)cmd.params[i].value;
					break;
				}
			}
		case G10:
			cb.sendMsg("G10\n");
			break;
		case G11:
			cb.sendMsg("G11\n");
			break;
		case G12:
			cb.sendMsg("G12\n");
			break;
		case G13:
			cb.sendMsg("G13\n");
			break;
		case G14:
			cb.sendMsg("G14\n");
			break;
		case G15:
			cb.sendMsg("G15\n");
			break;

		case G28:
			//			mCart.setMovingSpeed(0);
			//			mCart.setTurningAngle(0);
			break;

		default:
			break;
		}

		cmd.clear();
		if(currentProcTime - prevProcTime_dcmot > dcMotControlValueRefreshPeriod){
			prevProcTime_dcmot = HAL_GetTick();

			leftMotor.refreshControlState();
			rightMotor.refreshControlState();
		}

		if(currentProcTime - prevProcTime_turningMot > turningMotControlValueRefreshPeriod){
			prevProcTime_turningMot = HAL_GetTick();

			turningMotor.refreshControlState();
		}

		if(currentProcTime - incEncMesTime > IncEncMesPeriodTime){
			incEncMesTime = HAL_GetTick();

			sprintf(quickMsg, "%0.2f\n",enc.readCurrentWheelSpeed_rpm());
			bt.sendMsg(quickMsg);
		}

		if(currentProcTime - refreshDispVoltage > refreshDispVoltagePeriod){
			refreshDispVoltage = HAL_GetTick();

			//			bat.readLogicBatteryVoltage_volts();
			//			bat.readMotorBatteryVoltage_volts();

			//			sprintf(quickMsg, "G10 L%0.2f M%0.2f\n",
			//					bat.readLogicBatteryVoltage_percent(),
			//					bat.readMotorBatteryVoltage_percent());
			sprintf(quickMsg, "G10 L%0.2f M%0.2f\n",
					48.52,
					72.33);
			cb.sendMsg(quickMsg);
		}

		if(currentProcTime - prevProcTime_Msg > sendingMsgPeriod){
			prevProcTime_Msg = HAL_GetTick();

			//			sprintf(quickMsg, "G10 L%0.2f M%0.2f\n",
			//					bat.readLogicBatteryVoltage_percent(),
			//					bat.readMotorBatteryVoltage_percent());
			//			if(!bt.sendMsg(quickMsg)){
			//				HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			//				HAL_Delay(500);
			//				HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			//			}

			//			for(int i=0; i<quickMsgSize;i++)quickMsg[i]=0;
		}


		//		leftMotor.setSpeed_rpm(m.DCMotorSettedSpeed_rpm);
		//		rightMotor.setSpeed_rpm(m.DCMotorSettedSpeed_rpm);
		//		turningMotor.setAngle_deg(m.TurningDCMotorSettedAngle_deg);

#else

		if(HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4) != HAL_OK){
			while(1){};
		}
		if(HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3) != HAL_OK){
			while(1){};
		}

		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 400);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 400);


#endif


		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	while(1){
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(200);
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(200);
	}
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
