namespace ru::driver {

class Can;

enum class CanError{
  wrong_frequency,
  bus_error_stuff,
  bus_error_form,
  bus_error_acknowledge,
  bus_error_bit_recessive,
  bus_error_bit_dominant,
  bus_error_crc,
  bus_error_software,
  bus_error_off,
  bus_error_passive,
  bus_error_warning,
  bus_error_mode_error_active,
  bus_error_mode_error_passive,
  bus_error_mode_bus_off,
};

}
