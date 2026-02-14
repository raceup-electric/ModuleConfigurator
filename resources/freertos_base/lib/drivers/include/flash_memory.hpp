#pragma once

#include <cstddef>
#include <cstdint>

#include "common/common.hpp"

namespace ru::driver {

class FlashMemoryConfig : public Config {
public:
  const FlashMemoryId m_id;
  uint32_t m_total_size;
  FlashMemoryConfig(FlashMemoryId id, uint32_t total_size);
};

class FlashMemoryInstanceSpecific;

class FlashMemory : public Driver {
  FlashMemoryInstanceSpecific* p_instance_specific;

public:
  FlashMemory();
  static expected::expected<void, Error> start();
  expected::expected<void, Error> init(const Config&);
  expected::expected<void, Error> stop();

  expected::expected<void, Error> read(uint32_t addr, uint8_t* data, size_t len);
  expected::expected<void, Error> write(uint32_t addr, const uint8_t* data, size_t len);
  expected::expected<void, Error> erase(uint32_t addr, size_t len);
  expected::expected<void, Error> erase_all();
};

} // namespace ru::driver
