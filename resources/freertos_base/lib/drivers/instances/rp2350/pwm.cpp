#include "pwm.hpp"
#include "debug_log.hpp"

namespace ru::driver {

PwmConfig::PwmConfig(PwmId id, uint32_t frequency_hz, uint16_t duty_cycle_permille,
                     PwmPolarity polarity)
    : m_id(id),
      m_frequency_hz(frequency_hz),
      m_duty_cycle_permille(duty_cycle_permille),
      m_polarity(polarity) {}

Pwm::Pwm() : p_instance_specific(nullptr) {}

expected::expected<void, Error> Pwm::start() {
  debug::log_op("pwm", "start");
  return {};
}

expected::expected<void, Error> Pwm::init(const Config& config) {
  const auto* cfg = dynamic_cast<const PwmConfig*>(&config);
  if (cfg) {
    debug::log_op_id("pwm", "init", static_cast<uint32_t>(cfg->m_id));
  } else {
    debug::log_op("pwm", "init (unknown config)");
  }
  return {};
}

expected::expected<void, Error> Pwm::stop() {
  debug::log_op("pwm", "stop");
  return {};
}

expected::expected<void, Error> Pwm::enable() {
  debug::log_op("pwm", "enable");
  return {};
}

expected::expected<void, Error> Pwm::disable() {
  debug::log_op("pwm", "disable");
  return {};
}

expected::expected<void, Error> Pwm::set_frequency(uint32_t frequency_hz) {
  std::printf("[debug][pwm] set_frequency hz=%u\n", frequency_hz);
  return {};
}

expected::expected<void, Error> Pwm::set_duty_cycle(uint16_t duty_cycle_permille) {
  std::printf("[debug][pwm] set_duty_cycle permille=%u\n", duty_cycle_permille);
  return {};
}

expected::expected<uint16_t, Error> Pwm::get_duty_cycle() const {
  debug::log_op("pwm", "get_duty_cycle");
  return 0;
}

} // namespace ru::driver
