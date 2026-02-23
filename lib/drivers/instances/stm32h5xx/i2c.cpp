#include "i2c.hpp"

namespace ru::driver {

I2cConfig::I2cConfig(I2cId id, I2cSpeed speed, I2cAddressing addressing, uint16_t own_address)
    : m_id(id),
      m_speed(speed),
      m_addressing(addressing),
      m_own_address(own_address) {}

I2c::I2c() : p_instance_specific(nullptr) {}

expected::expected<void, Error> I2c::start() {
  return {};
}

expected::expected<void, Error> I2c::init(const Config&) {
  return {};
}

expected::expected<void, Error> I2c::stop() {
  return {};
}

expected::expected<void, Error> I2c::write(uint16_t, const uint8_t*, size_t) {
  return {};
}

expected::expected<void, Error> I2c::read(uint16_t, uint8_t*, size_t) {
  return {};
}

expected::expected<void, Error> I2c::write_read(uint16_t, const uint8_t*, size_t, uint8_t*, size_t) {
  return {};
}

} // namespace ru::driver
