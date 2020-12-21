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
#define FLASH_APP_START_ADDRESS ((uint32_t)0x08000000u)
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
  char version_info[64] = {0};
  memset(version_info, '\0', sizeof(char)*64);
  sprintf(version_info, "Firmware Version...%s,%s%s\r\n", FW_VERSION, __DATE__, __TIME__);
  uart_transmit_str((uint8_t*)version_info);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // [UART]
  uart_status comm_status = UART_OK;
  uint8_t *uart_receive_data = NULL;
  uart_receive_data = (uint8_t *)malloc(sizeof(uint8_t)*UART_RECEIVE_SIZE);
  // [FLASH]
  flash_status flashop_status = FLASH_OK;
  uint32_t upgrade_id_address = UPGRADE_ID_ADDRESS;
  uint32_t upgrade_id_erase_address = UPGRADE_ID_ERASE_ADDRESS;
  uint32_t upgrade_id = UPGRADE_ID;
  uint32_t upgrade_checker = 0;
  // [TEST]
  uint32_t upgrade_id_debug = 0xAABBCCDDu;

  // [MAIN LOOP]
  while (1)
  {
	  upgrade_checker = (uint32_t)(*(uint32_t *)(upgrade_id_address));
	  DEBUG("upgrade_checker = 0x%08x\r\n", upgrade_checker);
	  if (upgrade_checker == upgrade_id)
	  {
		  printf("READY_TO_UPGRADE\r\n");
		  // [TEST]
		  while (1)
		  {
			  memset(uart_receive_data, '\0', sizeof(uint8_t)*UART_RECEIVE_SIZE);
			  comm_status = uart_receive(uart_receive_data, 1);
			  DEBUG("uart_receive_data %s, comm_status = %x\r\n", (char *)uart_receive_data, (char *)comm_status);
			  if (uart_receive_data[0] == 'q')
			  {
				  flashop_status = flash_erase(upgrade_id_erase_address);
				  DEBUG("flash_erase = %d\r\n", (int)flashop_status);
				  flashop_status = flash_write(upgrade_id_address, (uint32_t*)&upgrade_id_debug, 1);
				  DEBUG("flash_write = %d\r\n", (int)flashop_status);
				  break;
			  }
		  }
		  // TODO >> receive the FW from uart and write it in the upload buffer
		  //         if there is any mistake, response "FAIL" and flash_jump_to_app()
		  //         if not, I will response with "OK" for every transmission of package
		  //         I expect to receive "FWUGRD" (fireware_upgrade_ready) to start the upgrade flow, and the following packages would be the FW
		  //         (the host can identify "READY_TO_UPGRADE" or "DONT_NEED_TO_UPGRADE" outputs from bootloader and send "FWUGRD")
		  //         currently, total size of FW is about 24KB, try to design the package size with 512bytes (about 48~50 packages)
		  //         each package needs two bytes of checksum and two bytes of data length, and ends with 0x0D(\r) 0x0A(\n), so the remaining size for data will be 506bytes
		  //         (0xAA(checksum MSB), 0xBB(checksum LSB), 0xCC(data length MSB), 0xDD(data length LSB), 0xEE(fw), ....., 0xFF(fw), 0x0D(end), 0x0A(end))
		  //         if the data length is less than 506bytes(equal : not the last, larger : FAIL), I would know it's the last package of FW
		  //         after the last package of FW is received, I would expect the following final package is the total checksum(two bytes) to verify the whole FW again
		  //         0xAA(total checksum MSB), 0xBB(total checksum LSB), 0x0D(end), 0x0A(end)
		  //         and just like the previous transmission, I would response "OK", the upgrade is finished
		  //         so according to the above mentioned, I need to check the checksum and data length of each package
		  //         after check the whole checksum, just copy the FW in the upload buffer to app flash block, and than flash_jump_to_app()
	  }
	  else
	  {
		  printf("DONT_NEED_TO_UPGRADE\r\n");
		  // [TEST]
		  while (1)
		  {
			  memset(uart_receive_data, '\0', sizeof(uint8_t)*UART_RECEIVE_SIZE);
			  comm_status = uart_receive(uart_receive_data, 1);
			  DEBUG("uart_receive_data %s, comm_status = %x\r\n", (char *)uart_receive_data, (char *)comm_status);
			  if (uart_receive_data[0] == 'q')
			  {
				  flashop_status = flash_erase(upgrade_id_erase_address);
				  DEBUG("flash_erase = %d\r\n", (int)flashop_status);
				  flashop_status = flash_write(upgrade_id_address, (uint32_t*)&upgrade_id, 1);
				  DEBUG("flash_write = %d\r\n", (int)flashop_status);
				  flash_jump_to_app();
			  }
		  }
		  // TODO >> need a flash block for app, and another one for the uploading buffer of upgrade flow,
		  //         if don't need to upgrade, just flash_jump_to_app()
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	// [TEST]
	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_SET)
	{
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	}
	else
	{
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	}

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
