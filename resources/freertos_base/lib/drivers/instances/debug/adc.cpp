#include <optional>

#include "adc.hpp"
#include "debug_log.hpp"

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
  debug::log_op("adc", "start");
  return {};
}

expected::expected<void, Error> Adc::init(const Config& config) {
  const auto* cfg = dynamic_cast<const AdcConfig*>(&config);
  if (cfg) {
    debug::log_op_id("adc", "init", static_cast<uint32_t>(cfg->m_id));
  } else {
    debug::log_op("adc", "init (unknown config)");
  }
  return {};
}

expected::expected<void, Error> Adc::stop() {
  debug::log_op("adc", "stop");
  return {};
}

expected::expected<uint16_t, Error> Adc::read() {
  debug::log_op("adc", "read");
  return 0;
}

expected::expected<std::optional<uint16_t>, Error> Adc::try_read() {
  debug::log_op("adc", "try_read");
  return std::optional<uint16_t>{};
}

} // namespace ru::driver
