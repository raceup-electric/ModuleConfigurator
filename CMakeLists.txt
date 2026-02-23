cmake_minimum_required(VERSION 3.20)

## Instance selection --------

# Force the instance to be stm32h563vit6x
if(NOT DEFINED INSTANCE)
  set(INSTANCE "stm32h563vit6x")
  message(STATUS "INSTANCE not set; defaulting to stm32h563vit6x")
elseif(NOT INSTANCE STREQUAL "stm32h563vit6x")
  message(FATAL_ERROR "This project only supports INSTANCE=stm32h563vit6x")
endif()

# Hardcode the family and instance directory for STM32H563
set(FAMILY_DIR "stm32h5xx")
set(INSTANCE_DIR ${CMAKE_SOURCE_DIR}/instances/stm32/${FAMILY_DIR}/${INSTANCE})

if(NOT EXISTS ${INSTANCE_DIR}/CMakeLists.txt)
  message(FATAL_ERROR "CMakeLists.txt not found in ${INSTANCE_DIR}")
endif()

# Include ASM so startup files (.s) are built and Reset_Handler is linked
project(multitarget_freertos LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FREERTOS_KERNEL_DIR
  "${CMAKE_SOURCE_DIR}/third_party/FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel"
  CACHE PATH "FreeRTOS kernel directory"
)

include(${INSTANCE_DIR}/CMakeLists.txt)

## Addition of targets -----

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE
  ${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(freertos_config INTERFACE
  ${PLATFORM_DEFS}
)

add_library(freertos_kernel STATIC
  ${FREERTOS_KERNEL_SOURCES}
  ${FREERTOS_PORT_SOURCES}
  ${FREERTOS_HEAP_FILE}
)

target_include_directories(freertos_kernel PUBLIC
  ${FREERTOS_KERNEL_DIR}/include
  ${FREERTOS_PORT_INCLUDE}
)

target_link_libraries(freertos_kernel PUBLIC
  freertos_config
)

add_subdirectory(lib/drivers)
add_subdirectory(app)

# Link any platform-specific libraries to drivers
if(DEFINED DRIVERS_LINK_LIBRARIES)
  target_link_libraries(drivers PUBLIC
    ${DRIVERS_LINK_LIBRARIES}
  )
endif()

add_executable(firmware
  ${FIRMWARE_SOURCES}
  ${PLATFORM_MAIN}
)

target_include_directories(firmware PUBLIC
  ${PLATFORM_MAIN_INCDIR}
)

target_link_libraries(firmware PRIVATE
  ${FIRMWARE_LINK_LIBRARIES}
  src
  freertos_kernel
)
