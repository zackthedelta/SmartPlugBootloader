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
#define FLASH_BOOT_END_ADDRESS   ((uint32_t)0x08003FFFu)

#define FLASH_APP_START_ADDRESS ((uint32_t)0x08004000u)
#define FLASH_APP_END_ADDRESS   ((uint32_t)0x0800FBFFu)

#define UPGRADE_ID ((uint32_t)0xAABBCCDDu)
#define UPGRADE_ID_ERASE_ADDRESS ((uint32_t)0x0800FC00u)
#define UPGRADE_ID_ADDRESS ((uint32_t)0x0800FC00u)

#define DONT_UPGRADE_ID ((uint32_t)0xFFFFFFFFu)

//=================== 0x08000000
//Bootloader //16KB
//=================== 0x08004000
//APP //46KB
//=================== 0x0800FC00
//Reserve //1KB
//=================== 0x0800FFFF


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
void flash_back_to_bootloader(void);

#endif /* FLASH_H_ */
