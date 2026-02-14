#include "spi.hpp"

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
  return {};
}

expected::expected<void, Error> Spi::init(const Config&) {
  return {};
}

expected::expected<void, Error> Spi::stop() {
  return {};
}

expected::expected<void, Error> Spi::write(const uint8_t*, size_t) {
  return {};
}

expected::expected<void, Error> Spi::read(uint8_t*, size_t) {
  return {};
}

expected::expected<void, Error> Spi::transfer(const uint8_t*, uint8_t*, size_t) {
  return {};
}

} // namespace ru::driver
