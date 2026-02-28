#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif


#define USER_LED_BANK  GPIOE
#define USER_LED_PIN   GPIO_PIN_3


void SystemClock_Config(void);

void config_FDCAN(void);

void config_GPIO(void);

#ifdef __cplusplus
}
#endif