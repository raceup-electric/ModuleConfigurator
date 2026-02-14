#include <cstring>

#include "stm32h5xx.h"

#include "serial.hpp"

namespace ru::driver {

class SerialInstanceSpecific {
public:
  USART_TypeDef* usart;
};

SerialConfig::SerialConfig(SerialId id, uint32_t baud_rate, SerialParity parity,
                           SerialStopBits stop_bits, SerialFlowControl flow_control)
    : m_id(id),
      m_baud_rate(baud_rate),
      m_parity(parity),
      m_stop_bits(stop_bits),
      m_flow_control(flow_control) {}

Serial::Serial() : p_instance_specific(nullptr) {}

expected::expected<void, Error> Serial::start() {
  return {};
}

expected::expected<void, Error> Serial::init(const Config& config) {
  const auto* cfg = dynamic_cast<const SerialConfig*>(&config);
  if (cfg) {
    if (!p_instance_specific) {
      p_instance_specific = new SerialInstanceSpecific();
    }
    auto* hw = static_cast<SerialInstanceSpecific*>(p_instance_specific);
    hw->usart = USART1;
  } else {
    return expected::unexpected(RU_ERROR(CommonError::out_of_range, "invalid serial config"));
  }
  return {};
}

expected::expected<void, Error> Serial::stop() {
  delete static_cast<SerialInstanceSpecific*>(p_instance_specific);
  p_instance_specific = nullptr;
  return {};
}

expected::expected<size_t, Error> Serial::write(const uint8_t* data, size_t len) {
  auto* hw = static_cast<SerialInstanceSpecific*>(p_instance_specific);
  if (!hw || !hw->usart) {
    return expected::unexpected(RU_ERROR(CommonError::not_inited, "serial not initialized"));
  }

  for (size_t i = 0; i < len; ++i) {
    while (!(hw->usart->ISR & USART_ISR_TXE_TXFNF)) {}
    hw->usart->TDR = data[i];
  }

  return len;
}

expected::expected<size_t, Error> Serial::read(uint8_t* data, size_t len) {
  auto* hw = static_cast<SerialInstanceSpecific*>(p_instance_specific);
  if (!hw || !hw->usart) {
    return expected::unexpected(RU_ERROR(CommonError::not_inited, "serial not initialized"));
  }

  size_t count = 0;
  for (; count < len; ++count) {
    if (!(hw->usart->ISR & USART_ISR_RXNE_RXFNE)) {
      break;
    }
    data[count] = static_cast<uint8_t>(hw->usart->RDR);
  }

  if (data && count < len) {
    std::memset(data + count, 0, len - count);
  }
  return count;
}

expected::expected<std::optional<size_t>, Error> Serial::try_read(uint8_t* data, size_t len) {
  if (!data || !len) {
    return std::optional<size_t>{};
  }

  auto* hw = static_cast<SerialInstanceSpecific*>(p_instance_specific);
  if (!hw || !hw->usart) {
    return expected::unexpected(RU_ERROR(CommonError::not_inited, "serial not initialized"));
  }

  size_t count = 0;
  for (; count < len; ++count) {
    if (!(hw->usart->ISR & USART_ISR_RXNE_RXFNE)) {
      break;
    }
    data[count] = static_cast<uint8_t>(hw->usart->RDR);
  }

  if (!count) {
    return std::optional<size_t>{};
  }
  return count;
}

} // namespace ru::driver
