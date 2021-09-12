#pragma once

#include <Windows.h>
#include "fl_bin_message_parser.h"
#include "app_common.h"

typedef struct
{
  char                      tx_buf[APP_LIB_MAX_BUF_LEN];
  uint32_t                  tx_len;
  char                      rx_buf[APP_LIB_MAX_BUF_LEN];
  uint32_t                  rx_len;
  HANDLE                    serial_handle;
  HANDLE                    thread_handle;
  unsigned int              thread_id;
  uint8_t                   loop;
  char                      com_port_name[APP_LIB_COM_PORT_NAME_BUF_LEN];

  fl_bin_msg_parser_t       app_bin_parser;
} host_simulator_t;

APP_LIB_BEGIN_DECLS

// Initialize a host simulator.
void hs_init(host_simulator_t* handle);

// Start a host simulator.
int hs_start(host_simulator_t* handle);

// Stop a host simulator.
int hs_stop(host_simulator_t* handle);

uint8_t hs_is_serial_open(host_simulator_t* handle);

int hs_send_packet(host_simulator_t* handle, uint8_t* buf, uint16_t len);

APP_LIB_END_DECLS