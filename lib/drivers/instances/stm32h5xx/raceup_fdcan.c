/**
 * @file raceup_fdcan.c
 * @author Luca Domeneghetti
 * @date 2026
 * @brief Implementation of the RaceUp FDCAN Wrapper.
 * * @details
 * This file implements the abstraction layer for the STM32H5 FDCAN peripheral.
 * It manages the mapping between hardware instances and software wrapper handles,
 * handles the translation of HAL status codes, and routes interrupts to user-registered
 * callbacks.
 * * **Key Implementation Details:**
 * - **Static Handles:** The driver uses static instances of `RUP_FDCAN_HandleTypeDef` 
 * to store state (callbacks, init flags) for FDCAN1 and FDCAN2.
 * - **Timing:** Bit timings are hardcoded in `RUP_FDCAN_Init` based on a specific 
 * PLL2Q clock configuration defined in `HAL_FDCAN_MspInit`.
 * - **Interrupt Routing:** The driver overrides the weak HAL callbacks (e.g., 
 * `HAL_FDCAN_RxFifo0Callback`) to look up the correct wrapper handle and execute 
 * the user's specific callback function.
 */

#include "raceup_fdcan.h"
#include "main.h"
#include <string.h>

/* Private Variables ---------------------------------------------------------*/

/** @brief Wrapper handle for FDCAN1 */
RUP_FDCAN_HandleTypeDef hRUCAN1;

#ifdef FDCAN2
/** @brief Wrapper handle for FDCAN2 (only if defined in hardware) */
RUP_FDCAN_HandleTypeDef hRUCAN2;
#endif

/* Private Function Prototypes -----------------------------------------------*/

/**
 * @brief  Maps STM32 HAL status codes to RaceUp wrapper status codes.
 * @internal
 * @param  status The HAL_StatusTypeDef returned by STM32 drivers.
 * @return The corresponding RUP_FDCAN_StatusTypeDef.
 */
static RUP_FDCAN_StatusTypeDef Map_HAL_Status(HAL_StatusTypeDef status) {
    switch (status) {
        case HAL_OK:
            return RUP_FDCAN_OK;
        case HAL_BUSY:
            return RUP_FDCAN_BUSY;
        case HAL_TIMEOUT:
            return RUP_FDCAN_TIMEOUT;
        case HAL_ERROR:
        default:
            return RUP_FDCAN_ERROR;
    }
}

/**
 * @brief  Retrieves the RaceUp Wrapper Handle associated with a hardware instance.
 * @internal
 * @param  Instance Pointer to the FDCAN hardware register base (FDCAN1/FDCAN2).
 * @return Pointer to the corresponding RUP_FDCAN_HandleTypeDef, or NULL if invalid.
 */
static RUP_FDCAN_HandleTypeDef* GetHandle(FDCAN_GlobalTypeDef *Instance) {
    if (Instance == FDCAN1) return &hRUCAN1;
#ifdef FDCAN2
    if (Instance == FDCAN2) return &hRUCAN2;
#endif
    return NULL;
}

/**
 * @brief  Converts a raw byte length (0-8) to the FDCAN DLC enum.
 * @internal
 * @note   FDCAN supports DLC > 8 bytes (up to 64) in FD mode, but this driver 
 * currently caps at 8 bytes for Classic CAN compatibility.
 * @param  len Data length in bytes.
 * @return The corresponding FDCAN_DLC_BYTES_x enum.
 */
static uint32_t Get_HAL_DLC(uint8_t len) {
    if (len > 8) len = 8;
    switch(len) {
        case 0: return FDCAN_DLC_BYTES_0;
        case 1: return FDCAN_DLC_BYTES_1;
        case 2: return FDCAN_DLC_BYTES_2;
        case 3: return FDCAN_DLC_BYTES_3;
        case 4: return FDCAN_DLC_BYTES_4;
        case 5: return FDCAN_DLC_BYTES_5;
        case 6: return FDCAN_DLC_BYTES_6;
        case 7: return FDCAN_DLC_BYTES_7;
        case 8: return FDCAN_DLC_BYTES_8;
        default: return FDCAN_DLC_BYTES_8;
    }
}

/**
 * @brief  Converts an FDCAN DLC enum back to a raw byte length.
 * @internal
 * @param  dlc The FDCAN_DLC_BYTES_x value from the RxHeader.
 * @return The length in bytes (0-8).
 */
static uint8_t Get_Len_From_DLC(uint32_t dlc) {
    switch(dlc) {
        case FDCAN_DLC_BYTES_0: return 0;
        case FDCAN_DLC_BYTES_1: return 1;
        case FDCAN_DLC_BYTES_2: return 2;
        case FDCAN_DLC_BYTES_3: return 3;
        case FDCAN_DLC_BYTES_4: return 4;
        case FDCAN_DLC_BYTES_5: return 5;
        case FDCAN_DLC_BYTES_6: return 6;
        case FDCAN_DLC_BYTES_7: return 7;
        case FDCAN_DLC_BYTES_8: return 8;
        default: return 8;
    }
}

/* Public Function Implementation --------------------------------------------*/

RUP_FDCAN_StatusTypeDef RUP_FDCAN_Init(FDCAN_GlobalTypeDef *Instance, 
                                       RUP_FDCAN_BitTimingTypeDef bt, 
                                       RUP_FDCAN_GlobalFilterTypeDef global_filter,
                                       RUP_FDCAN_RxItModeTypeDef rx_it_mode) {
  RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
  if (!hWrapper) return RUP_FDCAN_ERROR;

  // 1. Basic Initialization Parameters
  hWrapper->hfdcan.Instance = Instance;
  hWrapper->hfdcan.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hWrapper->hfdcan.Init.FrameFormat = FDCAN_FRAME_CLASSIC; // Forced Classic Mode
  hWrapper->hfdcan.Init.Mode = FDCAN_MODE_NORMAL;
  hWrapper->hfdcan.Init.AutoRetransmission = ENABLE;       // Enable auto-retry on error
  hWrapper->hfdcan.Init.TransmitPause = DISABLE;
  hWrapper->hfdcan.Init.ProtocolException = DISABLE;

  // Apply nominal timings (Arbitration Phase)
  hWrapper->hfdcan.Init.NominalPrescaler = bt.presc;
  hWrapper->hfdcan.Init.NominalSyncJumpWidth = bt.sjw;
  hWrapper->hfdcan.Init.NominalTimeSeg1 = bt.ts1;
  hWrapper->hfdcan.Init.NominalTimeSeg2 = bt.ts2;

  // Apply data timings (Data Phase - used in FD mode, mirrored here for consistency)
  hWrapper->hfdcan.Init.DataPrescaler = bt.presc;
  hWrapper->hfdcan.Init.DataSyncJumpWidth = bt.sjw;
  hWrapper->hfdcan.Init.DataTimeSeg1 = bt.ts1;
  hWrapper->hfdcan.Init.DataTimeSeg2 = bt.ts2;

  // 3. Configure Message RAM Limits
  hWrapper->hfdcan.Init.StdFiltersNbr = 28; // Max standard filters
  hWrapper->hfdcan.Init.ExtFiltersNbr = 0;
  hWrapper->hfdcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;

  // 4. Initialize Hardware
  RUP_FDCAN_StatusTypeDef status = Map_HAL_Status(HAL_FDCAN_Init(&hWrapper->hfdcan));
  if (status != RUP_FDCAN_OK) {
      return status;
  }

  // 5. Configure Global Filter (Acceptance of non-matching frames)
  if (HAL_FDCAN_ConfigGlobalFilter(&hWrapper->hfdcan,
                                   global_filter,
                                   global_filter,
                                   FDCAN_REJECT_REMOTE, // Remote Frames always rejected
                                   FDCAN_REJECT_REMOTE) != HAL_OK)
  {
    return RUP_FDCAN_ERROR;
  }

  // 6. Configure Interrupts
  uint32_t ActiveRxITs = 0;

  if (rx_it_mode & RUP_FDCAN_IT_RX_FIFO0) {
      ActiveRxITs |= FDCAN_IT_RX_FIFO0_NEW_MESSAGE;
  }
  
  if (rx_it_mode & RUP_FDCAN_IT_RX_FIFO1) {
      ActiveRxITs |= FDCAN_IT_RX_FIFO1_NEW_MESSAGE;
  }

  // High Priority Interrupts are enabled if any Rx interrupt is active
  if (ActiveRxITs != 0) {
      ActiveRxITs |= FDCAN_IT_RX_HIGH_PRIORITY_MSG;
  }

  // Map Rx Interrupts to Line 0
  if (HAL_FDCAN_ConfigInterruptLines(&hWrapper->hfdcan,
                                 ActiveRxITs,
                                 FDCAN_INTERRUPT_LINE0) != HAL_OK)
  {
      return RUP_FDCAN_ERROR;
  }

  // Map Error/Status Interrupts to Line 1
  if (HAL_FDCAN_ConfigInterruptLines(&hWrapper->hfdcan,
                                 FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_WARNING | FDCAN_IT_ERROR_PASSIVE,
                                 FDCAN_INTERRUPT_LINE1) != HAL_OK)
  {
      return RUP_FDCAN_ERROR;
  }

  // Activate Rx Notifications (Enable IRQ generation)
  if (HAL_FDCAN_ActivateNotification(&hWrapper->hfdcan,
                                 ActiveRxITs,
                                 0) != HAL_OK)
  {
      return RUP_FDCAN_ERROR;
  }

  // Activate Error Notifications
  if (HAL_FDCAN_ActivateNotification(&hWrapper->hfdcan,
                                 FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_WARNING | FDCAN_IT_ERROR_PASSIVE,
                                 0) != HAL_OK)
  {
      return RUP_FDCAN_ERROR;
  }
  
  hWrapper->Initialized = 1;
  return RUP_FDCAN_OK;
}

RUP_FDCAN_StatusTypeDef RUP_FDCAN_Start(FDCAN_GlobalTypeDef *Instance) {
    RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper) return RUP_FDCAN_ERROR;
    
    // Transitions the FDCAN from Initialization mode to Normal mode
    return Map_HAL_Status(HAL_FDCAN_Start(&hWrapper->hfdcan));
}

RUP_FDCAN_StatusTypeDef RUP_FDCAN_PollRxMessage(FDCAN_GlobalTypeDef *Instance, RUP_FDCAN_RxFifoTypeDef RxFifo, uint32_t Timeout) {
    RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper) return RUP_FDCAN_ERROR;

    uint32_t tickstart = HAL_GetTick();

    // Blocking loop waiting for Fill Level > 0
    while (HAL_FDCAN_GetRxFifoFillLevel(&hWrapper->hfdcan, RxFifo) == 0) {
        if (Timeout != HAL_MAX_DELAY) {
            if ((HAL_GetTick() - tickstart) > Timeout) {
                return RUP_FDCAN_TIMEOUT;
            }
        }
    }
    return RUP_FDCAN_OK;
}

RUP_FDCAN_StatusTypeDef RUP_FDCAN_ReadRxMessage(FDCAN_GlobalTypeDef *Instance, RUP_FDCAN_RxFifoTypeDef RxFifo) {
    RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper) return RUP_FDCAN_ERROR;

    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];
    
    // 1. Retrieve the message from hardware FIFO
    if (HAL_FDCAN_GetRxMessage(&hWrapper->hfdcan, RxFifo, &RxHeader, RxData) != HAL_OK) {
        return RUP_FDCAN_ERROR;
    }

    // 2. Dispatch to the appropriate callback
    // This allows manual polling loops to still trigger the registered logic
    if (RxFifo == RUP_FDCAN_RX_FIFO0) {
        if (hWrapper->RxFIFO0Callback != NULL) {
            hWrapper->RxFIFO0Callback((uint16_t)RxHeader.Identifier, RxData, Get_Len_From_DLC(RxHeader.DataLength));
        }
    } 
    else if (RxFifo == RUP_FDCAN_RX_FIFO1) {
        if (hWrapper->RxFIFO1Callback != NULL) {
            hWrapper->RxFIFO1Callback((uint16_t)RxHeader.Identifier, RxData, Get_Len_From_DLC(RxHeader.DataLength));
        }
    }

    return RUP_FDCAN_OK;
}

void RUP_FDCAN_RegisterRxFIFO0Callback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint16_t id, uint8_t* data, uint8_t len)) {
    RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (hWrapper) {
        hWrapper->RxFIFO0Callback = Callback;
    }
}

void RUP_FDCAN_RegisterRxFIFO1Callback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint16_t id, uint8_t* data, uint8_t len)) {
    RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (hWrapper) {
        hWrapper->RxFIFO1Callback = Callback;
    }
}

void RUP_FDCAN_RegisterErrorCallback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(uint32_t error_flags)) {
  RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
  if (hWrapper) {
    hWrapper->ErrorCallback = Callback;
  }
}

void RUP_FDCAN_RegisterHpCallback(FDCAN_GlobalTypeDef *Instance, void (*Callback)(FDCAN_HpMsgStatusTypeDef* hpStatus)) {
  RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
  if (hWrapper) {
    hWrapper->HpCallback = Callback;
  }
}

RUP_FDCAN_StatusTypeDef RUP_FDCAN_AddFilter(FDCAN_GlobalTypeDef *Instance, RUP_FDCAN_FilterTypeTypeDef type, RUP_FDCAN_FilterConfigTypeDef config, uint16_t id1, uint16_t id2) {
    RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper) return RUP_FDCAN_ERROR;

    FDCAN_FilterTypeDef sFilterConfig;
    static uint32_t filterIdx1 = 0; // Filter bank index for CAN1
#ifdef FDCAN2
    static uint32_t filterIdx2 = 0; // Filter bank index for CAN2
#endif //FDCAN2

    sFilterConfig.IdType = FDCAN_STANDARD_ID;
    sFilterConfig.FilterType = type;
    sFilterConfig.FilterConfig = config;
    sFilterConfig.FilterID1 = id1;
    sFilterConfig.FilterID2 = id2;
    
    // Automatically increment filter index to prevent overwrites
    if(Instance == FDCAN1) {
        sFilterConfig.FilterIndex = filterIdx1++;
    } 
#ifdef FDCAN2
    else if(Instance == FDCAN2) {
        sFilterConfig.FilterIndex = filterIdx2++;
    }
#endif

  return Map_HAL_Status(HAL_FDCAN_ConfigFilter(&hWrapper->hfdcan, &sFilterConfig));
}

RUP_FDCAN_StatusTypeDef RUP_FDCAN_Send(FDCAN_GlobalTypeDef *Instance, uint16_t id, uint8_t* data, uint8_t len) {
    RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper) return RUP_FDCAN_ERROR;

    FDCAN_TxHeaderTypeDef TxHeader;
    
    // Setup Tx Header for Standard ID / Classic Frame
    TxHeader.Identifier = id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = Get_HAL_DLC(len);
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF; // No bit rate switch in Classic Mode
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    return Map_HAL_Status(HAL_FDCAN_AddMessageToTxFifoQ(&hWrapper->hfdcan, &TxHeader, data));
}

void RUP_FDCAN_IRQHandler(FDCAN_GlobalTypeDef *Instance) {
    RUP_FDCAN_HandleTypeDef *hWrapper = GetHandle(Instance);
    
    if (hWrapper != NULL) {
        // Delegates to the generic HAL handler, which then calls 
        // the weak callbacks we have overridden below.
        HAL_FDCAN_IRQHandler(&hWrapper->hfdcan);
    }
}

/* HAL Callback Overrides ----------------------------------------------------*/

/**
 * @brief  HAL Callback for FIFO0 new message interrupt.
 * @note   Overrides the weak function in stm32h5xx_hal_fdcan.c.
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    RUP_FDCAN_HandleTypeDef *targetWrapper = GetHandle(hfdcan->Instance);

    // Verify wrapper exists and user has registered a callback
    if (targetWrapper && targetWrapper->RxFIFO0Callback) {
        if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0) {
            FDCAN_RxHeaderTypeDef RxHeader;
            uint8_t RxData[8];

            // Fetch data and invoke user callback
            if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
                targetWrapper->RxFIFO0Callback((uint16_t)RxHeader.Identifier, RxData, Get_Len_From_DLC(RxHeader.DataLength));
            }
        }
    }
}

/**
 * @brief  HAL Callback for FIFO1 new message interrupt.
 * @note   Overrides the weak function in stm32h5xx_hal_fdcan.c.
 */
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs) {
    RUP_FDCAN_HandleTypeDef *targetWrapper = GetHandle(hfdcan->Instance);

    if (targetWrapper && targetWrapper->RxFIFO1Callback) {
        if ((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != 0) {
            FDCAN_RxHeaderTypeDef RxHeader;
            uint8_t RxData[8];

            if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &RxHeader, RxData) == HAL_OK) {
                targetWrapper->RxFIFO1Callback((uint16_t)RxHeader.Identifier, RxData, Get_Len_From_DLC(RxHeader.DataLength));
            }
        }
    }
}

/**
 * @brief  HAL Callback for Error interrupts (Bus Off, Warning, Passive).
 */
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs) {
    RUP_FDCAN_HandleTypeDef *targetWrapper = GetHandle(hfdcan->Instance);

    if (targetWrapper && targetWrapper->ErrorCallback) {
        targetWrapper->ErrorCallback(ErrorStatusITs);
    }
}

/**
 * @brief  HAL Callback for High Priority Messages.
 */
void HAL_FDCAN_HighPriorityMessageCallback(FDCAN_HandleTypeDef *hfdcan) {
    RUP_FDCAN_HandleTypeDef *targetWrapper = GetHandle(hfdcan->Instance);

    if (targetWrapper && targetWrapper->HpCallback) {
        FDCAN_HpMsgStatusTypeDef hpStatus;

        if (HAL_FDCAN_GetHighPriorityMessageStatus(hfdcan, &hpStatus) == HAL_OK) {
            targetWrapper->HpCallback(&hpStatus);
        }
    }
}

/* Interrupt Service Routines ------------------------------------------------*/

/**
 * @brief  FDCAN1 Interrupt Line 0 Handler (Rx Events).
 */
void FDCAN1_IT0_IRQHandler(void)
{
  RUP_FDCAN_IRQHandler(FDCAN1);
}

/**
 * @brief  FDCAN1 Interrupt Line 1 Handler (Errors/Status).
 */
void FDCAN1_IT1_IRQHandler(void)
{
  RUP_FDCAN_IRQHandler(FDCAN1);
}

#ifdef FDCAN2
/**
 * @brief  FDCAN2 Interrupt Line 0 Handler (Rx Events).
 */
void FDCAN2_IT0_IRQHandler(void)
{
  RUP_FDCAN_IRQHandler(FDCAN2);
}

/**
 * @brief  FDCAN2 Interrupt Line 1 Handler (Errors/Status).
 */
void FDCAN2_IT1_IRQHandler(void)
{
  RUP_FDCAN_IRQHandler(FDCAN2);
}
#endif
