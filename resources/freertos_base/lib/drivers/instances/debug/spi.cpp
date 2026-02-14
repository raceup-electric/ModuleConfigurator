#include <cstring>

#include "spi.hpp"
#include "debug_log.hpp"

namespace ru::driver {

SpiConfig::SpiConfig(SpiId id, uint32_t frequency_hz, SpiMode mode,
                     SpiClockPolarity clock_polarity, SpiBitOrder bit_order,
                     SpiDirection direction, bool chip_select_active_low)
    : m_id(id),
      m_frequency_hz(frequency_hz),
      m_mode(mode),
      m_clock_polarity(clock_polarity),
      m_bit_order(bit_order),
      m_direction(direction),
      m_chip_select_active_low(chip_select_active_low) {}

Spi::Spi() : p_instance_specific(nullptr) {}

expected::expected<void, Error> Spi::start() {
  debug::log_op("spi", "start");
  return {};
}

expected::expected<void, Error> Spi::init(const Config& config) {
  const auto* cfg = dynamic_cast<const SpiConfig*>(&config);
  if (cfg) {
    debug::log_op_id("spi", "init", static_cast<uint32_t>(cfg->m_id));
  } else {
    debug::log_op("spi", "init (unknown config)");
  }
  return {};
}

expected::expected<void, Error> Spi::stop() {
  debug::log_op("spi", "stop");
  return {};
}

expected::expected<void, Error> Spi::write(const uint8_t*, size_t len) {
  std::printf("[debug][spi] write len=%zu\n", len);
  return {};
}

expected::expected<void, Error> Spi::read(uint8_t* data, size_t len) {
  std::printf("[debug][spi] read len=%zu\n", len);
  if (data && len) {
    std::memset(data, 0, len);
  }
  return {};
}

expected::expected<void, Error> Spi::transfer(const uint8_t*, uint8_t* rx, size_t len) {
  std::printf("[debug][spi] transfer len=%zu\n", len);
  if (rx && len) {
    std::memset(rx, 0, len);
  }
  return {};
}

} // namespace ru::driver
