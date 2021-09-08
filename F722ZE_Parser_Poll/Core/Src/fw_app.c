#include <stdio.h>
#include <string.h>
#include "fw_app.h"

FL_DECLARE_DATA fw_app_t g_app;

// DO NOT ANY HARDWARE SETTINGS.
FL_DECLARE(void) fw_app_init(void)
{
  memset(&g_app, 0, sizeof(g_app));

  g_app.proto_mgr.uart_handle = &huart3;
}

FL_DECLARE(void) fw_app_hw_init(void)
{
  g_app.device_id = 1;

  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
}

FL_DECLARE(void) fw_app_systick(void)
{
  g_app.tick++;
  // Do some work every 1 second.
  if (g_app.tick >= FW_APP_ONE_SEC_INTERVAL)
  {
    // LED1 toggle.
    HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    g_app.tick = 0;
  }
}

