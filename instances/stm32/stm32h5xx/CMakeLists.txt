# STM32H5xx family configuration

message(STATUS "Configuring STM32H5xx family")

set(MCPU
  cortex-m33
)

set(FREERTOS_PORT_DIR
  ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM33_NTZ/non_secure
)

set(FREERTOS_KERNEL_SOURCES
  ${FREERTOS_KERNEL_DIR}/list.c
  ${FREERTOS_KERNEL_DIR}/queue.c
  ${FREERTOS_KERNEL_DIR}/tasks.c
  ${FREERTOS_KERNEL_DIR}/timers.c
  ${FREERTOS_KERNEL_DIR}/event_groups.c
  ${FREERTOS_KERNEL_DIR}/stream_buffer.c
)

set(FREERTOS_PORT_SOURCES
  ${FREERTOS_PORT_DIR}/port.c
)

# Conditionally add portasm.c if it exists
if(EXISTS ${FREERTOS_PORT_DIR}/portasm.c)
  list(APPEND FREERTOS_PORT_SOURCES ${FREERTOS_PORT_DIR}/portasm.c)
endif()

set(FREERTOS_PORT_INCLUDE
  ${FREERTOS_PORT_DIR}
  ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM33/secure
  ${FREERTOS_KERNEL_DIR}/portable/ARMv8M/secure/context
)

set(FREERTOS_HEAP_FILE
  ${FREERTOS_KERNEL_DIR}/portable/MemMang/heap_4.c
)

# CMSIS Device
set(CMSIS_DEVICE_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/cmsis-device-h5/Include
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/STM32CubeH5/Drivers/CMSIS/Include/
)

set(CMSIS_GENERIC_INCLUDE_DIRS
  # CMSIS-Core headers should live here (core_cm33.h, etc.). Vendor if missing.
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/cmsis-device-h5/Source/Templates
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/STM32CubeH5/Drivers/CMSIS/Include/
)

set(CMSIS_DEVICE_SOURCES
  # ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/cmsis-device-h5/Source/Templates/system_stm32h5xx.c
  # They are the same, the one inside "Inc" is given by the MX project -> could it be easier to setup it there and just copy here the files?
  ${CMAKE_CURRENT_LIST_DIR}/Src/system_stm32h5xx.c
)

# STM32H5xx HAL
set(HAL_DEVICE_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Inc
  ${CMSIS_DEVICE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_LIST_DIR}/Inc
)

# Add HAL source files you need (example: common, GPIO, UART, etc.)
set(HAL_DEVICE_SOURCES
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal.c
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal_cortex.c
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal_flash.c
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal_flash_ex.c
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal_fdcan.c
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal_gpio.c
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal_rcc.c
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal_rcc_ex.c
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal_pwr.c
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal_pwr_ex.c
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/stm32h5xx-hal-driver/Src/stm32h5xx_hal_uart.c
  ${CMAKE_CURRENT_LIST_DIR}/Src/stm32h5xx_hal_msp.c
  ${CMAKE_CURRENT_LIST_DIR}/Src/stm32h5xx_it.c
  ${CMAKE_CURRENT_LIST_DIR}/Src/system_stm32h5xx.c
)

# Platform definitions
list(APPEND PLATFORM_DEFS
  USE_HAL_DRIVER
  FREERTOS_CPU_CLOCK_HZ=250000000
  FREERTOS_TICK_RATE_HZ=1000
  configUSE_TICK_HOOK=1    # DISABLE if not interested in using HAL tick functions
  # ARM CM33 port requirements
  configENABLE_FPU=1
  configENABLE_MPU=0
  configENABLE_TRUSTZONE=0
  configPRIO_BITS=4
  configLIBRARY_LOWEST_INTERRUPT_PRIORITY=15
  configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY=5
  # Precompute to avoid shell/CMake tokenization issues
  configMAX_SYSCALL_INTERRUPT_PRIORITY=80     # (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
)


set(INSTANCE_MAIN
  ${CMAKE_SOURCE_DIR}/instances/${INSTANCE}/main.cpp
)

set(PLATFORM_MAIN
  ${CMAKE_CURRENT_LIST_DIR}/Src/stm32h5xx_it.c
  ${CMAKE_CURRENT_LIST_DIR}/Src/freertos_impl.c
)

if(EXISTS ${INSTANCE_MAIN})
  list(PREPEND PLATFORM_MAIN ${INSTANCE_MAIN})
else()
  list(PREPEND PLATFORM_MAIN ${CMAKE_CURRENT_LIST_DIR}/Src/main.cpp)
endif()

set(PLATFORM_MAIN_INCDIR
  ${CMAKE_CURRENT_LIST_DIR}/Inc
)

set(DRIVER_SOURCES
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/common.cpp
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/adc.cpp
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/can.cpp
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/flash_memory.cpp
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/gpio.cpp
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/i2c.cpp
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/pwm.cpp
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/serial.cpp
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/spi.cpp
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/timer.cpp
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/raceup_fdcan.c
  ${CMAKE_SOURCE_DIR}/lib/drivers/instances/stm32h5xx/raceup_setup.c
)

set(GCC_CMSIS_DIR
  ${CMAKE_SOURCE_DIR}/third_party/STM32/STM32H5/cmsis-device-h5/Source/Templates/gcc/
)

set(LINKER_SCRIPT_FOLDER
  ${GCC_CMSIS_DIR}/linker/
)

# TODO: Define STARTUP_SCRIPT when ready
set(STARTUP_SCRIPT
  ${GCC_CMSIS_DIR}startup_${STM32_device}.s
)
