#include "FreeRTOS.h"
#include "task.h"

#include <cstdio>

#include "gpio.hpp"
#include "serial.hpp"

using namespace ru::driver;

// ------------------------------------------------------ Device and Per-task context structure definition

/// @brief Context structure for the debug task: GPIO led debug and Serial.
typedef struct DebugTaskContext {
  ru::driver::Gpio *led;
  ru::driver::Serial *serial;
} DebugTaskContext;

/// @brief Device context structure: contains all peripherals.
typedef struct DeviceContext {
  ru::driver::Gpio led;
  ru::driver::Serial serial;
} DeviceContext;

// ------------------------------------------------------ Global variables


// ------------------------------------------------------ Function prototypes
/// @brief Initializes the peripherals and stores them in the device context.
static void SetupPeripherals(DeviceContext* deviceContext);

// ------------------------------------------------------ FreeRTOS task definition
/// @brief Stack for the debug task.
static StackType_t debugTaskStack[1024];
/// @brief Task control block for the debug task.
static StaticTask_t debugTaskTcb;
/// @brief Debug task: toggles the led and writes a message to the serial every second.
static void DebugTask(void *arg);

// ------------------------------------------------------ Application entry point
void app_start(void) {
  // static to make it survive after the app_start() returns
  static DeviceContext deviceContext;
  // setup peripherals and store them in the device context
  SetupPeripherals(&deviceContext);

  // setup task context (static to survive after app_start() returns) and create the task
  static DebugTaskContext debugTaskContext = {
    .led = &deviceContext.led,
    .serial = &deviceContext.serial
  };
  xTaskCreateStatic(DebugTask, "app", 1024, &debugTaskContext, tskIDLE_PRIORITY + 1,
                    debugTaskStack, &debugTaskTcb);
}

// ------------------------------------------------------ Function definitions
static void SetupPeripherals(DeviceContext* deviceContext) {
  // configuration of the peripherals
  ru::driver::GpioConfig led_cfg(
      ru::driver::GpioId::debug_led,
      ru::driver::GpioFunction::output_pushpull_pullup,
      true,
      ru::driver::GpioSpeed::low);
  ru::driver::SerialConfig serial_cfg(ru::driver::SerialId::serial_debug, 115200);

  // initialization in the device context
  (void)deviceContext->led.init(led_cfg);
  (void)deviceContext->serial.init(serial_cfg);
}

// ------------------------------------------------------ FreeRTOS task definitions
static void DebugTask(void *arg) {
  auto* ctx = static_cast<DebugTaskContext*>(arg);
  const char msg[] = "debug: tick\n";

  for (;;) {
    (void)ctx->led->toggle();
    (void)ctx->serial->write(reinterpret_cast<const uint8_t *>(msg), sizeof(msg) - 1);
    std::printf("[app] %s", msg);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}