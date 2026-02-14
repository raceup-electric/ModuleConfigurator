#include <cstring>

#include "flash_memory.hpp"
#include "debug_log.hpp"

namespace ru::driver {

FlashMemoryConfig::FlashMemoryConfig(FlashMemoryId id, uint32_t total_size)
    : m_id(id), m_total_size(total_size) {}

FlashMemory::FlashMemory() : p_instance_specific(nullptr) {}

expected::expected<void, Error> FlashMemory::start() {
  debug::log_op("flash", "start");
  return {};
}

expected::expected<void, Error> FlashMemory::init(const Config& config) {
  const auto* cfg = dynamic_cast<const FlashMemoryConfig*>(&config);
  if (cfg) {
    debug::log_op_id("flash", "init", static_cast<uint32_t>(cfg->m_id));
  } else {
    debug::log_op("flash", "init (unknown config)");
  }
  return {};
}

expected::expected<void, Error> FlashMemory::stop() {
  debug::log_op("flash", "stop");
  return {};
}

expected::expected<void, Error> FlashMemory::read(uint32_t addr, uint8_t* data, size_t len) {
  std::printf("[debug][flash] read addr=%u len=%zu\n", addr, len);
  if (data && len) {
    std::memset(data, 0, len);
  }
  return {};
}

expected::expected<void, Error> FlashMemory::write(uint32_t addr, const uint8_t*, size_t len) {
  std::printf("[debug][flash] write addr=%u len=%zu\n", addr, len);
  return {};
}

expected::expected<void, Error> FlashMemory::erase(uint32_t addr, size_t len) {
  std::printf("[debug][flash] erase addr=%u len=%zu\n", addr, len);
  return {};
}

expected::expected<void, Error> FlashMemory::erase_all() {
  debug::log_op("flash", "erase_all");
  return {};
}

} // namespace ru::driver
