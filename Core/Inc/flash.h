/**
 * @file    flash.h
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module handles the memory related functions.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "stm32f1xx_hal.h"

/* Start and end addresses of the user application. */
#define FLASH_BOOT_START_ADDRESS ((uint32_t)0x08000000u)
#define FLASH_BOOT_END_ADDRESS   ((uint32_t)FLASH_BANK1_END-0x10u) /**< Leave a little extra space at the end. */

#define FLASH_APP_START_ADDRESS ((uint32_t)0x08000000u)
#define FLASH_APP_END_ADDRESS   ((uint32_t)FLASH_BANK1_END-0x10u) /**< Leave a little extra space at the end. */

#define UPGRADE_ID ((uint32_t)0xABCDEFFFu)
#define UPGRADE_ID_ERASE_ADDRESS ((uint32_t)0x08003800u)
#define UPGRADE_ID_ADDRESS ((uint32_t)0x08003FFCu) //use 16KBytes for bootloader (identify position : 0x3FFF - 0x0004)

/* Status report for the functions. */
typedef enum {
  FLASH_OK              = 0x00u, /**< The action was successful. */
  FLASH_ERROR_SIZE      = 0x01u, /**< The binary is too big. */
  FLASH_ERROR_WRITE     = 0x02u, /**< Writing failed. */
  FLASH_ERROR_READBACK  = 0x04u, /**< Writing was successful, but the content of the memory is wrong. */
  FLASH_ERROR           = 0xFFu  /**< Generic error. */
} flash_status;

flash_status flash_erase(uint32_t address);
flash_status flash_write(uint32_t address, uint32_t *data, uint32_t length);
void flash_jump_to_app(void);

#endif /* FLASH_H_ */
