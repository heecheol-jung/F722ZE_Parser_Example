// fw_app.h
// Firmware application.

#ifndef FW_APP_H
#define FW_APP_H

#include "main.h"
#include "usart.h"
#include "gpio.h"

#include "fl_def.h"
#include "fl_queue.h"
#include "fl_txt_message_parser.h"

#define FW_APP_HW_MAJOR             (0)
#define FW_APP_HW_MINOR             (0)
#define FW_APP_HW_REVISION          (1)

#define FW_APP_FW_MAJOR             (0)
#define FW_APP_FW_MINOR             (1)
#define FW_APP_FW_REVISION          (3)

#define FW_APP_UART_HANDLE                                UART_HandleTypeDef*

#define FW_APP_ONE_SEC_INTERVAL     (999) // 1 second

#define FW_APP_PROTO_RX_TIMEOUT     (500)
#define FW_APP_PROTO_TX_TIMEOUT     (500)

#define FW_APP_Q_SIZE               (64)

FL_BEGIN_PACK1

// Protocol manager
typedef struct _fw_app_proto_manager
{
  // UART handle.
  FW_APP_UART_HANDLE      uart_handle;
  fl_queue_t              q;
  fl_txt_msg_parser_t     parser_handle;
  uint8_t                 out_buf[FL_TXT_MSG_MAX_LENGTH];
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
