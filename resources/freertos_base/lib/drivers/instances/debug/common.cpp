#include "common/common.hpp"
#include "debug_log.hpp"


namespace ru::driver {

expected::expected<void, Error> Driver::start() {
  debug::log_op("driver", "start");
  return {};
}

} // namespace ru::driver
