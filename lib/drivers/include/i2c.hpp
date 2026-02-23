#pragma once

#include <cstddef>
#include <cstdint>

#include "common/common.hpp"

namespace ru::driver {

enum class I2cSpeed {
  standard_100k,
  fast_400k,
  fast_plus_1m,
  high_3m
};

enum class I2cAddressing {
  addr_7bit,
  addr_10bit
};

class I2cConfig : public Config {
public:
  const I2cId m_id;
  I2cSpeed m_speed;
  I2cAddressing m_addressing;
  uint16_t m_own_address;
  I2cConfig(I2cId id, I2cSpeed speed = I2cSpeed::standard_100k,
            I2cAddressing addressing = I2cAddressing::addr_7bit, uint16_t own_address = 0);
};

class I2cInstanceSpecific;

class I2c : public Driver {
  I2cInstanceSpecific* p_instance_specific;

public:
  I2c();
  static expected::expected<void, Error> start();
  expected::expected<void, Error> init(const Config&);
  expected::expected<void, Error> stop();

  expected::expected<void, Error> write(uint16_t addr, const uint8_t* data, size_t len);
  expected::expected<void, Error> read(uint16_t addr, uint8_t* data, size_t len);
  expected::expected<void, Error> write_read(uint16_t addr, const uint8_t* tx, size_t tx_len,
                                        uint8_t* rx, size_t rx_len);
};

} // namespace ru::driver
