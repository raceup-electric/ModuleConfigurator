/**
 * @file raceup_fdcan.h
 * @author Luca Domeneghetti
 * @date 2026
 * @brief RaceUp Team FDCAN Wrapper Driver.
 * * This file contains the configuration, structures, and API definitions for the 
 * FDCAN peripheral wrapper. It simplifies the STM32 HAL FDCAN usage by providing 
 * specific bitrates, simplified filter management, and callback abstractions.
 *
 * @version 1.0
 */

#ifndef _RACEUP_FDCAN_H
#define _RACEUP_FDCAN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/** @addtogroup RaceUp_Drivers RaceUp Drivers
 * @brief Base group for all custom team drivers.
 * @{
 */

/** @defgroup RUP_FDCAN FDCAN Wrapper
 * @brief FDCAN abstraction layer for simplified communication.
 * @{
 */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief  FDCAN Wrapper Handle Structure.
 * @note   This structure extends the standard HAL handle to include custom 
 * callback function pointers and initialization state.
 */
typedef struct {
    FDCAN_HandleTypeDef hfdcan;     /*!< Underlying HAL FDCAN Handle */

    /**
     * @brief Callback for FIFO0 Rx events.
     * @param id   Standard CAN ID (11-bit).
     * @param data Pointer to the received data payload.
     * @param len  Length of the data (0-8 bytes).
     */
    void (*RxFIFO0Callback)(uint16_t id, uint8_t* data, uint8_t len);

    /**
     * @brief Callback for FIFO1 Rx events.
     * @param id   Standard CAN ID (11-bit).
     * @param data Pointer to the received data payload.
     * @param len  Length of the data (0-8 bytes).
     */
    void (*RxFIFO1Callback)(uint16_t id, uint8_t* data, uint8_t len);

    /**
     * @brief Callback for Error events.
     * @param error_flags Bitmask of error flags (see FDCAN_IT_xxx in HAL).
     */
    void (*ErrorCallback)(uint32_t error_flags);

    /**
     * @brief Callback for High Priority Message events.
     * @param hpStatus Pointer to the High Priority Message Status structure.
     */
    void (*HpCallback)(FDCAN_HpMsgStatusTypeDef* hpStatus);

    volatile uint8_t Initialized;   /*!< Flag indicating if the driver is initialized (1) or not (0) */

} RUP_FDCAN_HandleTypeDef;

/**
 * @brief  RaceUp FDCAN Status Enumeration.
 * @details simplified status codes to avoid exposing HAL-specific enums everywhere.
 */
typedef enum {
    RUP_FDCAN_OK       = 0x00U, /*!< Operation completed successfully */
    RUP_FDCAN_ERROR    = 0x01U, /*!< Operation failed */
    RUP_FDCAN_BUSY     = 0x02U, /*!< Peripheral is busy */
    RUP_FDCAN_TIMEOUT  = 0x03U  /*!< Operation timed out */
} RUP_FDCAN_StatusTypeDef;

// /**
//  * @brief  Supported Nominal Bitrates.
//  * @note   These bitrates are configured with specific hardcoded TQ values in the Init function.
//  * See @ref fdcan_usage for the exact prescaler and time segment values.
//  */
// typedef enum {
//   FDCAN_NOMINAL_BR_125,  /*!< 125 kBit/s (Presc: 10, Total TQ: 16) */
//   FDCAN_NOMINAL_BR_250,  /*!< 250 kBit/s (Presc: 5,  Total TQ: 16) */
//   FDCAN_NOMINAL_BR_500,  /*!< 500 kBit/s (Presc: 4,  Total TQ: 10) */
//   FDCAN_NOMINAL_BR_1000  /*!< 1 Mbit/s   (Presc: 2,  Total TQ: 10) */
// } RUP_FDCAN_NominalBitrate;

typedef struct {
  uint32_t presc;
  uint32_t ts1;
  uint32_t ts2;
  uint32_t sjw;
} RUP_FDCAN_BitTimingTypeDef;

/**
 * @brief  Rx FIFO Selection.
 * @note   Maps directly to HAL definitions.
 */
typedef enum {
    RUP_FDCAN_RX_FIFO0 = FDCAN_RX_FIFO0, /*!< Receiver FIFO 0 */
    RUP_FDCAN_RX_FIFO1 = FDCAN_RX_FIFO1  /*!< Receiver FIFO 1 */
} RUP_FDCAN_RxFifoTypeDef;

/**
 * @brief  Global Filter Configuration.
 * @details Defines behavior for messages that do NOT match any specific filter.
 */
typedef enum {
    RUP_FDCAN_ACCEPT_IN_RX_FIFO0 = FDCAN_ACCEPT_IN_RX_FIFO0, /*!< Accept non-matching frames into FIFO 0 */
    RUP_FDCAN_ACCEPT_IN_RX_FIFO1 = FDCAN_ACCEPT_IN_RX_FIFO1, /*!< Accept non-matching frames into FIFO 1 */
    RUP_FDCAN_REJECT             = FDCAN_REJECT,             /*!< Reject non-matching frames */
} RUP_FDCAN_GlobalFilterTypeDef;

/**
 * @brief  Filter Type Selection.
 * @details Defines how the ID1 and ID2 arguments in @ref RUP_FDCAN_AddFilter are interpreted.
 */
typedef enum {
    RUP_FDCAN_FILTER_RANGE = FDCAN_FILTER_RANGE, /*!< Range filter from ID1 to ID2 */
    RUP_FDCAN_FILTER_DUAL  = FDCAN_FILTER_DUAL,  /*!< Dual ID filter for ID1 or ID2 */
    RUP_FDCAN_FILTER_MASK  = FDCAN_FILTER_MASK   /*!< Classic mask mode: ID1 = ID, ID2 = Mask */
} RUP_FDCAN_FilterTypeTypeDef;

/**
 * @brief  Filter Configuration / Action.
 * @details Defines what happens when a message matches a filter.
 */
typedef enum {
    RUP_FDCAN_FILTER_TO_RXFIFO0    = FDCAN_FILTER_TO_RXFIFO0,    /*!< Store matching message in FIFO 0 */
    RUP_FDCAN_FILTER_TO_RXFIFO1    = FDCAN_FILTER_TO_RXFIFO1,    /*!< Store matching message in FIFO 1 */
    RUP_FDCAN_FILTER_REJECT        = FDCAN_FILTER_REJECT,        /*!< Reject matching message */
    RUP_FDCAN_FILTER_RXFIFO0_HP    = FDCAN_FILTER_TO_RXFIFO0_HP, /*!< Store in FIFO 0 as High Priority */
    RUP_FDCAN_FILTER_RXFIFO1_HP    = FDCAN_FILTER_TO_RXFIFO1_HP  /*!< Store in FIFO 1 as High Priority */
} RUP_FDCAN_FilterConfigTypeDef;

/**
 * @brief  Interrupt Enable Mode.
 * @details Bitmask to enable specific interrupts during initialization.
 */
typedef enum {
    RUP_FDCAN_IT_NONE      = 0x00U, /*!< No Rx Interrupts enabled */
    RUP_FDCAN_IT_RX_FIFO0  = 0x01U, /*!< Enable FIFO 0 New Message Interrupt */
    RUP_FDCAN_IT_RX_FIFO1  = 0x02U, /*!< Enable FIFO 1 New Message Interrupt */
    RUP_FDCAN_IT_ALL       = 0x03U  /*!< Enable both FIFO 0 and FIFO 1 Interrupts */
} RUP_FDCAN_RxItModeTypeDef;

/* Exported variables --------------------------------------------------------*/

/** @brief Global handle for FDCAN1 wrapper instance */
extern RUP_FDCAN_HandleTypeDef hRUCAN1;

#ifdef FDCAN2
/** @brief Global handle for FDCAN2 wrapper instance (if hardware supports it) */
extern RUP_FDCAN_HandleTypeDef hRUCAN2;
#endif

/* Exported functions --------------------------------------------------------*/
/** @defgroup RUP_FDCAN_Exported_Functions API Functions
 * @{
 */

/**
 * @brief  Initializes the FDCAN peripheral with RaceUp defaults.
 * @details Configures the FDCAN in Classic Mode, sets bit timings based on `br`,
 * configures global filters, and enables requested interrupts.
 * * @param  Instance       Pointer to FDCAN peripheral (FDCAN1 or FDCAN2).
 * @param  br             Nominal bitrate selection (125k, 250k, 500k, 1M).
 * @param  global_filter  Behavior for non-matching messages (Accept/Reject).
 * @param  rx_it_mode     Interrupts to enable (FIFO0, FIFO1, None, or All).
 * * @return RUP_FDCAN_OK on success, RUP_FDCAN_ERROR otherwise.
 * * @warning This function resets the peripheral. Any existing configuration is lost.
 */
RUP_FDCAN_StatusTypeDef RUP_FDCAN_Init(FDCAN_GlobalTypeDef *Instance, 
                                       RUP_FDCAN_BitTimingTypeDef br, 
                                       RUP_FDCAN_GlobalFilterTypeDef global_filter,
                                       RUP_FDCAN_RxItModeTypeDef rx_it_mode);

/**
 * @brief  Starts the FDCAN module.
 * @details Transitions the peripheral from Initialization mode to Normal Operation mode.
 * Must be called after filters are configured.
 * * @param  Instance  Pointer to FDCAN peripheral.
 * @return RUP_FDCAN_OK on success.
 */
RUP_FDCAN_StatusTypeDef RUP_FDCAN_Start(FDCAN_GlobalTypeDef *Instance);

/**
 * @brief  Configures a standard ID filter.
 * @note   Only Standard IDs (11-bit) are supported by this wrapper function.
 * Extended IDs are configured via HAL directly if needed.
 * * @param  Instance  Pointer to FDCAN peripheral.
 * @param  type      Filter mode (Range, Dual, or Mask).
 * @param  config    Action to take on match (FIFO assignment or Reject).
 * @param  id1       First ID (or ID in Mask mode, or ID1 in Dual mode).
 * @param  id2       Second ID (or Mask in Mask mode, or ID2 in Dual mode).
 * * @return RUP_FDCAN_OK on success.
 */
RUP_FDCAN_StatusTypeDef RUP_FDCAN_AddFilter(FDCAN_GlobalTypeDef *Instance,
    RUP_FDCAN_FilterTypeTypeDef type,
    RUP_FDCAN_FilterConfigTypeDef config,
    uint16_t id1,
    uint16_t id2);

/**
 * @brief  Waits for a message to arrive in the specified FIFO (Blocking).
 * @details Blocks execution until a message is available or timeout occurs.
 * * @param  Instance  Pointer to FDCAN peripheral.
 * @param  RxFifo    FIFO to poll (RUP_FDCAN_RX_FIFO0 or RUP_FDCAN_RX_FIFO1).
 * @param  Timeout   Timeout in milliseconds (or HAL_MAX_DELAY).
 * * @return RUP_FDCAN_OK if message available, RUP_FDCAN_TIMEOUT if time expired.
 */
RUP_FDCAN_StatusTypeDef RUP_FDCAN_PollRxMessage(FDCAN_GlobalTypeDef *Instance,
    RUP_FDCAN_RxFifoTypeDef RxFifo,
    uint32_t Timeout);

/**
 * @brief  Reads a message from the FIFO and triggers the registered callback.
 * @details Use this in polling mode after @ref RUP_FDCAN_PollRxMessage returns OK,
 * or rely on the IRQ handler which calls this internally.
 * * @param  Instance  Pointer to FDCAN peripheral.
 * @param  RxFifo    FIFO to read from.
 * * @return RUP_FDCAN_OK on success.
 */
RUP_FDCAN_StatusTypeDef RUP_FDCAN_ReadRxMessage(FDCAN_GlobalTypeDef *Instance,
    RUP_FDCAN_RxFifoTypeDef RxFifo);

/**
 * @brief  Registers a custom callback for FIFO 0 Rx events.
 * * @param  Instance  Pointer to FDCAN peripheral.
 * @param  Callback  Function pointer to the user handler.
 */
void RUP_FDCAN_RegisterRxFIFO0Callback(FDCAN_GlobalTypeDef *Instance,
    void (*Callback)(uint16_t id, uint8_t* data, uint8_t len));

/**
 * @brief  Registers a custom callback for FIFO 1 Rx events.
 * * @param  Instance  Pointer to FDCAN peripheral.
 * @param  Callback  Function pointer to the user handler.
 */
void RUP_FDCAN_RegisterRxFIFO1Callback(FDCAN_GlobalTypeDef *Instance,
    void (*Callback)(uint16_t id, uint8_t* data, uint8_t len));

/**
 * @brief  Registers a custom callback for Error events.
 * * @param  Instance  Pointer to FDCAN peripheral.
 * @param  Callback  Function pointer to the user error handler.
 */
void RUP_FDCAN_RegisterErrorCallback(FDCAN_GlobalTypeDef *Instance,
    void (*Callback)(uint32_t error_flags));

/**
 * @brief  Registers a custom callback for High Priority events.
 * * @param  Instance  Pointer to FDCAN peripheral.
 * @param  Callback  Function pointer to the user HP handler.
 */
void RUP_FDCAN_RegisterHpCallback(FDCAN_GlobalTypeDef *Instance,
    void (*Callback)(FDCAN_HpMsgStatusTypeDef* hpStatus));

/**
 * @brief  Sends a Standard ID CAN message.
 * @details Simplifies transmission by automatically handling header configuration.
 * * @param  Instance  Pointer to FDCAN peripheral.
 * @param  id        Standard CAN ID (11-bit).
 * @param  data      Pointer to data buffer.
 * @param  len       Length of data (0 to 8 bytes).
 * * @return RUP_FDCAN_OK if added to Tx FIFO, RUP_FDCAN_ERROR otherwise.
 */
RUP_FDCAN_StatusTypeDef RUP_FDCAN_Send(FDCAN_GlobalTypeDef *Instance,
    uint16_t id,
    uint8_t* data,
    uint8_t len);

/** @} */ /* End of RUP_FDCAN_Exported_Functions */

/** @} */ /* End of RUP_FDCAN */

/** @} */ /* End of RaceUp_Drivers */

#ifdef __cplusplus
}
#endif

#endif /* _RACEUP_FDCAN_H */
