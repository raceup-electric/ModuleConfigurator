#pragma once

#include <cstdint>

#include "common/driver_id_macros.hpp"

namespace ru::driver {

enum class AdcId : uint16_t {
  invalid = 0,
#undef X_adc
#define X_adc(name) name,
#include "custom_board/driver_ids.hpp"
#undef X_adc
#define X_adc(name)
};

enum class CanId : uint16_t {
  invalid = 0,
#undef X_can
#define X_can(name) name,
#include "custom_board/driver_ids.hpp"
#undef X_can
#define X_can(name)
};

enum class EthernetId : uint16_t {
  invalid = 0,
#undef X_ethernet
#define X_ethernet(name) name,
#include "custom_board/driver_ids.hpp"
#undef X_ethernet
#define X_ethernet(name)
};

enum class FlashMemoryId : uint16_t {
  invalid = 0,
#undef X_flash_memory
#define X_flash_memory(name) name,
#include "custom_board/driver_ids.hpp"
#undef X_flash_memory
#define X_flash_memory(name)
};

enum class GpioId : uint16_t {
  invalid = 0,
#undef X_gpio
#define X_gpio(name) name,
#include "custom_board/driver_ids.hpp"
#undef X_gpio
#define X_gpio(name)
};

enum class I2cId : uint16_t {
  invalid = 0,
#undef X_i2c
#define X_i2c(name) name,
#include "custom_board/driver_ids.hpp"
#undef X_i2c
#define X_i2c(name)
};

enum class PwmId : uint16_t {
  invalid = 0,
#undef X_pwm
#define X_pwm(name) name,
#include "custom_board/driver_ids.hpp"
#undef X_pwm
#define X_pwm(name)
};

enum class SerialId : uint16_t {
  invalid = 0,
#undef X_serial
#define X_serial(name) name,
#include "custom_board/driver_ids.hpp"
#undef X_serial
#define X_serial(name)
};

enum class SpiId : uint16_t {
  invalid = 0,
#undef X_spi
#define X_spi(name) name,
#include "custom_board/driver_ids.hpp"
#undef X_spi
#define X_spi(name)
};

enum class TimerId : uint16_t {
  invalid = 0,
#undef X_timer
#define X_timer(name) name,
#include "custom_board/driver_ids.hpp"
#undef X_timer
#define X_timer(name)
};

} // namespace ru::driver
