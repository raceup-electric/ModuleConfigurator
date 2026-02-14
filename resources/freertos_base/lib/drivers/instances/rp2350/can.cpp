#include <optional>

#include "can.hpp"
#include "debug_log.hpp"

namespace ru::driver {

CanConfig::CanConfig(CanId id, CanBitrate bitrate)
    : m_id(id), m_normal_bitrate(bitrate) {}

CanTx& Can::into_tx() & {
  debug::log_op("can", "into_tx");
  static CanTx tx(*this, capability::Token1<Can>{});
  return tx;
}

CanRx& Can::into_rx() & {
  debug::log_op("can", "into_rx");
  static CanRx rx(*this, capability::Token1<Can>{});
  return rx;
}

Can::Can() : p_instance_specific(nullptr) {}

expected::expected<void, Error> Can::start() {
  debug::log_op("can", "start");
  return {};
}

expected::expected<void, Error> Can::init(const Config& config) {
  const auto* cfg = dynamic_cast<const CanConfig*>(&config);
  if (cfg) {
    debug::log_op_id("can", "init", static_cast<uint32_t>(cfg->m_id));
  } else {
    debug::log_op("can", "init (unknown config)");
  }
  return {};
}

expected::expected<void, Error> Can::stop() {
  debug::log_op("can", "stop");
  return {};
}

expected::expected<CanMessage, Error> Can::read() {
  debug::log_op("can", "read");
  return CanMessage{};
}

expected::expected<std::optional<CanMessage>, Error> Can::try_read() {
  debug::log_op("can", "try_read");
  return std::optional<CanMessage>{};
}

expected::expected<void, Error> Can::write(const CanMessage&) {
  debug::log_op("can", "write");
  return {};
}

expected::expected<void, Error> Can::try_write(const CanMessage&) {
  debug::log_op("can", "try_write");
  return {};
}

expected::expected<CanMessage, Error> CanRx::read() {
  debug::log_op("can_rx", "read");
  return CanMessage{};
}

expected::expected<std::optional<CanMessage>, Error> CanRx::try_read() {
  debug::log_op("can_rx", "try_read");
  return std::optional<CanMessage>{};
}

expected::expected<void, Error> CanTx::write(const CanMessage&) {
  debug::log_op("can_tx", "write");
  return {};
}

expected::expected<void, Error> CanTx::try_write(const CanMessage&) {
  debug::log_op("can_tx", "try_write");
  return {};
}

} // namespace ru::driver
