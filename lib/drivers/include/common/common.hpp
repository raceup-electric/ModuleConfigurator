// Driver Library
// RaceUp
// By Michele Tesser (michele.tesser@raceup.it)
// 31/12/2025
//
// Di sicuro mancano un paio di cose:
// - manca un layer intermedio per dividere le implementazioni comuni da quelle specifiche
// - bisognerebbe rendere le istanze sicure by design

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <variant>

#include "common/instance_ids.hpp"
#include "forward_decl/adc.hpp"
#include "forward_decl/can.hpp"
#include "forward_decl/common.hpp"
#include "forward_decl/ethernet.hpp"
#include "forward_decl/flash_memory.hpp"
#include "forward_decl/gpio.hpp"
#include "forward_decl/i2c.hpp"
#include "forward_decl/pwm.hpp"
#include "forward_decl/serial.hpp"
#include "forward_decl/spi.hpp"
#include "forward_decl/timer.hpp"

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
  #include <expected>
  namespace expected = std;
#else
  #include "expected.hpp"
  namespace expected = tl;
#endif

#define RU_ERROR(code, msg) \
  ru::driver::Error{ code, { __FILE__, __LINE__, msg } }

namespace ru::driver {

using ErrorCode = std::variant<
  AdcError,
  CanError,
  CommonError,
  EthernetError,
  FlashMemoryError,
  GpioError,
  I2cError,
  PwmError,
  SerialError,
  SpiError,
  TimerError
>;

struct ErrorInfo {
  const char* file;
  uint32_t line;
  const char* message;
};

struct Error {
  ErrorCode code;
  ErrorInfo info;
};

class Config {
public:
  virtual ~Config() = default;
protected:
  Config() = default;
};

class Driver {
private:
  uint8_t m_is_init:1;
public:
  static expected::expected<void, Error> start();
  virtual expected::expected<void, Error> init(const Config&) = 0;
  virtual expected::expected<void, Error> stop() = 0;
  bool is_init() const {return m_is_init;};
protected:
  Driver() = default;
};

} // namespace ru::driver
