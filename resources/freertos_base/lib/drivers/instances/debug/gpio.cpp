#include "gpio.hpp"
#include "debug_log.hpp"

namespace ru::driver {

GpioConfig::GpioConfig(GpioId id, GpioFunction function, bool active_high, GpioSpeed speed)
    : m_id(id), m_function(function), m_active_high(active_high), m_gpio_speed(speed) {}

GpioConfigFriend::GpioConfigFriend(GpioId id, const GpioFunction function)
    : m_id(id), m_function(function) {}

Gpio::Gpio() : m_active_high(1), m_is_output(0), p_instance_specific(nullptr) {}

expected::expected<void, Error> Gpio::start() {
  debug::log_op("gpio", "start");
  return {};
}

expected::expected<void, Error> Gpio::init(const Config& config) {
  const auto* cfg = dynamic_cast<const GpioConfig*>(&config);
  if (cfg) {
    debug::log_op_id("gpio", "init", static_cast<uint32_t>(cfg->m_id));
    m_active_high = cfg->m_active_high ? 1 : 0;
    m_is_output = (cfg->m_function == GpioFunction::output_opendrain_pullup ||
                   cfg->m_function == GpioFunction::output_opendrain_pulldown ||
                   cfg->m_function == GpioFunction::output_pushpull_pullup ||
                   cfg->m_function == GpioFunction::output_pushpull_pulldown)
                    ? 1
                    : 0;
  } else {
    debug::log_op("gpio", "init (unknown config)");
  }
  return {};
}

expected::expected<void, Error> Gpio::stop() {
  debug::log_op("gpio", "stop");
  return {};
}

expected::expected<bool, Error> Gpio::is_active() const {
  debug::log_op("gpio", "is_active");
  return false;
}

expected::expected<bool, Error> Gpio::is_inactive() const {
  debug::log_op("gpio", "is_inactive");
  return true;
}

expected::expected<bool, Error> Gpio::is_high() const {
  debug::log_op("gpio", "is_high");
  return false;
}

expected::expected<bool, Error> Gpio::is_low() const {
  debug::log_op("gpio", "is_low");
  return true;
}

expected::expected<void, Error> Gpio::set_active() {
  debug::log_op("gpio", "set_active");
  return {};
}

expected::expected<void, Error> Gpio::set_inactive() {
  debug::log_op("gpio", "set_inactive");
  return {};
}

expected::expected<void, Error> Gpio::set_level(const bool active) {
  std::printf("[debug][gpio] set_level active=%d\n", active ? 1 : 0);
  return {};
}

expected::expected<void, Error> Gpio::toggle() {
  debug::log_op("gpio", "toggle");
  return {};
}

expected::expected<void, Error> Gpio::init(struct capability::Token1<Can>,
                                      const GpioConfigFriend& config) {
  debug::log_op_id("gpio", "init(friend)", static_cast<uint32_t>(config.m_id));
  return {};
}

void Gpio::reverse_protected_example(struct capability::Token1<Can>) {
  debug::log_op("gpio", "reverse_protected_example");
}

} // namespace ru::driver
