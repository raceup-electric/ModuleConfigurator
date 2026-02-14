/**
 * @file raceup_usart.h
 * @author RaceUp Team
 * @date 2026
 * @brief RaceUp Team USART Wrapper Driver.
 * * This file contains the configuration, structures, and API definitions for the 
 * USART peripheral wrapper. It simplifies the STM32 HAL USART usage by providing 
 * simplified initialization, callback abstractions, and printf redirection.
 *
 * @version 1.0
 */

#ifndef _RACEUP_USART_H
#define _RACEUP_USART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
#include <stdio.h>

/** @addtogroup RaceUp_Drivers
 * @{
 */

/** @defgroup RUP_USART USART Wrapper
 * @brief USART abstraction layer for simplified communication and printf support.
 * @{
 */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief  USART Wrapper Handle Structure.
 * @note   Extends the standard HAL handle to include custom callback function pointers.
 */
typedef struct {
    UART_HandleTypeDef huart;       /*!< Underlying HAL UART Handle */

    /**
     * @brief Callback for Rx Event (Byte Received).
     * @param data Pointer to the received data (usually 1 byte).
     * @param len  Length of the data (usually 1).
     */
    void (*RxCallback)(uint8_t* data, uint16_t len);

    /**
     * @brief Callback for Error events.
     * @param error_flags Bitmask of error flags (see HAL_UART_ERROR_xxx).
     */
    void (*ErrorCallback)(uint32_t error_flags);

    volatile uint8_t Initialized;   /*!< Flag indicating if the driver is initialized */
    uint8_t RxBuffer[1];            /*!< Internal buffer for 1-byte interrupt reception */

} RUP_USART_HandleTypeDef;

/**
 * @brief  RaceUp USART Status Enumeration.
 */
typedef enum {
    RUP_USART_OK       = 0x00U,
    RUP_USART_ERROR    = 0x01U,
    RUP_USART_BUSY     = 0x02U,
    RUP_USART_TIMEOUT  = 0x03U
} RUP_USART_StatusTypeDef;

/* Exported variables --------------------------------------------------------*/

/** @brief Global handle for USART1 wrapper instance */
extern RUP_USART_HandleTypeDef hRUSART1;

#ifdef USART2
/** @brief Global handle for USART2 wrapper instance */
extern RUP_USART_HandleTypeDef hRUSART2;
#endif

#ifdef USART3
/** @brief Global handle for USART3 wrapper instance */
extern RUP_USART_HandleTypeDef hRUSART3;
#endif

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initializes the USART peripheral with RaceUp defaults (8N1).
 * @param  Instance Pointer to USART peripheral (USART1, USART2, etc.).
 * @param  baudrate Desired baud rate (e.g., 115200).
 * @return RUP_USART_OK on success.
 */
RUP_USART_StatusTypeDef RUP_USART_Init(USART_TypeDef *Instance, uint32_t baudrate);

/**
 * @brief  Starts the USART Reception (Interrupt Mode).
 * @details Enables the Rx Interrupt to trigger the registered RxCallback.
 * @param  Instance Pointer to USART peripheral.
 * @return RUP_USART_OK on success.
 */
RUP_USART_StatusTypeDef RUP_USART_Start(USART_TypeDef *Instance);

/**
 * @brief  Sends data via USART (Blocking).
 * @param  Instance Pointer to USART peripheral.
 * @param  data     Pointer to data buffer.
 * @param  len      Length of data.
 * @return RUP_USART_OK on success.
 */
RUP_USART_StatusTypeDef RUP_USART_Send(USART_TypeDef *Instance, uint8_t* data, uint16_t len);

/**
 * @brief  Registers a custom callback for Rx events.
 * @param  Instance Pointer to USART peripheral.
 * @param  Callback Function pointer to the user handler.
 */
void RUP_USART_RegisterRxCallback(USART_TypeDef *Instance, void (*Callback)(uint8_t* data, uint16_t len));

/**
 * @brief  Registers a custom callback for Error events.
 * @param  Instance Pointer to USART peripheral.
 * @param  Callback Function pointer to the user handler.
 */
void RUP_USART_RegisterErrorCallback(USART_TypeDef *Instance, void (*Callback)(uint32_t error_flags));

/**
 * @brief Sets the USART instance used for printf/stdio redirection.
 * @param Instance Pointer to USART peripheral.
 */
void RUP_USART_SetStdioInstance(USART_TypeDef *Instance);

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RACEUP_USART_H */
