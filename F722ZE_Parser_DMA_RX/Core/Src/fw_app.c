#include <stdio.h>
#include <string.h>
#include "fw_app.h"

FL_DECLARE_DATA fw_app_t  g_app;
uint8_t                   _g_q1_buf[FW_APP_Q_SIZE];

extern DMA_HandleTypeDef hdma_usart3_rx;

static void on_message_parsed(const void* parser_handle, void* context);

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
      //fl_write_reg_t* wreg = (fl_write_reg_t*)&(proto_mgr->parser_handle.payload);
      // TODO : Write a value to a register.
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
  }

  if (g_app.proto_mgr.out_length > 0)
  {
    HAL_UART_Transmit(g_app.proto_mgr.uart_handle, g_app.proto_mgr.out_buf, g_app.proto_mgr.out_length, FW_APP_PROTO_TX_TIMEOUT);
  }
  g_app.proto_mgr.out_length = 0;
}
