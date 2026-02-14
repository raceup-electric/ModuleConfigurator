#pragma once

#include <cstdint>

#include "common/common.hpp"

namespace ru::driver {

using TimerInstant = uint64_t;

class Timer : public Driver {
public:
  Timer();
  static expected::expected<void, Error> start();
  expected::expected<void, Error> init(const Config&);
  expected::expected<void, Error> stop();

  expected::expected<TimerInstant, Error> time_now() const;
};

} // namespace ru::driver
