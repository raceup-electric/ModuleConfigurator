#include "timer.hpp"

namespace ru::driver {

Timer::Timer() = default;

expected::expected<void, Error> Timer::start() {
  return {};
}

expected::expected<void, Error> Timer::init(const Config&) {
  return {};
}

expected::expected<void, Error> Timer::stop() {
  return {};
}

expected::expected<TimerInstant, Error> Timer::time_now() const {
  return 0;
}

} // namespace ru::driver
