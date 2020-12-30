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
#define FLASH_BOOT_END_ADDRESS   ((uint32_t)0x08005FFFu)
#define FLASH_BOOT_PAGES   ((FLASH_BOOT_END_ADDRESS-FLASH_BOOT_START_ADDRESS)/FLASH_PAGE_SIZE)+1

#define FLASH_APP1_START_ADDRESS ((uint32_t)0x08006000u)
#define FLASH_APP1_END_ADDRESS   ((uint32_t)0x08011FFFu)
#define FLASH_APP1_PAGES   ((FLASH_APP1_END_ADDRESS-FLASH_APP1_START_ADDRESS)/FLASH_PAGE_SIZE)+1

#define FLASH_APP2_START_ADDRESS ((uint32_t)0x08006000u)
#define FLASH_APP2_END_ADDRESS   ((uint32_t)0x08011FFFu)
#define FLASH_APP2_PAGES   ((FLASH_APP2_END_ADDRESS-FLASH_APP2_START_ADDRESS)/FLASH_PAGE_SIZE)+1

#define FLASH_UPGRADEINFO_START_ADDRESS ((uint32_t)0x0801FC00u)
#define FLASH_UPGRADEINFO_END_ADDRESS   ((uint32_t)0x0801FFFFu)
#define FLASH_UPGRADEINFO_PAGES   ((FLASH_UPGRADEINFO_END_ADDRESS-FLASH_UPGRADEINFO_START_ADDRESS)/FLASH_PAGE_SIZE)+1

#define UPGRADE_ID ((uint32_t)0xAABBCCDDu)

//=================== 0x08000000
//Bootloader //24KB
//=================== 0x08006000
//APP_1 //48KB
//=================== 0x08012000
//APP_2 //48KB
//=================== 0x0801E000
//Reserve //7KB
//=================== 0x0801FC00
//Upgrade info. //1KB
//=================== 0x0801FFFF

/* Status report for the functions. */
typedef enum {
  FLASH_OK              = 0x00u, /**< The action was successful. */
  FLASH_ERROR_SIZE      = 0x01u, /**< The binary is too big. */
  FLASH_ERROR_WRITE     = 0x02u, /**< Writing failed. */
  FLASH_ERROR_READBACK  = 0x04u, /**< Writing was successful, but the content of the memory is wrong. */
  FLASH_ERROR           = 0xFFu  /**< Generic error. */
} flash_status;

flash_status flash_erase(uint32_t address, uint32_t nbpages);
flash_status flash_write(uint32_t address, uint32_t *data, uint32_t length);
void flash_jump_to_app(void);
void flash_back_to_bootloader(void);

#endif /* FLASH_H_ */
