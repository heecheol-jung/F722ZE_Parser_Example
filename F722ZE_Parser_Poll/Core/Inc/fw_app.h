// fw_app.h
// Firmware application.

#ifndef FW_APP_H
#define FW_APP_H

#include "main.h"
#include "usart.h"
#include "gpio.h"

#include "fl_def.h"

#define FW_APP_TXT_PARSER           (0)
#define FW_APP_BIN_PARSER           (1)

//#define FW_APP_PARSER               FW_APP_TXT_PARSER
#define FW_APP_PARSER               FW_APP_BIN_PARSER

#if FW_APP_PARSER == FW_APP_TXT_PARSER
#include "fl_txt_message_parser.h"
#else
#include "fl_bin_message_parser.h"
#include "fl_util.h"
#endif

#define FW_APP_HW_MAJOR             (0)
#define FW_APP_HW_MINOR             (0)
#define FW_APP_HW_REVISION          (1)

#define FW_APP_FW_MAJOR             (0)
#define FW_APP_FW_MINOR             (1)
#define FW_APP_FW_REVISION          (1)

#define FW_APP_UART_HANDLE                                UART_HandleTypeDef*

#define FW_APP_ONE_SEC_INTERVAL     (999) // 1 second

#define FW_APP_PROTO_RX_TIMEOUT     (500)
#define FW_APP_PROTO_TX_TIMEOUT     (500)

#define FW_APP_TX_MSG_LENGTH        (64)

FL_BEGIN_PACK1

// Protocol manager
typedef struct _fw_app_proto_manager
{
  // UART handle.
  FW_APP_UART_HANDLE      uart_handle;
#if FW_APP_PARSER == FW_APP_TXT_PARSER
  fl_txt_msg_parser_t     parser_handle;
#else
  fl_bin_msg_parser_t     parser_handle;
#endif
  uint8_t                 out_buf[FW_APP_TX_MSG_LENGTH];
  uint8_t                 out_length;
  uint8_t                 rx_buf[1];
} fw_app_proto_manager_t;

// Firmware application manager.
typedef struct _fw_app
{
  uint32_t                device_id;

  // Current tick count.
  volatile uint32_t       tick;
  fw_app_proto_manager_t  proto_mgr;
} fw_app_t;

FL_END_PACK

FL_BEGIN_DECLS

FL_DECLARE_DATA extern fw_app_t g_app;

FL_DECLARE(void) fw_app_init(void);
FL_DECLARE(void) fw_app_hw_init(void);
FL_DECLARE(void) fw_app_systick(void);

FL_END_DECLS

#endif
