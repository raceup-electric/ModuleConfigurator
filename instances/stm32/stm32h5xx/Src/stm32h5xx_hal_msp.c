#include "main.h"

void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  /* USER CODE END MspInit 0 */

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
