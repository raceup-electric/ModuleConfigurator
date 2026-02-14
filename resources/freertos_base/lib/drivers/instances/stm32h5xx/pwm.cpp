#include "pwm.hpp"

namespace ru::driver {

PwmConfig::PwmConfig(PwmId id, uint32_t frequency_hz, uint16_t duty_cycle_permille,
                     PwmPolarity polarity)
    : m_id(id),
      m_frequency_hz(frequency_hz),
      m_duty_cycle_permille(duty_cycle_permille),
      m_polarity(polarity) {}

Pwm::Pwm() : p_instance_specific(nullptr) {}

expected::expected<void, Error> Pwm::start() {
  return {};
}

expected::expected<void, Error> Pwm::init(const Config&) {
  return {};
}

expected::expected<void, Error> Pwm::stop() {
  return {};
}

expected::expected<void, Error> Pwm::enable() {
  return {};
}

expected::expected<void, Error> Pwm::disable() {
  return {};
}

expected::expected<void, Error> Pwm::set_frequency(uint32_t) {
  return {};
}

expected::expected<void, Error> Pwm::set_duty_cycle(uint16_t) {
  return {};
}

expected::expected<uint16_t, Error> Pwm::get_duty_cycle() const {
  return 0;
}

} // namespace ru::driver
