#include <stdio.h>
#include <string.h>
#include "fw_app.h"

FL_DECLARE_DATA fw_app_t  g_app;
uint8_t                   _g_q1_buf[FW_APP_Q_SIZE];

extern DMA_HandleTypeDef hdma_usart3_rx;

static void on_message_parsed(const void* parser_handle, void* context);
static void dma_transmit(void);

// DO NOT ANY HARDWARE SETTINGS.
FL_DECLARE(void) fw_app_init(void)
{
  memset(&g_app, 0, sizeof(g_app));
  memset(_g_q1_buf, 0, sizeof(_g_q1_buf));

  fl_dma_q_init(&g_app.proto_mgr.q, &hdma_usart3_rx, _g_q1_buf, sizeof(_g_q1_buf));

  g_app.proto_mgr.uart_handle = &huart3;
  g_app.proto_mgr.parser_handle.on_parsed_callback = on_message_parsed;
  g_app.proto_mgr.parser_handle.context = (void*)&g_app;
}

FL_DECLARE(void) fw_app_hw_init(void)
{
  g_app.device_id = 1;

  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  HAL_UART_Receive_DMA(&huart3, _g_q1_buf, sizeof(_g_q1_buf));
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

#if FW_APP_PARSER == FW_APP_TXT_PARSER
static void on_message_parsed(const void* parser_handle, void* context)
{
  fl_txt_msg_parser_t*    txt_parser = (fl_txt_msg_parser_t*)parser_handle;
  fw_app_proto_manager_t* proto_mgr = &((fw_app_t*)context)->proto_mgr;

  // Ignore the parsed message.
  if (txt_parser->device_id != ((fw_app_t*)context)->device_id)
  {
    return;
  }

  proto_mgr->out_length = 0;
  switch (proto_mgr->parser_handle.msg_id)
  {
  case FL_MSG_ID_READ_HW_VERSION:
    // Version string.
    proto_mgr->out_length = sprintf((char*)proto_mgr->out_buf, "%s %ld,%d,%d.%d.%d%c",
        fl_txt_msg_get_message_name(proto_mgr->parser_handle.msg_id),
        proto_mgr->parser_handle.device_id,
        FL_OK,
        FW_APP_HW_MAJOR, FW_APP_HW_MINOR, FW_APP_HW_REVISION,
        FL_TXT_MSG_TAIL);
    break;

  case FL_MSG_ID_READ_FW_VERSION:
    // Version string.
    proto_mgr->out_length = sprintf((char*)proto_mgr->out_buf, "%s %ld,%d,%d.%d.%d%c",
        fl_txt_msg_get_message_name(proto_mgr->parser_handle.msg_id),
        proto_mgr->parser_handle.device_id,
        FL_OK,
        FW_APP_FW_MAJOR, FW_APP_FW_MINOR, FW_APP_FW_REVISION,
        FL_TXT_MSG_TAIL);
    break;

  case FL_MSG_ID_BOOT_MODE:
    if (proto_mgr->parser_handle.arg_count == 1)
    {
      proto_mgr->out_length = sprintf((char*)proto_mgr->out_buf, "%s %ld,%d%c",
                  fl_txt_msg_get_message_name(proto_mgr->parser_handle.msg_id),
                  proto_mgr->parser_handle.device_id,
                  FL_OK,
                  FL_TXT_MSG_TAIL);
    }

    if (proto_mgr->out_length == 0)
    {
      proto_mgr->out_length = sprintf((char*)proto_mgr->out_buf, "%s %ld,%d%c",
          fl_txt_msg_get_message_name(proto_mgr->parser_handle.msg_id),
          proto_mgr->parser_handle.device_id,
          FL_ERROR,
          FL_TXT_MSG_TAIL);
    }
    break;

  case FL_MSG_ID_RESET:
    proto_mgr->out_length = sprintf((char*)proto_mgr->out_buf, "%s %ld,%d%c",
                fl_txt_msg_get_message_name(proto_mgr->parser_handle.msg_id),
                proto_mgr->parser_handle.device_id,
                FL_OK,
                FL_TXT_MSG_TAIL);
    break;

  case FL_MSG_ID_READ_REGISTER:
    if (proto_mgr->parser_handle.arg_count == 1)
    {
      fl_read_reg_t* rreg = (fl_read_reg_t*)&(proto_mgr->parser_handle.payload);

      // TODO : Read a value from a register.
      proto_mgr->out_length = sprintf((char*)proto_mgr->out_buf, "%s %ld,%d,%ld,%d%c",
                  fl_txt_msg_get_message_name(proto_mgr->parser_handle.msg_id),
                  proto_mgr->parser_handle.device_id,
                  FL_OK,
                  rreg->address,
                  10,
                  FL_TXT_MSG_TAIL);
    }

    if (proto_mgr->out_length == 0)
    {
      proto_mgr->out_length = sprintf((char*)proto_mgr->out_buf, "%s %ld,%d%c",
          fl_txt_msg_get_message_name(proto_mgr->parser_handle.msg_id),
          proto_mgr->parser_handle.device_id,
          FL_ERROR,
          FL_TXT_MSG_TAIL);
    }
    break;

  case FL_MSG_ID_WRITE_REGISTER:
    if (proto_mgr->parser_handle.arg_count == 2)
    {
      fl_write_reg_t* wreg = (fl_write_reg_t*)&(proto_mgr->parser_handle.payload);
      // TODO : Write a value to a register.

      // For fail response test.
      if ((wreg->address == 3) &&
          (wreg->value == 4))
      {
        proto_mgr->out_length = sprintf((char*)proto_mgr->out_buf, "%s %ld,%d%c",
            fl_txt_msg_get_message_name(proto_mgr->parser_handle.msg_id),
            proto_mgr->parser_handle.device_id,
            FL_OK,
            FL_TXT_MSG_TAIL);
      }
    }

    if (proto_mgr->out_length == 0)
    {
      proto_mgr->out_length = sprintf((char*)proto_mgr->out_buf, "%s %ld,%d%c",
          fl_txt_msg_get_message_name(proto_mgr->parser_handle.msg_id),
          proto_mgr->parser_handle.device_id,
          FL_ERROR,
          FL_TXT_MSG_TAIL);
    }
    break;
  }

  if (proto_mgr->out_length > 0)
  {
    dma_transmit();
  }
}
#else
static void on_message_parsed(const void* parser_handle, void* context)
{
  fl_bin_msg_parser_t*    bin_parser = (fl_bin_msg_parser_t*)parser_handle;
  fw_app_proto_manager_t* proto_mgr = &((fw_app_t*)context)->proto_mgr;
  fl_bin_msg_header_t*    header = (fl_bin_msg_header_t*)&bin_parser->buf[1];
  fl_bin_msg_full_t*      rx_msg_full = (fl_bin_msg_full_t*)bin_parser->buf;
  fl_bin_msg_full_t*      tx_msg_full = (fl_bin_msg_full_t*)proto_mgr->out_buf;

  if (header->device_id != g_app.device_id)
  {
    fl_bin_msg_parser_clear(bin_parser);
    return;
  }

  tx_msg_full->header.device_id = header->device_id;
  tx_msg_full->header.message_id = header->message_id;
  tx_msg_full->header.flag1.sequence_num = header->flag1.sequence_num;
  tx_msg_full->header.flag1.return_expected = FL_FALSE;
  tx_msg_full->header.error = FL_OK;

  proto_mgr->out_length = 0;

  switch (header->message_id)
  {
  case FL_MSG_ID_READ_HW_VERSION:
  {
    fl_hw_ver_resp_t* hw_ver = (fl_hw_ver_resp_t*)&(tx_msg_full->payload);
    sprintf(hw_ver->version, "%d.%d.%d", FW_APP_HW_MAJOR, FW_APP_HW_MINOR, FW_APP_HW_REVISION);
    proto_mgr->out_length = fl_bin_msg_build_response((uint8_t*)proto_mgr->out_buf, sizeof(proto_mgr->out_buf));
    break;
  }

  case FL_MSG_ID_READ_FW_VERSION:
  {
    fl_fw_ver_resp_t* fw_ver = (fl_fw_ver_resp_t*)&(tx_msg_full->payload);
    sprintf(fw_ver->version, "%d.%d.%d", FW_APP_FW_MAJOR, FW_APP_FW_MINOR, FW_APP_FW_REVISION);
    proto_mgr->out_length = fl_bin_msg_build_response((uint8_t*)proto_mgr->out_buf, sizeof(proto_mgr->out_buf));
    break;
  }

  case FL_MSG_ID_BOOT_MODE:
  {
    fl_boot_mode_t* bmode = (fl_boot_mode_t*)&(rx_msg_full->payload);
    if ((bmode->boot_mode != FL_BMODE_APP) &&
        (bmode->boot_mode != FL_BMODE_BOOTLOADER))
    {
      tx_msg_full->header.error = FL_ERROR;
    }
    proto_mgr->out_length = fl_bin_msg_build_response((uint8_t*)proto_mgr->out_buf, sizeof(proto_mgr->out_buf));
    break;
  }

  case FL_MSG_ID_RESET:
    proto_mgr->out_length = fl_bin_msg_build_response((uint8_t*)proto_mgr->out_buf, sizeof(proto_mgr->out_buf));
    break;

  case FL_MSG_ID_READ_REGISTER:
  {
    fl_read_reg_t* rreg = (fl_read_reg_t*)&(rx_msg_full->payload);
    // TODO : Check the validity of an address.
    fl_read_reg_resp_t* rreg_resp = (fl_read_reg_resp_t*)&(tx_msg_full->payload);
    rreg_resp->address = rreg->address;
    rreg_resp->value = 10;

    proto_mgr->out_length = fl_bin_msg_build_response((uint8_t*)proto_mgr->out_buf, sizeof(proto_mgr->out_buf));
    break;
  }

  case FL_MSG_ID_WRITE_REGISTER:
  {
    fl_write_reg_t* wreg = (fl_write_reg_t*)&(rx_msg_full->payload);
    // TODO : Check the validity of an address.
    // TODO : Check the validity of a value.

    // For fail response test.
    if ((wreg->address != 3) ||
        (wreg->value != 4))
    {
      tx_msg_full->header.error = FL_ERROR;
    }

    proto_mgr->out_length = fl_bin_msg_build_response((uint8_t*)proto_mgr->out_buf, sizeof(proto_mgr->out_buf));
    break;
  }
  }

  if (proto_mgr->out_length > 0)
  {
    dma_transmit();
  }
}
#endif

static void dma_transmit(void)
{
  HAL_StatusTypeDef ret;
  uint8_t wait_count = 0;

  ret = HAL_UART_Transmit_DMA(&huart3, g_app.proto_mgr.out_buf, g_app.proto_mgr.out_length);

  if (ret == HAL_OK)
  {
    // TODO : Asynchronous send.
    while (1)
    {
      if (huart3.gState == HAL_UART_STATE_READY)
      {
        break;
      }
      HAL_Delay(1);
      wait_count++;
      if (wait_count >= FW_APP_DMA_TX_MAX_COUNT)
      {
        break;
      }
    }
  }

  g_app.proto_mgr.out_length = 0;
}

