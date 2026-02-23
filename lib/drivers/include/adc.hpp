#pragma once

#include <cstdint>
#include <optional>

#include "common/common.hpp"

namespace ru::driver {

enum class AdcResolution {
  bits_6 = 6,
  bits_8 = 8,
  bits_10 = 10,
  bits_12 = 12,
  bits_14 = 14,
  bits_16 = 16
};

enum class AdcAlignment {
  right,
  left
};

class AdcConfig : public Config {
public:
  const AdcId m_id;
  uint8_t m_channel;
  AdcResolution m_resolution;
  AdcAlignment m_alignment;
  bool m_continuous;
  AdcConfig(AdcId id, uint8_t channel, AdcResolution resolution = AdcResolution::bits_12,
            AdcAlignment alignment = AdcAlignment::right, bool continuous = false);
};

class AdcInstanceSpecific;

class Adc : public Driver {
  AdcInstanceSpecific* p_instance_specific;

public:
  Adc();
  static expected::expected<void, Error> start();
  expected::expected<void, Error> init(const Config&);
  expected::expected<void, Error> stop();

  expected::expected<uint16_t, Error> read();
  expected::expected<std::optional<uint16_t>, Error> try_read();
};

} // namespace ru::driver
