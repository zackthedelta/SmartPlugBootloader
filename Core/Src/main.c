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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "stm32f1xx_hal.h"
#include "flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* Function pointer for jumping to user application. */
typedef void (*fnc_ptr)(void);
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
#define UPGRADE_STATE_START_LENGTH 6 // strlen("FWUG") + 2(0x0D 0x0A)
#define UPGRADE_STATE_PROC_LENGTH 16 // 2bytes of data length + data(<= (UPGRADE_STATE_PROC_LENGTH-4)bytes) + 2(0x0D 0x0A)
#define UPGRADE_STATE_END_LENGTH 6 // strlen("FWED") + 2(0x0D 0x0A)
/* Status report for the functions. */
typedef enum {
  UPGRADE_STATE_START = 0x00u,
  UPGRADE_STATE_PROC = 0x01u,
  UPGRADE_STATE_END = 0x02u,
} upgrade_state;
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("Firmware Version...%s,%s%s\r\n", FW_VERSION, __DATE__, __TIME__);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // [UART]
  uint8_t *uart_receive_data = NULL;
  uart_receive_data = (uint8_t *)malloc(sizeof(uint8_t)*UART_RECEIVE_SIZE);
  // [FLASH]
  uint32_t upgrade_id = UPGRADE_ID;
  uint32_t upgrade_id_checker = 0;

  // [MAIN LOOP]
  upgrade_state upgrade_status = UPGRADE_STATE_START;
  while (1)
  {
	  upgrade_id_checker = (uint32_t)(*(uint32_t *)(UPGRADE_ID_ADDRESS));
	  DEBUG_INFO("upgrade_id_checker = 0x%08x\r\n", upgrade_id_checker);

	  if (upgrade_id_checker == UPGRADE_ID)
	  {
		  printf("READY_TO_UPGRADE\r\n");

		  while(1)
		  {
			  if (upgrade_status == UPGRADE_STATE_START)
			  {
				  memset(uart_receive_data, '\0', sizeof(uint8_t)*UART_RECEIVE_SIZE);
				  HAL_UART_Receive(&huart1, uart_receive_data, UPGRADE_STATE_START_LENGTH, UART_TIMEOUT);
				  DEBUG_INFO("[UPGRADE_STATE_START] uart_receive_data %s\r\n", (char *)uart_receive_data);

				  if (uart_receive_data[0] == 'F' && uart_receive_data[1] == 'W' && uart_receive_data[2] == 'U' && uart_receive_data[3] == 'G' && \
					  uart_receive_data[4] == '\r' && uart_receive_data[5] == '\n')
				  {
					  printf("OK\r\n");
					  flash_erase(FLASH_APP_START_ADDRESS);
					  upgrade_status = UPGRADE_STATE_PROC;
				  }
				  else
				  {
					  printf("FAIL\r\n");
					  flash_back_to_bootloader();
				  }
			  }
			  else if (upgrade_status == UPGRADE_STATE_PROC)
			  {
				  memset(uart_receive_data, '\0', sizeof(uint8_t)*UART_RECEIVE_SIZE);
				  HAL_UART_Receive(&huart1, uart_receive_data, UPGRADE_STATE_PROC_LENGTH, UART_TIMEOUT);
				  uint16_t data_length = ((((uint16_t)uart_receive_data[0])<<8) + ((uint16_t)uart_receive_data[1]));
				  DEBUG_INFO("[UPGRADE_STATE_PROC] uart_receive_data = %s, data_length = %d\r\n", (char *)uart_receive_data, (int)data_length);
				  DEBUG_INFO("[UPGRADE_STATE_PROC] uart_receive_data[(data_length + 2)] = %x, uart_receive_data[(data_length + 3)] = %x\r\n", uart_receive_data[(data_length + 2)], uart_receive_data[(data_length + 3)]);
				  if (data_length > (UPGRADE_STATE_PROC_LENGTH - 4) || uart_receive_data[(data_length + 2)] != '\r' || uart_receive_data[(data_length + 3)] != '\n')
				  {
					  printf("FAIL\r\n");
					  flash_erase(UPGRADE_ID_ERASE_ADDRESS);
					  flash_write(UPGRADE_ID_ADDRESS, (uint32_t*)&upgrade_id, 1);
					  flash_back_to_bootloader();
				  }
				  else if (data_length == (UPGRADE_STATE_PROC_LENGTH - 4))
				  {
					  flash_write(FLASH_APP_START_ADDRESS, (uint32_t*)&uart_receive_data[2], data_length);
					  printf("OK\r\n");
				  }
				  else if (data_length < (UPGRADE_STATE_PROC_LENGTH - 4))
				  {
					  flash_write(FLASH_APP_START_ADDRESS, (uint32_t*)&uart_receive_data[2], data_length);
					  printf("OK\r\n");
					  upgrade_status = UPGRADE_STATE_END;
				  }
			  }
			  else if (upgrade_status == UPGRADE_STATE_END)
			  {
				  memset(uart_receive_data, '\0', sizeof(uint8_t)*UART_RECEIVE_SIZE);
				  HAL_UART_Receive(&huart1, uart_receive_data, UPGRADE_STATE_END_LENGTH, UART_TIMEOUT);
				  DEBUG_INFO("[UPGRADE_STATE_END] uart_receive_data %s\r\n", (char *)uart_receive_data);

				  if (uart_receive_data[0] == 'F' && uart_receive_data[1] == 'W' && uart_receive_data[2] == 'E' && uart_receive_data[3] == 'D' && \
					  uart_receive_data[4] == '\r' && uart_receive_data[5] == '\n' )
				  {
					  printf("OK\r\n");
					  flash_back_to_bootloader(); //flash_jump_to_app();
				  }
				  else
				  {
					  printf("FAIL\r\n");
					  flash_erase(UPGRADE_ID_ERASE_ADDRESS);
					  flash_write(UPGRADE_ID_ADDRESS, (uint32_t*)&upgrade_id, 1);
					  flash_back_to_bootloader();
				  }
			  }
		  }
	  }
	  else
	  {
		  printf("DONT_NEED_TO_UPGRADE\r\n");
#ifdef _DEBUG
		  flash_erase(UPGRADE_ID_ERASE_ADDRESS);
		  flash_write(UPGRADE_ID_ADDRESS, (uint32_t*)&upgrade_id, 1);
#endif
		  flash_back_to_bootloader(); //flash_jump_to_app();
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	// TIME DELAY
	HAL_Delay(100);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
