namespace ru::driver {

class Serial;

enum class SerialError {
  parity,
  framing,
  overrun,
  timeout,
  buffer_full
};

}
