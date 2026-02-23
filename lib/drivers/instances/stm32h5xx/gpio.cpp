#include "gpio.hpp"

#include "stm32h5xx_hal.h"

namespace ru::driver {

class GpioInstanceSpecific {
public:
  GPIO_TypeDef* port;
  uint16_t pin;
};

namespace {
bool is_output_function(GpioFunction function) {
  return function == GpioFunction::output_opendrain_pullup ||
         function == GpioFunction::output_opendrain_pulldown ||
         function == GpioFunction::output_pushpull_pullup ||
         function == GpioFunction::output_pushpull_pulldown;
}

uint32_t to_hal_pull(GpioFunction function) {
  switch (function) {
    case GpioFunction::input_pullup:
    case GpioFunction::output_opendrain_pullup:
    case GpioFunction::output_pushpull_pullup:
      return GPIO_PULLUP;
    case GpioFunction::input_pulldown:
    case GpioFunction::output_opendrain_pulldown:
    case GpioFunction::output_pushpull_pulldown:
      return GPIO_PULLDOWN;
    case GpioFunction::input_floating:
    default:
      return GPIO_NOPULL;
  }
}

uint32_t to_hal_mode(GpioFunction function) {
  switch (function) {
    case GpioFunction::output_opendrain_pullup:
    case GpioFunction::output_opendrain_pulldown:
      return GPIO_MODE_OUTPUT_OD;
    case GpioFunction::output_pushpull_pullup:
    case GpioFunction::output_pushpull_pulldown:
      return GPIO_MODE_OUTPUT_PP;
    case GpioFunction::input_floating:
    case GpioFunction::input_pullup:
    case GpioFunction::input_pulldown:
    default:
      return GPIO_MODE_INPUT;
  }
}

uint32_t to_hal_speed(GpioSpeed speed) {
  switch (speed) {
    case GpioSpeed::medium:
      return GPIO_SPEED_FREQ_MEDIUM;
    case GpioSpeed::high:
      return GPIO_SPEED_FREQ_HIGH;
    case GpioSpeed::very_high:
      return GPIO_SPEED_FREQ_VERY_HIGH;
    case GpioSpeed::low:
    default:
      return GPIO_SPEED_FREQ_LOW;
  }
}
} // namespace

GpioConfig::GpioConfig(GpioId id, GpioFunction function, bool active_high, GpioSpeed speed)
    : m_id(id), m_function(function), m_active_high(active_high), m_gpio_speed(speed) {}

GpioConfigFriend::GpioConfigFriend(GpioId id, const GpioFunction function)
    : m_id(id), m_function(function) {}

Gpio::Gpio() : m_active_high(1), m_is_output(0), p_instance_specific(nullptr) {}

expected::expected<void, Error> Gpio::start() {
  return {};
}

expected::expected<void, Error> Gpio::init(const Config& config) {
  const auto* cfg = dynamic_cast<const GpioConfig*>(&config);
  if (!cfg) {
    return expected::unexpected(RU_ERROR(CommonError::out_of_range, "invalid gpio config"));
  }

  if (cfg->m_id != GpioId::debug_led) {
    return expected::unexpected(RU_ERROR(CommonError::out_of_range, "unsupported gpio id"));
  }

  if (!p_instance_specific) {
    p_instance_specific = new GpioInstanceSpecific();
  }

  auto* hw = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  hw->port = GPIOB;
  hw->pin = GPIO_PIN_2;

  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef init{};
  init.Pin = hw->pin;
  init.Mode = to_hal_mode(cfg->m_function);
  init.Pull = to_hal_pull(cfg->m_function);
  init.Speed = to_hal_speed(cfg->m_gpio_speed);
  HAL_GPIO_Init(hw->port, &init);

  m_active_high = cfg->m_active_high ? 1 : 0;
  m_is_output = is_output_function(cfg->m_function) ? 1 : 0;

  return {};
}

expected::expected<void, Error> Gpio::stop() {
  delete static_cast<GpioInstanceSpecific*>(p_instance_specific);
  p_instance_specific = nullptr;
  return {};
}

expected::expected<bool, Error> Gpio::is_active() const {
  auto* hw = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!hw || !hw->port) {
    return expected::unexpected(RU_ERROR(CommonError::not_inited, "gpio not initialized"));
  }
  return is_high();
}

expected::expected<bool, Error> Gpio::is_inactive() const {
  auto active = is_active();
  if (!active) {
    return expected::unexpected(active.error());
  }
  return !active.value();
}

expected::expected<bool, Error> Gpio::is_high() const {
  auto* hw = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!hw || !hw->port) {
    return expected::unexpected(RU_ERROR(CommonError::not_inited, "gpio not initialized"));
  }
  const GPIO_PinState state = HAL_GPIO_ReadPin(hw->port, hw->pin);
  const bool high = (state == GPIO_PIN_SET);
  return m_active_high ? high : !high;
}

expected::expected<bool, Error> Gpio::is_low() const {
  auto high = is_high();
  if (!high) {
    return expected::unexpected(high.error());
  }
  return !high.value();
}

expected::expected<void, Error> Gpio::set_active() {
  return set_level(true);
}

expected::expected<void, Error> Gpio::set_inactive() {
  return set_level(false);
}

expected::expected<void, Error> Gpio::set_level(const bool active) {
  auto* hw = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!hw || !hw->port) {
    return expected::unexpected(RU_ERROR(CommonError::not_inited, "gpio not initialized"));
  }
  const bool level = m_active_high ? active : !active;
  HAL_GPIO_WritePin(hw->port, hw->pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
  return {};
}

expected::expected<void, Error> Gpio::toggle() {
  auto* hw = static_cast<GpioInstanceSpecific*>(p_instance_specific);
  if (!hw || !hw->port) {
    return expected::unexpected(RU_ERROR(CommonError::not_inited, "gpio not initialized"));
  }
  HAL_GPIO_TogglePin(hw->port, hw->pin);
  return {};
}

expected::expected<void, Error> Gpio::init(struct capability::Token1<Can>, const GpioConfigFriend& config) {
  return init(GpioConfig(config.m_id, config.m_function));
}

void Gpio::reverse_protected_example(struct capability::Token1<Can>) {}

} // namespace ru::driver
