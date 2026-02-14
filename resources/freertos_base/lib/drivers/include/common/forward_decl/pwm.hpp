namespace ru::driver {

class Pwm;

enum class PwmError {
  invalid_duty_cycle,
  invalid_frequency,
  not_supported
};

}
