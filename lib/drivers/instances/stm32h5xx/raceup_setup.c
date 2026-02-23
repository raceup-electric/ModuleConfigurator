#include "main.h"
#include "raceup_fdcan.h"

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

  // PLL1
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 125;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
  PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL2Q;
  PeriphClkInitStruct.PLL2.PLL2Source = RCC_PLL2_SOURCE_HSE; 
  PeriphClkInitStruct.PLL2.PLL2M = 2;
  PeriphClkInitStruct.PLL2.PLL2N = 10;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 3;
  PeriphClkInitStruct.PLL2.PLL2R = 2;

  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2_VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2_VCORANGE_MEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.PLL2.PLL2ClockOut = RCC_PLL2_DIVQ;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    Error_Handler();
  }
  
  
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

void config_FDCAN(void) {

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable global FDCAN clock */
  __HAL_RCC_FDCAN_CLK_ENABLE();

  /* Loop through all FDCAN instances (e.g., fdcan1, fdcan2) defined in YAML */
  
  

  /* ==============================================================================
   * FDCAN1 Hardware Setup (GPIO & NVIC)
   * ============================================================================== */

  /* Enable GPIO Bank Clocks */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE(); // Enable Tx bank just in case
  
  /* Configure Rx and Tx pins */
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
  
  // Assumes Rx and Tx are on the same GPIO bank for this implementation
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* Configure Interrupts (NVIC) */
  
  HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 5, 0); // Rx Interrupts
  HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
  

  
  HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 6, 0); // Error Interrupts
  HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
  

  /* ==============================================================================
   * FDCAN1 Peripheral Configuration 
   * 
   * ============================================================================== */

  /* 1. Bit Timings (Calculated by clock_prescaler.py or fixed for now) */
  RUP_FDCAN_BitTimingTypeDef timing_fdcan1 = {
    .presc = 2,
    .ts1   = 8,
    .ts2   = 1,
    .sjw   = 1  /* Recommended default SJW */
  };

  /* 2. Global Filter Configuration */
  
  
    
    
    
  

  /* 3. RX Interrupt Mode Configuration */
  
  

  /* 4. Peripheral Initialization */
  RUP_FDCAN_Init(FDCAN1, timing_fdcan1, RUP_FDCAN_REJECT, RUP_FDCAN_IT_NONE);

  /* 5. Reception Filters */
  
    
    
    
    
    
    
    
    
  RUP_FDCAN_AddFilter(FDCAN1, RUP_FDCAN_FILTER_RANGE, RUP_FDCAN_FILTER_TO_RXFIFO0, 0x100, 0x110);
  
    
    
    
    
    
    
    
    
  RUP_FDCAN_AddFilter(FDCAN1, RUP_FDCAN_FILTER_MASK, RUP_FDCAN_FILTER_TO_RXFIFO1, 0x200, 0x7F0);
  

  /* 6. Start Peripheral */
  RUP_FDCAN_Start(FDCAN1);

  


}

void config_GPIO(void) {


  
  GPIO_InitTypeDef GPIO_InitStruct_F4 = {0};
  __HAL_RCC_GPIOF_CLK_ENABLE();
  GPIO_InitStruct_F4.Pin = GPIO_PIN_4;
  GPIO_InitStruct_F4.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_F4.Pull = GPIO_NOPULL;
  GPIO_InitStruct_F4.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct_F4);

  
  GPIO_InitTypeDef GPIO_InitStruct_G4 = {0};
  __HAL_RCC_GPIOG_CLK_ENABLE();
  GPIO_InitStruct_G4.Pin = GPIO_PIN_4;
  GPIO_InitStruct_G4.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_G4.Pull = GPIO_NOPULL;
  GPIO_InitStruct_G4.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct_G4);

  
  GPIO_InitTypeDef GPIO_InitStruct_B0 = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct_B0.Pin = GPIO_PIN_0;
  GPIO_InitStruct_B0.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct_B0.Pull = GPIO_NOPULL;
  GPIO_InitStruct_B0.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct_B0);

  

}