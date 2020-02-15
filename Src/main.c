/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bh1750.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
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
float BH1750_lux_1;
char msg[3];
int NightMode=10;
int BH1750_lux_int_1;
char buffer[40];
uint8_t size;
float J_z, J_f;
int wyp;
int pom=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, msg, 3);
  BH1750_Init(&hi2c2);
  BH1750_SetMode(CONTINUOUS_HIGH_RES_MODE_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  J_z=__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1);
  while (1)
  {
	 	  if(BH1750_OK == BH1750_ReadLight(&BH1750_lux_1))
	 	  {
	 		 BH1750_lux_int_1 = BH1750_lux_1;

	 		 J_f=J_z*((1000-BH1750_lux_1)/1000);
	 		 wyp=J_f/(0.75);

	 		 if(wyp>0 && wyp <=1000)
	 		 {
	 			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, wyp);
	 			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, wyp*NightMode/10);
	 			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, wyp*NightMode/10);
	 		 }
	 		 else if(wyp>1000)
	 		 {
	 			 wyp=1000;
	 			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, wyp);
	 			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, wyp*NightMode/10);
	 			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, wyp*NightMode/10);
	 		 }
	 		 else
	 		 {
	 			 wyp=0;
	 			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, wyp);
	 			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, wyp*NightMode/10);
	 			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, wyp*NightMode/10);
	 		 }
	 		 if(pom==1)
	 		 {
	 			 size = sprintf(buffer, "%d,%d,%d;\n\r",(int)J_z,BH1750_lux_int_1,wyp);
	 			 HAL_UART_Transmit_IT(&huart3, buffer, size);
	 			 HAL_Delay(10);
	 		 }
	 	  }

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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//! przerwanie od USARTA sterujące tymczasowo diodami na płytce
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{

	char value1[3];

	int value_i1;

	value1[0] = msg[0];
	value1[1] = msg[1];
	value1[2] = msg[2];

	value_i1 = 10*(atoi(value1));

	if(atoi(value1) <=100)
	{
		J_z=value_i1;
	}

	if(atoi(value1) == 999) //Night mode on
	{
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		NightMode=4;
		wyp=__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1);
		size=sprintf(buffer,"NIGHT MODE ON\n\r");
		HAL_UART_Transmit_IT(&huart3, buffer, size);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, wyp);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, wyp*NightMode/10);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, wyp*NightMode/10);
	}
	if(atoi(value1) == 998) //Night mode off
	{
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		NightMode=10;
		wyp=__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1);
		size=sprintf(buffer,"NIGHT MODE OFF\n\r");
		HAL_UART_Transmit_IT(&huart3, buffer, size);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, wyp);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, wyp*NightMode/10);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, wyp*NightMode/10);
	}
	if(atoi(value1) == 997) //State check
	{
		if(BH1750_OK == BH1750_ReadLight(&BH1750_lux_1))
		{
			BH1750_lux_int_1 = BH1750_lux_1;
		}
		int state=__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1);
		size = sprintf(buffer, "%d, %d;\n\r", BH1750_lux_int_1,state);
		HAL_UART_Transmit_IT(&huart3,buffer, size);
	}
	if(atoi(value1) == 996) //State check
		{
			pom^=1;
		}
	if(atoi(value1) >100 && atoi(value1) < 996)
	{
		value_i1=__HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1);
		size=sprintf(buffer,"WRONG VALUE\n\r");
		HAL_UART_Transmit_IT(&huart3, buffer, size);
	}

HAL_UART_Receive_IT(&huart3, msg, 3);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
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
