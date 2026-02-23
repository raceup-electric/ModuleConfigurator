#pragma once

#include <cstdint>
#include <cstdio>

namespace ru::driver::debug {

inline void log_op(const char* driver, const char* op) {
  std::printf("[debug][%s] %s\n", driver, op);
}

inline void log_op_id(const char* driver, const char* op, uint32_t id) {
  std::printf("[debug][%s] %s id=%u\n", driver, op, id);
}

} // namespace ru::driver::debug
