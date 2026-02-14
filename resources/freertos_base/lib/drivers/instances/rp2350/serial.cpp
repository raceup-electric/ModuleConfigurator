#include <cstring>

#include "pico/stdio.h"
#include "pico/stdio_uart.h"
#include "hardware/uart.h"

#include "serial.hpp"
#include "debug_log.hpp"

namespace ru::driver {

namespace {
bool s_stdio_ready = false;

void ensure_stdio_ready(uint32_t baud_rate) {
  if (s_stdio_ready) {
    return;
  }
#if defined(uart_default) && defined(PICO_DEFAULT_UART_TX_PIN) && defined(PICO_DEFAULT_UART_RX_PIN)
  stdio_uart_init_full(uart_default, baud_rate, PICO_DEFAULT_UART_TX_PIN, PICO_DEFAULT_UART_RX_PIN);
  s_stdio_ready = true;
#else
  (void)baud_rate;
#endif
}
} // namespace

SerialConfig::SerialConfig(SerialId id, uint32_t baud_rate, SerialParity parity,
                           SerialStopBits stop_bits, SerialFlowControl flow_control)
    : m_id(id),
      m_baud_rate(baud_rate),
      m_parity(parity),
      m_stop_bits(stop_bits),
      m_flow_control(flow_control) {}

Serial::Serial() : p_instance_specific(nullptr) {}

expected::expected<void, Error> Serial::start() {
  debug::log_op("serial", "start");
  return {};
}

expected::expected<void, Error> Serial::init(const Config& config) {
  const auto* cfg = dynamic_cast<const SerialConfig*>(&config);
  if (cfg) {
    debug::log_op_id("serial", "init", static_cast<uint32_t>(cfg->m_id));
    if (cfg->m_id == SerialId::serial_debug) {
      ensure_stdio_ready(cfg->m_baud_rate);
    }
  } else {
    debug::log_op("serial", "init (unknown config)");
  }
  return {};
}

expected::expected<void, Error> Serial::stop() {
  debug::log_op("serial", "stop");
  return {};
}

expected::expected<size_t, Error> Serial::write(const uint8_t* data, size_t len) {
  if (!s_stdio_ready) {
    ensure_stdio_ready(115200);
  }
  if (len) {
    stdio_put_string(reinterpret_cast<const char*>(data), static_cast<int>(len), false, true);
  }
  return len;
}

expected::expected<size_t, Error> Serial::read(uint8_t* data, size_t len) {
  if (!s_stdio_ready) {
    ensure_stdio_ready(115200);
  }
  if (data && len) {
    size_t count = 0;
    for (; count < len; ++count) {
      int ch = getchar_timeout_us(0);
      if (ch < 0) {
        break;
      }
      data[count] = static_cast<uint8_t>(ch);
    }
    if (count < len) {
      std::memset(data + count, 0, len - count);
    }
    return count;
  }
  return 0;
}

expected::expected<std::optional<size_t>, Error> Serial::try_read(uint8_t* data, size_t len) {
  if (!s_stdio_ready) {
    ensure_stdio_ready(115200);
  }
  if (!data || !len) {
    return std::optional<size_t>{};
  }

  size_t count = 0;
  for (; count < len; ++count) {
    int ch = getchar_timeout_us(0);
    if (ch < 0) {
      break;
    }
    data[count] = static_cast<uint8_t>(ch);
  }
  if (!count) {
    return std::optional<size_t>{};
  }
  return count;
}

} // namespace ru::driver
