#include "main.h" 
#include "raceup_fdcan.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <cstring>

// ------------------------------------------------------ Data Structures

/// @brief Structure to hold CAN message data in the FreeRTOS Queue
typedef struct {
  uint16_t id;
  uint8_t data[8];
  uint8_t len;
} CanRxMessage_t;

// ------------------------------------------------------ Function Prototypes
static void BlinkTask(void *arg);
static void CanTxTask(void *arg);
static void CanRxTask(void *arg);
static void CanRxCallback(uint16_t id, uint8_t* data, uint8_t len);
static void BlinkGPIO(GPIO_TypeDef* bank, uint16_t pin, uint32_t duration);

// ------------------------------------------------------ FreeRTOS Static Allocations

// 1. Blink Task
static StackType_t blinkTaskStack[256];
static StaticTask_t blinkTaskTcb;

// 2. Tx Task
static StackType_t txTaskStack[256];
static StaticTask_t txTaskTcb;

// 3. Rx Task
static StackType_t rxTaskStack[256];
static StaticTask_t rxTaskTcb;

// 4. Rx Queue (Holds up to 10 incoming CAN messages)
#define RX_QUEUE_LENGTH 10
#define RX_QUEUE_ITEM_SIZE sizeof(CanRxMessage_t)
static uint8_t rxQueueStorage[RX_QUEUE_LENGTH * RX_QUEUE_ITEM_SIZE];
static StaticQueue_t rxQueueStruct;
static QueueHandle_t rxQueueHandle;

// ------------------------------------------------------ Application Entry
void app_start(void) {
  // 1. Create the FreeRTOS Queue for Rx messages
  rxQueueHandle = xQueueCreateStatic(RX_QUEUE_LENGTH, RX_QUEUE_ITEM_SIZE, rxQueueStorage, &rxQueueStruct);

  // 2. Configure and Initialize FDCAN
  RUP_FDCAN_BitTimingTypeDef can_timing = {
    .presc = 4, .ts1 = 8, .ts2 = 1, .sjw = 1
  };
  RUP_FDCAN_Init(FDCAN1, can_timing, RUP_FDCAN_ACCEPT_IN_RX_FIFO0, RUP_FDCAN_IT_RX_FIFO0);

  // 3. Register Callback and Start FDCAN
  RUP_FDCAN_RegisterRxFIFO0Callback(FDCAN1, CanRxCallback);
  RUP_FDCAN_Start(FDCAN1);

  // 4. Create Tasks
  // xTaskCreateStatic(BlinkTask, "BlinkTask", 256, NULL, tskIDLE_PRIORITY + 1, blinkTaskStack, &blinkTaskTcb);
  xTaskCreateStatic(CanTxTask, "CanTxTask", 256, NULL, tskIDLE_PRIORITY + 1, txTaskStack, &txTaskTcb);
  
  // Rx task given slightly higher priority so it preempts Tx/Blink to process incoming data immediately
  xTaskCreateStatic(CanRxTask, "CanRxTask", 256, NULL, tskIDLE_PRIORITY + 2, rxTaskStack, &rxTaskTcb);
}

// ------------------------------------------------------ FDCAN Rx Callback (ISR Context)
static void CanRxCallback(uint16_t id, uint8_t* data, uint8_t len) {
  // NOTE: This runs in the hardware interrupt context!
  
  CanRxMessage_t msg;
  msg.id = id;
  msg.len = len;
  std::memcpy(msg.data, data, len); // Copy data before leaving callback

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  
  // Safely send data to the queue from an ISR
  xQueueSendFromISR(rxQueueHandle, &msg, &xHigherPriorityTaskWoken);

  // Yield if waking the Rx task requires a context switch
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// ------------------------------------------------------ Task Implementations

static void BlinkTask(void *arg) {
  for (;;) {
    BlinkGPIO(GPIOE, GPIO_PIN_3, 300); // Main LED
    vTaskDelay(pdMS_TO_TICKS(700));
  }
}

static void CanTxTask(void *arg) {
  uint8_t tx_data[8] = {0xDE, 0xAD, 0xBE, 0xEF, 0x11, 0x22, 0x33, 0x44};

  for (;;) {
    // Send a CAN frame every 1000ms
    RUP_FDCAN_Send(FDCAN1, 0x123, tx_data, 8);
    // BlinkGPIO(GPIOE, GPIO_PIN_3, 100); // Main LED
    vTaskDelay(pdMS_TO_TICKS(900));
  }
}

static void CanRxTask(void *arg) {
  CanRxMessage_t received_msg;

  for (;;) {
    // Block indefinitely until a message arrives in the queue
    if (xQueueReceive(rxQueueHandle, &received_msg, portMAX_DELAY) == pdTRUE) {
      // Process the received message here safely in a task context!
      
      // Example: Toggle the secondary LED on every message received
      BlinkGPIO(GPIOE, GPIO_PIN_3, 100); // Main LED
      
      // You can now safely check received_msg.id or received_msg.data
      // without stalling the FDCAN peripheral.
    }
  }
}

static void BlinkGPIO(GPIO_TypeDef* bank, uint16_t pin, uint32_t duration) {
  // Turn the LED ON (Assumes active-high; swap SET/RESET if active-low)
  HAL_GPIO_WritePin(bank, pin, GPIO_PIN_SET);
  
  // Keep it on for the requested duration, yielding the CPU to other tasks
  vTaskDelay(pdMS_TO_TICKS(duration));
  
  // Turn the LED OFF
  HAL_GPIO_WritePin(bank, pin, GPIO_PIN_RESET);
}
