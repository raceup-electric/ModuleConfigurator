#include <optional>

#include "can.hpp"

namespace ru::driver {

CanConfig::CanConfig(CanId id, CanBitrate bitrate)
    : m_id(id), m_normal_bitrate(bitrate) {}

CanTx& Can::into_tx() & {
  static CanTx tx(*this, capability::Token1<Can>{});
  return tx;
}

CanRx& Can::into_rx() & {
  static CanRx rx(*this, capability::Token1<Can>{});
  return rx;
}

Can::Can() : p_instance_specific(nullptr) {}

expected::expected<void, Error> Can::start() {
  return {};
}

expected::expected<void, Error> Can::init(const Config&) {
  return {};
}

expected::expected<void, Error> Can::stop() {
  return {};
}

expected::expected<CanMessage, Error> Can::read() {
  return CanMessage{};
}

expected::expected<std::optional<CanMessage>, Error> Can::try_read() {
  return std::optional<CanMessage>{};
}

expected::expected<void, Error> Can::write(const CanMessage&) {
  return {};
}

expected::expected<void, Error> Can::try_write(const CanMessage&) {
  return {};
}

expected::expected<CanMessage, Error> CanRx::read() {
  return CanMessage{};
}

expected::expected<std::optional<CanMessage>, Error> CanRx::try_read() {
  return std::optional<CanMessage>{};
}

expected::expected<void, Error> CanTx::write(const CanMessage&) {
  return {};
}

expected::expected<void, Error> CanTx::try_write(const CanMessage&) {
  return {};
}

} // namespace ru::driver
