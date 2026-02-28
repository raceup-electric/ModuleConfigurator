#include "main.h" 
#include "raceup_fdcan.h"
#include "raceup_setup.h"
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

// Task Prototypes generated from config.yaml
static void StartDefaultTask(void *arg);
static void StartCanRxTask(void *arg);
static void StartCanTxTask(void *arg);

// FDCAN Rx Callback Prototypes
static void Fdcan1RxCallback(uint16_t id, uint8_t* data, uint8_t len);

static void BlinkGPIO(GPIO_TypeDef* bank, uint16_t pin, uint32_t duration);

// ------------------------------------------------------ FreeRTOS Static Allocations

// Task Stacks and TCBs generated from config.yaml
static StackType_t default_taskStack[256];
static StaticTask_t default_taskTcb;


static StackType_t can_rx_taskStack[512];
static StaticTask_t can_rx_taskTcb;


static StackType_t can_tx_taskStack[512];
static StaticTask_t can_tx_taskTcb;



// Rx Queue (Holds up to 10 incoming CAN messages)
#define RX_QUEUE_LENGTH 10
#define RX_QUEUE_ITEM_SIZE sizeof(CanRxMessage_t)
static uint8_t rxQueueStorage[RX_QUEUE_LENGTH * RX_QUEUE_ITEM_SIZE];
static StaticQueue_t rxQueueStruct;
static QueueHandle_t rxQueueHandle;

// ------------------------------------------------------ Application Entry
void app_start(void) {
  // 1. Create the FreeRTOS Queue for Rx messages
  rxQueueHandle = xQueueCreateStatic(RX_QUEUE_LENGTH, RX_QUEUE_ITEM_SIZE, rxQueueStorage, &rxQueueStruct);
  
  config_FDCAN();
  config_GPIO();

  // 3. Create Tasks dynamically
  xTaskCreateStatic(StartDefaultTask, "default_task", 256, NULL, 3, default_taskStack, &default_taskTcb);
  xTaskCreateStatic(StartCanRxTask, "can_rx_task", 512, NULL, 5, can_rx_taskStack, &can_rx_taskTcb);
  xTaskCreateStatic(StartCanTxTask, "can_tx_task", 512, NULL, 5, can_tx_taskStack, &can_tx_taskTcb);
}

// ------------------------------------------------------ FDCAN Rx Callbacks (ISR Context)
static void Fdcan1RxCallback(uint16_t id, uint8_t* data, uint8_t len) {
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
static void StartDefaultTask(void *arg) {
  // Default Task Loop
  for (;;) {
    BlinkGPIO(GPIOE, GPIO_PIN_3, 300); // Main LED
    vTaskDelay(pdMS_TO_TICKS(700));
  }
}


static void StartCanRxTask(void *arg) {
  CanRxMessage_t received_msg;

  for (;;) {
    // Block indefinitely until a message arrives in the queue
    if (xQueueReceive(rxQueueHandle, &received_msg, portMAX_DELAY) == pdTRUE) {
      // Process the received message here safely in a task context!
      
      // Example: You can now safely check received_msg.id or received_msg.data
      // without stalling the FDCAN peripheral.
    }
  }
}


static void StartCanTxTask(void *arg) {
  uint8_t tx_data[8] = {0xDE, 0xAD, 0xBE, 0xEF, 0x11, 0x22, 0x33, 0x44};

  for (;;) {
    // Send a CAN frame every 1000ms
    RUP_FDCAN_Send(FDCAN1, 0x123, tx_data, 8);
    // BlinkGPIO(GPIOE, GPIO_PIN_3, 100); // Main LED
    vTaskDelay(pdMS_TO_TICKS(900));
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