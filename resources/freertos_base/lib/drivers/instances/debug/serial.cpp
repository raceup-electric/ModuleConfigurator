#include <cstring>

#include "serial.hpp"
#include "debug_log.hpp"

namespace ru::driver {

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
  } else {
    debug::log_op("serial", "init (unknown config)");
  }
  return {};
}

expected::expected<void, Error> Serial::stop() {
  debug::log_op("serial", "stop");
  return {};
}

expected::expected<size_t, Error> Serial::write(const uint8_t*, size_t len) {
  std::printf("[debug][serial] write len=%zu\n", len);
  return len;
}

expected::expected<size_t, Error> Serial::read(uint8_t* data, size_t len) {
  std::printf("[debug][serial] read len=%zu\n", len);
  if (data && len) {
    std::memset(data, 0, len);
  }
  return len;
}

expected::expected<std::optional<size_t>, Error> Serial::try_read(uint8_t* data, size_t len) {
  std::printf("[debug][serial] try_read len=%zu\n", len);
  if (data && len) {
    std::memset(data, 0, len);
  }
  return std::optional<size_t>{};
}

} // namespace ru::driver
