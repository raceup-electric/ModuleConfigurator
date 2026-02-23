#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "common/common.hpp"

namespace ru::driver {

enum class SerialParity {
  none,
  even,
  odd
};

enum class SerialStopBits {
  one,
  two
};

enum class SerialFlowControl {
  none,
  rts_cts
};

class SerialConfig : public Config {
public:
  const SerialId m_id;
  uint32_t m_baud_rate;
  SerialParity m_parity;
  SerialStopBits m_stop_bits;
  SerialFlowControl m_flow_control;
  SerialConfig(SerialId id, uint32_t baud_rate, SerialParity parity = SerialParity::none,
               SerialStopBits stop_bits = SerialStopBits::one,
               SerialFlowControl flow_control = SerialFlowControl::none);
};

class SerialInstanceSpecific;

class Serial : public Driver {
  SerialInstanceSpecific* p_instance_specific;

public:
  Serial();
  static expected::expected<void, Error> start();
  expected::expected<void, Error> init(const Config&);
  expected::expected<void, Error> stop();

  expected::expected<size_t, Error> write(const uint8_t* data, size_t len);
  expected::expected<size_t, Error> read(uint8_t* data, size_t len);
  expected::expected<std::optional<size_t>, Error> try_read(uint8_t* data, size_t len);
};

} // namespace ru::driver
