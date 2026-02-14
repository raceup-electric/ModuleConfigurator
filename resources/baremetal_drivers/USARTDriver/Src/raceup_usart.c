/**
 * @file raceup_usart.c
 * @author RaceUp Team
 * @date 2026
 * @brief Implementation of the RaceUp USART Wrapper.
 * @details
 * This file implements the abstraction layer for the STM32H5 USART peripheral.
 * It manages hardware instance mapping, interrupt routing, and stdio redirection.
 */

#include "raceup_usart.h"
#include <string.h>

/* Private Variables ---------------------------------------------------------*/

/** @brief Wrapper handle for USART1 */
RUP_USART_HandleTypeDef hRUSART1;

#ifdef USART2
/** @brief Wrapper handle for USART2 */
RUP_USART_HandleTypeDef hRUSART2;
#endif

#ifdef USART3
/** @brief Wrapper handle for USART3 */
RUP_USART_HandleTypeDef hRUSART3;
#endif

/** @brief Pointer to the UART instance used for printf */
static USART_TypeDef* StdioUART = NULL;

/* Private Function Prototypes -----------------------------------------------*/

/**
 * @brief  Maps STM32 HAL status codes to RaceUp wrapper status codes.
 */
static RUP_USART_StatusTypeDef Map_HAL_Status(HAL_StatusTypeDef status) {
    switch (status) {
        case HAL_OK:      return RUP_USART_OK;
        case HAL_BUSY:    return RUP_USART_BUSY;
        case HAL_TIMEOUT: return RUP_USART_TIMEOUT;
        case HAL_ERROR:   
        default:          return RUP_USART_ERROR;
    }
}

/**
 * @brief  Retrieves the RaceUp Wrapper Handle associated with a hardware instance.
 */
static RUP_USART_HandleTypeDef* GetHandle(USART_TypeDef *Instance) {
    if (Instance == USART1) return &hRUSART1;
#ifdef USART2
    if (Instance == USART2) return &hRUSART2;
#endif
#ifdef USART3
    if (Instance == USART3) return &hRUSART3;
#endif
    return NULL;
}

/* Public Function Implementation --------------------------------------------*/

RUP_USART_StatusTypeDef RUP_USART_Init(USART_TypeDef *Instance, uint32_t baudrate) {
    RUP_USART_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper) return RUP_USART_ERROR;

    // 1. Basic Initialization Parameters
    hWrapper->huart.Instance = Instance;
    hWrapper->huart.Init.BaudRate = baudrate;
    hWrapper->huart.Init.WordLength = UART_WORDLENGTH_8B;
    hWrapper->huart.Init.StopBits = UART_STOPBITS_1;
    hWrapper->huart.Init.Parity = UART_PARITY_NONE;
    hWrapper->huart.Init.Mode = UART_MODE_TX_RX;
    hWrapper->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hWrapper->huart.Init.OverSampling = UART_OVERSAMPLING_16;
    hWrapper->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hWrapper->huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    hWrapper->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    // 2. Initialize Hardware (MSP Init must be provided by user/CubeMX)
    RUP_USART_StatusTypeDef status = Map_HAL_Status(HAL_UART_Init(&hWrapper->huart));
    
    if (status == RUP_USART_OK) {
        hWrapper->Initialized = 1;
        // Set as default Stdio if not yet set
        if (StdioUART == NULL) {
            StdioUART = Instance;
        }
    }

    return status;
}

RUP_USART_StatusTypeDef RUP_USART_Start(USART_TypeDef *Instance) {
    RUP_USART_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper || !hWrapper->Initialized) return RUP_USART_ERROR;

    // Start reception in Interrupt mode (1 byte at a time)
    return Map_HAL_Status(HAL_UART_Receive_IT(&hWrapper->huart, hWrapper->RxBuffer, 1));
}

RUP_USART_StatusTypeDef RUP_USART_Send(USART_TypeDef *Instance, uint8_t* data, uint16_t len) {
    RUP_USART_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (!hWrapper) return RUP_USART_ERROR;

    // Blocking transmission
    return Map_HAL_Status(HAL_UART_Transmit(&hWrapper->huart, data, len, HAL_MAX_DELAY));
}

void RUP_USART_RegisterRxCallback(USART_TypeDef *Instance, void (*Callback)(uint8_t* data, uint16_t len)) {
    RUP_USART_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (hWrapper) {
        hWrapper->RxCallback = Callback;
    }
}

void RUP_USART_RegisterErrorCallback(USART_TypeDef *Instance, void (*Callback)(uint32_t error_flags)) {
    RUP_USART_HandleTypeDef *hWrapper = GetHandle(Instance);
    if (hWrapper) {
        hWrapper->ErrorCallback = Callback;
    }
}

void RUP_USART_SetStdioInstance(USART_TypeDef *Instance) {
    StdioUART = Instance;
}

/* HAL Callback Overrides ----------------------------------------------------*/

/**
 * @brief  Rx Transfer completed callback.
 * @note   Overrides the weak function in stm32h5xx_hal_uart.c
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    RUP_USART_HandleTypeDef *targetWrapper = GetHandle(huart->Instance);

    if (targetWrapper && targetWrapper->Initialized) {
        // Invoke user callback if registered
        if (targetWrapper->RxCallback) {
            targetWrapper->RxCallback(targetWrapper->RxBuffer, 1);
        }

        // Re-arm interrupt for the next byte
        HAL_UART_Receive_IT(huart, targetWrapper->RxBuffer, 1);
    }
}

/**
 * @brief  UART error callback.
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    RUP_USART_HandleTypeDef *targetWrapper = GetHandle(huart->Instance);
    
    if (targetWrapper && targetWrapper->ErrorCallback) {
        targetWrapper->ErrorCallback(huart->ErrorCode);
    }
}

/* Stdio / Printf Redirection ------------------------------------------------*/

/**
 * @brief  Retargets the C library printf function to the UART.
 * @note   Works with GCC/STM32CubeIDE (Newlib).
 */
int _write(int file, char *ptr, int len) {
    (void)file; // Unused
    if (StdioUART != NULL) {
        RUP_USART_Send(StdioUART, (uint8_t*)ptr, (uint16_t)len);
        return len;
    }
    return 0;
}

/* Note: HAL_UART_MspInit is NOT implemented here to allow user configuration */
