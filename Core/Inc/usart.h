/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define UART_RECEIVE_SIZE 520
#define UPGRADE_STATE_START_LENGTH 6 // strlen("FWUG") + 2(0x0D 0x0A)
#ifdef _DEBUG_FLOW
#define UPGRADE_STATE_PROC_LENGTH 16 // 2bytes of data length + data(<=(UPGRADE_STATE_PROC_LENGTH-4)bytes) + 2(0x0D 0x0A)
#else
#define UPGRADE_STATE_PROC_LENGTH 516 // 2bytes of data length + data(<=(UPGRADE_STATE_PROC_LENGTH-4)bytes) + 2(0x0D 0x0A)
#endif
#define UPGRADE_STATE_END_LENGTH 10 // strlen("FWED") + 4bytes of checksum + 2(0x0D 0x0A)
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
/* Timeout for HAL. */
#define UART_TIMEOUT HAL_MAX_DELAY //((uint16_t)1000u)

/* Status report for the functions. */
typedef enum {
  UART_OK     = 0x00u, /**< The action was successful. */
  UART_ERROR  = 0x01u  /**< Generic error. */
} uart_status;

uart_status uart_receive(uint8_t *data, uint16_t length);
uart_status uart_transmit_str(uint8_t *data);
uart_status uart_transmit_ch(uint8_t data);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
