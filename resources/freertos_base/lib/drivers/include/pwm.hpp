#pragma once

#include <cstdint>

#include "common/common.hpp"

namespace ru::driver {

enum class PwmPolarity {
  active_high,
  active_low
};

class PwmConfig : public Config {
public:
  const PwmId m_id;
  uint32_t m_frequency_hz;
  uint16_t m_duty_cycle_permille;
  PwmPolarity m_polarity;
  PwmConfig(PwmId id, uint32_t frequency_hz, uint16_t duty_cycle_permille = 0,
            PwmPolarity polarity = PwmPolarity::active_high);
};

class PwmInstanceSpecific;

class Pwm : public Driver {
  PwmInstanceSpecific* p_instance_specific;

public:
  Pwm();
  static expected::expected<void, Error> start();
  expected::expected<void, Error> init(const Config&);
  expected::expected<void, Error> stop();

  expected::expected<void, Error> enable();
  expected::expected<void, Error> disable();
  expected::expected<void, Error> set_frequency(uint32_t frequency_hz);
  expected::expected<void, Error> set_duty_cycle(uint16_t duty_cycle_permille);
  expected::expected<uint16_t, Error> get_duty_cycle() const;
};

} // namespace ru::driver
