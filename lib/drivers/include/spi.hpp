#pragma once

#include <cstddef>
#include <cstdint>

#include "common/common.hpp"

namespace ru::driver {

enum class SpiMode {
  mode0,
  mode1,
  mode2,
  mode3
};

enum class SpiClockPolarity {
  idle_low,
  idle_high
};

enum class SpiBitOrder {
  msb_first,
  lsb_first
};

enum class SpiDirection {
  master,
  slave
};

class SpiConfig : public Config {
public:
  const SpiId m_id;
  uint32_t m_frequency_hz;
  SpiMode m_mode;
  SpiClockPolarity m_clock_polarity;
  SpiBitOrder m_bit_order;
  SpiDirection m_direction;
  bool m_chip_select_active_low;
  SpiConfig(SpiId id, uint32_t frequency_hz, SpiMode mode = SpiMode::mode0,
            SpiClockPolarity clock_polarity = SpiClockPolarity::idle_low,
            SpiBitOrder bit_order = SpiBitOrder::msb_first,
            SpiDirection direction = SpiDirection::master,
            bool chip_select_active_low = true);
};

class SpiInstanceSpecific;

class Spi : public Driver {
  SpiInstanceSpecific* p_instance_specific;

public:
  Spi();
  static expected::expected<void, Error> start();
  expected::expected<void, Error> init(const Config&);
  expected::expected<void, Error> stop();

  expected::expected<void, Error> write(const uint8_t* data, size_t len);
  expected::expected<void, Error> read(uint8_t* data, size_t len);
  expected::expected<void, Error> transfer(const uint8_t* tx, uint8_t* rx, size_t len);
};

} // namespace ru::driver
