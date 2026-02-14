#include <optional>

#include "adc.hpp"

namespace ru::driver {

AdcConfig::AdcConfig(AdcId id, uint8_t channel, AdcResolution resolution,
                     AdcAlignment alignment, bool continuous)
    : m_id(id),
      m_channel(channel),
      m_resolution(resolution),
      m_alignment(alignment),
      m_continuous(continuous) {}

Adc::Adc() : p_instance_specific(nullptr) {}

expected::expected<void, Error> Adc::start() {
  return {};
}

expected::expected<void, Error> Adc::init(const Config&) {
  return {};
}

expected::expected<void, Error> Adc::stop() {
  return {};
}

expected::expected<uint16_t, Error> Adc::read() {
  return 0;
}

expected::expected<std::optional<uint16_t>, Error> Adc::try_read() {
  return std::optional<uint16_t>{};
}

} // namespace ru::driver
