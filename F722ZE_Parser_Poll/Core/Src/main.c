/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fw_app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void txt_message_processing(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  fw_app_init();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */
  fw_app_hw_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    txt_message_processing();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
static void txt_message_processing(void)
{
  fl_status_t ret = 0;

  if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE))
  {
    HAL_UART_Receive(&huart3, g_app.proto_mgr.rx_buf, 1, FW_APP_PROTO_RX_TIMEOUT);
    ret = fl_txt_msg_parser_parse_command(&g_app.proto_mgr.parser_handle, g_app.proto_mgr.rx_buf[0], NULL);
    if ((ret == FL_OK) &&
        (g_app.proto_mgr.parser_handle.device_id == g_app.device_id))
    {
      g_app.proto_mgr.out_length = 0;
      switch (g_app.proto_mgr.parser_handle.msg_id)
      {
      case FL_MSG_ID_READ_HW_VERSION:
        // Version string.
        g_app.proto_mgr.out_length = sprintf((char*)g_app.proto_mgr.out_buf, "%s %ld,%d,%d.%d.%d%c",
            fl_txt_msg_get_message_name(g_app.proto_mgr.parser_handle.msg_id),
            g_app.proto_mgr.parser_handle.device_id,
            FL_OK,
            FW_APP_HW_MAJOR, FW_APP_HW_MINOR, FW_APP_HW_REVISION,
            FL_TXT_MSG_TAIL);
        break;

      case FL_MSG_ID_READ_FW_VERSION:
        // Version string.
        g_app.proto_mgr.out_length = sprintf((char*)g_app.proto_mgr.out_buf, "%s %ld,%d,%d.%d.%d%c",
            fl_txt_msg_get_message_name(g_app.proto_mgr.parser_handle.msg_id),
            g_app.proto_mgr.parser_handle.device_id,
            FL_OK,
            FW_APP_FW_MAJOR, FW_APP_FW_MINOR, FW_APP_FW_REVISION,
            FL_TXT_MSG_TAIL);
        break;

      case FL_MSG_ID_BOOT_MODE:
        if (g_app.proto_mgr.parser_handle.arg_count == 1)
        {
          g_app.proto_mgr.out_length = sprintf((char*)g_app.proto_mgr.out_buf, "%s %ld,%d%c",
                      fl_txt_msg_get_message_name(g_app.proto_mgr.parser_handle.msg_id),
                      g_app.proto_mgr.parser_handle.device_id,
                      FL_OK,
                      FL_TXT_MSG_TAIL);
        }

        if (g_app.proto_mgr.out_length == 0)
        {
          g_app.proto_mgr.out_length = sprintf((char*)g_app.proto_mgr.out_buf, "%s %ld,%d%c",
              fl_txt_msg_get_message_name(g_app.proto_mgr.parser_handle.msg_id),
              g_app.proto_mgr.parser_handle.device_id,
              FL_ERROR,
              FL_TXT_MSG_TAIL);
        }
        break;

      case FL_MSG_ID_RESET:
        g_app.proto_mgr.out_length = sprintf((char*)g_app.proto_mgr.out_buf, "%s %ld,%d%c",
                    fl_txt_msg_get_message_name(g_app.proto_mgr.parser_handle.msg_id),
                    g_app.proto_mgr.parser_handle.device_id,
                    FL_OK,
                    FL_TXT_MSG_TAIL);
        break;

      case FL_MSG_ID_READ_REGISTER:
        if (g_app.proto_mgr.parser_handle.arg_count == 1)
        {
          fl_read_reg_t* rreg = (fl_read_reg_t*)&(g_app.proto_mgr.parser_handle.payload);

          // TODO : Read a value from a register.
          g_app.proto_mgr.out_length = sprintf((char*)g_app.proto_mgr.out_buf, "%s %ld,%d,%ld,%d%c",
                      fl_txt_msg_get_message_name(g_app.proto_mgr.parser_handle.msg_id),
                      g_app.proto_mgr.parser_handle.device_id,
                      FL_OK,
                      rreg->address,
                      10,
                      FL_TXT_MSG_TAIL);
        }

        if (g_app.proto_mgr.out_length == 0)
        {
          g_app.proto_mgr.out_length = sprintf((char*)g_app.proto_mgr.out_buf, "%s %ld,%d%c",
              fl_txt_msg_get_message_name(g_app.proto_mgr.parser_handle.msg_id),
              g_app.proto_mgr.parser_handle.device_id,
              FL_ERROR,
              FL_TXT_MSG_TAIL);
        }
        break;

      case FL_MSG_ID_WRITE_REGISTER:
        if (g_app.proto_mgr.parser_handle.arg_count == 2)
        {
          //fl_write_reg_t* wreg = (fl_write_reg_t*)&(g_app.proto_mgr.parser_handle.payload);
          // TODO : Write a value to a register.
          g_app.proto_mgr.out_length = sprintf((char*)g_app.proto_mgr.out_buf, "%s %ld,%d%c",
                      fl_txt_msg_get_message_name(g_app.proto_mgr.parser_handle.msg_id),
                      g_app.proto_mgr.parser_handle.device_id,
                      FL_OK,
                      FL_TXT_MSG_TAIL);
        }

        if (g_app.proto_mgr.out_length == 0)
        {
          g_app.proto_mgr.out_length = sprintf((char*)g_app.proto_mgr.out_buf, "%s %ld,%d%c",
              fl_txt_msg_get_message_name(g_app.proto_mgr.parser_handle.msg_id),
              g_app.proto_mgr.parser_handle.device_id,
              FL_ERROR,
              FL_TXT_MSG_TAIL);
        }
        break;
      }

      if (g_app.proto_mgr.out_length > 0)
      {
        HAL_UART_Transmit(g_app.proto_mgr.uart_handle, g_app.proto_mgr.out_buf, g_app.proto_mgr.out_length, FW_APP_PROTO_TX_TIMEOUT);
      }
      g_app.proto_mgr.out_length = 0;

      fl_txt_msg_parser_clear(&g_app.proto_mgr.parser_handle);
    }
  }

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
