#include "flash_memory.hpp"

namespace ru::driver {

FlashMemoryConfig::FlashMemoryConfig(FlashMemoryId id, uint32_t total_size)
    : m_id(id), m_total_size(total_size) {}

FlashMemory::FlashMemory() : p_instance_specific(nullptr) {}

expected::expected<void, Error> FlashMemory::start() {
  return {};
}

expected::expected<void, Error> FlashMemory::init(const Config&) {
  return {};
}

expected::expected<void, Error> FlashMemory::stop() {
  return {};
}

expected::expected<void, Error> FlashMemory::read(uint32_t, uint8_t*, size_t) {
  return {};
}

expected::expected<void, Error> FlashMemory::write(uint32_t, const uint8_t*, size_t) {
  return {};
}

expected::expected<void, Error> FlashMemory::erase(uint32_t, size_t) {
  return {};
}

expected::expected<void, Error> FlashMemory::erase_all() {
  return {};
}

} // namespace ru::driver
