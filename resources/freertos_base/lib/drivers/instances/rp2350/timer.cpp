#include <chrono>

#include "timer.hpp"
#include "debug_log.hpp"

namespace ru::driver {

Timer::Timer() = default;

expected::expected<void, Error> Timer::start() {
  debug::log_op("timer", "start");
  return {};
}

expected::expected<void, Error> Timer::init(const Config&) {
  debug::log_op("timer", "init");
  return {};
}

expected::expected<void, Error> Timer::stop() {
  debug::log_op("timer", "stop");
  return {};
}

expected::expected<TimerInstant, Error> Timer::time_now() const {
  const auto now = std::chrono::steady_clock::now().time_since_epoch();
  const auto us = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
  std::printf("[debug][timer] time_now us=%lld\n", static_cast<long long>(us));
  return static_cast<TimerInstant>(us);
}

} // namespace ru::driver
