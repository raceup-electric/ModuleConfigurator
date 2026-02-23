#pragma once

#include <cstdint>

#include "common/capability.hpp"
#include "common/common.hpp"

namespace ru::driver {

enum class GpioFunction {
  input_floating,
  input_pullup,
  input_pulldown,
  output_opendrain_pullup,
  output_opendrain_pulldown,
  output_pushpull_pullup,
  output_pushpull_pulldown,
};

enum class GpioFunctionSpecial {
  analog,
  alternate_opendrain_pullup,
  alternate_opendrain_pulldown,
  alternate_pushpull_pullup,
  alternate_pushpull_pulldown
};

enum class GpioSpeed {
  low,        // 33ns | 3MHz
  medium,     // 10ns | 15MHz
  high,       // 6.9ns | 35MHz
  very_high,  // 6.6ns | 50Mhz
};

class GpioConfig: public Config {
public:
  const GpioId m_id;
  GpioFunction m_function;
  bool m_active_high;
  GpioSpeed m_gpio_speed;
  GpioConfig(GpioId id, GpioFunction=GpioFunction::input_floating, bool active_high=true, GpioSpeed=GpioSpeed::low);
};

class GpioConfigFriend: public Config {
public:
  const GpioId m_id;
  GpioFunction m_function;
  GpioConfigFriend(GpioId id, const GpioFunction);
};

class GpioInstanceSpecific;

class Gpio: public Driver {
  uint8_t m_active_high:1;
  uint8_t m_is_output:1;
  GpioInstanceSpecific* p_instance_specific;

public:
  Gpio();
  static expected::expected<void, Error> start();
  expected::expected<void, Error> init(const Config&);
  expected::expected<void, Error> stop();

  expected::expected<bool, Error> is_active() const;
  expected::expected<bool, Error> is_inactive() const;
  expected::expected<bool, Error> is_high() const;
  expected::expected<bool, Error> is_low() const;
  expected::expected<void, Error> set_active();
  expected::expected<void, Error> set_inactive();
  expected::expected<void, Error> set_level(const bool active);
  expected::expected<void, Error> toggle();

  expected::expected<void, Error> init(struct capability::Token1<Can>, const GpioConfigFriend&);
  void reverse_protected_example(struct capability::Token1<Can>);
};

} // namespace ru::driver
