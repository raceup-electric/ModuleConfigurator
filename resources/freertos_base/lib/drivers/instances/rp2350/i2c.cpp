#include <cstring>

#include "i2c.hpp"
#include "debug_log.hpp"

namespace ru::driver {

I2cConfig::I2cConfig(I2cId id, I2cSpeed speed, I2cAddressing addressing, uint16_t own_address)
    : m_id(id), m_speed(speed), m_addressing(addressing), m_own_address(own_address) {}

I2c::I2c() : p_instance_specific(nullptr) {}

expected::expected<void, Error> I2c::start() {
  debug::log_op("i2c", "start");
  return {};
}

expected::expected<void, Error> I2c::init(const Config& config) {
  const auto* cfg = dynamic_cast<const I2cConfig*>(&config);
  if (cfg) {
    debug::log_op_id("i2c", "init", static_cast<uint32_t>(cfg->m_id));
  } else {
    debug::log_op("i2c", "init (unknown config)");
  }
  return {};
}

expected::expected<void, Error> I2c::stop() {
  debug::log_op("i2c", "stop");
  return {};
}

expected::expected<void, Error> I2c::write(uint16_t addr, const uint8_t*, size_t len) {
  std::printf("[debug][i2c] write addr=0x%04x len=%zu\n", addr, len);
  return {};
}

expected::expected<void, Error> I2c::read(uint16_t addr, uint8_t* data, size_t len) {
  std::printf("[debug][i2c] read addr=0x%04x len=%zu\n", addr, len);
  if (data && len) {
    std::memset(data, 0, len);
  }
  return {};
}

expected::expected<void, Error> I2c::write_read(uint16_t addr, const uint8_t*, size_t tx_len,
                                          uint8_t* rx, size_t rx_len) {
  std::printf("[debug][i2c] write_read addr=0x%04x tx_len=%zu rx_len=%zu\n", addr, tx_len, rx_len);
  if (rx && rx_len) {
    std::memset(rx, 0, rx_len);
  }
  return {};
}

} // namespace ru::driver
