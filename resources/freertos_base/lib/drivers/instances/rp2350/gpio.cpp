#include "gpio.hpp"

#include "pico/stdlib.h"
#include "debug_log.hpp"


namespace ru::driver {

struct GpioInstanceSpecific {
  uint32_t pin;
  bool initialized;
};

static GpioInstanceSpecific g_debug_led{PICO_DEFAULT_LED_PIN, false};

static GpioInstanceSpecific* get_gpio_instance(GpioId id) {
  if (id == GpioId::debug_led) {
    return &g_debug_led;
  }
  return nullptr;
}

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
    p_instance_specific = get_gpio_instance(cfg->m_id);
    auto* inst = static_cast<GpioInstanceSpecific*>(p_instance_specific);
    if (inst && m_is_output) {
      gpio_init(inst->pin);
      gpio_set_dir(inst->pin, GPIO_OUT);
      gpio_put(inst->pin, m_active_high ? 0 : 1);
      inst->initialized = true;
    }
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
  auto* inst = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!inst || !inst->initialized) {
    debug::log_op("gpio", "is_active (not initialized)");
    return false;
  }
  const bool level = gpio_get(inst->pin);
  return level == (m_active_high != 0);
}

expected::expected<bool, Error> Gpio::is_inactive() const {
  auto* inst = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!inst || !inst->initialized) {
    debug::log_op("gpio", "is_inactive (not initialized)");
    return true;
  }
  const bool level = gpio_get(inst->pin);
  return level != (m_active_high != 0);
}

expected::expected<bool, Error> Gpio::is_high() const {
  auto* inst = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!inst || !inst->initialized) {
    debug::log_op("gpio", "is_high (not initialized)");
    return false;
  }
  return gpio_get(inst->pin);
}

expected::expected<bool, Error> Gpio::is_low() const {
  auto* inst = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!inst || !inst->initialized) {
    debug::log_op("gpio", "is_low (not initialized)");
    return true;
  }
  return !gpio_get(inst->pin);
}

expected::expected<void, Error> Gpio::set_active() {
  auto* inst = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!inst || !inst->initialized) {
    debug::log_op("gpio", "set_active (not initialized)");
    return {};
  }
  gpio_put(inst->pin, m_active_high ? 1 : 0);
  return {};
}

expected::expected<void, Error> Gpio::set_inactive() {
  auto* inst = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!inst || !inst->initialized) {
    debug::log_op("gpio", "set_inactive (not initialized)");
    return {};
  }
  gpio_put(inst->pin, m_active_high ? 0 : 1);
  return {};
}

expected::expected<void, Error> Gpio::set_level(const bool active) {
  auto* inst = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!inst || !inst->initialized) {
    debug::log_op("gpio", "set_level (not initialized)");
    return {};
  }
  const bool level = (active == (m_active_high != 0));
  gpio_put(inst->pin, level ? 1 : 0);
  return {};
}

expected::expected<void, Error> Gpio::toggle() {
  auto* inst = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!inst || !inst->initialized) {
    debug::log_op("gpio", "toggle (not initialized)");
    return {};
  }
  gpio_put(inst->pin, !gpio_get(inst->pin));
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
