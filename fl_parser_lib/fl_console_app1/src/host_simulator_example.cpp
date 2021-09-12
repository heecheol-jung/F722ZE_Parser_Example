#include <iostream>
#include <string>
#include <map>
#include "app_common_util.h"
#include "host_simulator.h"
#include "app_constant.h"

#define DEVICE_ID     (1)
#define WAIT_TIME     (1000)

static host_simulator_t   _g_hs;
static uint8_t            _packet_buf[APP_LIB_MAX_BUF_LEN];
static fl_bin_msg_full_t* _msg_full;
static uint16_t           _len;
static uint8_t            _rx_msg_buf[APP_LIB_MAX_BUF_LEN];
static uint8_t            _seq_num = 0;

static void start();
static void stop();
static void print_hs_info();
static void com_port_name();
static void bin_protocol_test();
static void read_hardware_bin_command();
static void read_firmware_bin_command();
static void boot_mode_command();
static void reset_command();
static void read_register_command();
static void write_register_command();
static void write_register_command_with_fail_expectation();
static void wait_for_response();
static void print_parsed_result(host_simulator_t* handle);
static uint8_t get_next_sequence_num();

void host_simulator_example()
{
  bool loop = true;
  std::map<std::string, app_func_ptr_t> map_menus;

  map_menus.insert(std::make_pair(APP_LIB_STR_QUIT, (app_func_ptr_t)NULL));
  map_menus[APP_LIB_STR_START] = start;
  map_menus[APP_LIB_STR_STOP] = stop;
  map_menus[APP_LIB_STR_COM_PORT_NAME] = com_port_name;
  map_menus[STR_BIN_PROTOCOL_TEST] = bin_protocol_test;

  _msg_full = (fl_bin_msg_full_t*)_packet_buf;
  hs_init(&_g_hs);

  sprintf(_g_hs.com_port_name, "COM3");

  while (loop)
  {
    std::string str_line;

    print_hs_info();
    print_menu(&map_menus);

    std::getline(std::cin, str_line);
    if (str_line == APP_LIB_STR_QUIT)
    {
      loop = false;
    }
    else
    {
      if (map_menus.count(str_line) > 0)
      {
        map_menus[str_line]();
      }
      else
      {
        std::cout << APP_LIB_STR_UNKNOWN_COMMANDD << std::endl;
      }
    }
  }

  stop();

  std::cout << "host_simulator_example done." << std::endl << std::endl;
}

static void start()
{
  if (strlen(_g_hs.com_port_name) > 0)
  {
    hs_start(&_g_hs);
  }
  else
  {
    std::cout << "Invalid COM port name(set a valid COM port name)." << std::endl;
  }
}

static void stop()
{
  hs_stop(&_g_hs);
}

static void print_hs_info()
{
  std::cout << "==============================================================" << std::endl;

  if (strlen(_g_hs.com_port_name) > 0)
  {
    std::cout << "COM port : " << _g_hs.com_port_name << std::endl;
  }
  else
  {
    std::cout << "COM port : NOT SET" << std::endl;
  }

  if (hs_is_serial_open(&_g_hs) == 1)
  {
    std::cout << "COM port opened" << std::endl;
  }
  else
  {
    std::cout << "COM port closed" << std::endl;
  }

  std::cout << "==============================================================" << std::endl << std::endl;
}

static void com_port_name()
{
  std::string str_line;

  std::cout << "COM port name : ";

  std::getline(std::cin, str_line);
  if (str_line.size() > 0)
  {
    sprintf(_g_hs.com_port_name, "%s", str_line.c_str());
  }
  else
  {
    std::cout << "Invalid COM port name!!!" << std::endl;
  }
}

static void bin_protocol_test()
{
  if (hs_is_serial_open(&_g_hs) != 1)
  {
    std::cout << "COM port is NOT OPENED" << std::endl;
    return;
  }
  
  std::cout << "Read hardware version" << std::endl;
  read_hardware_bin_command();
  std::cout << std::endl;
  
  std::cout << "Read firmware version" << std::endl;
  read_firmware_bin_command();
  std::cout << std::endl;

  std::cout << "Boot mode" << std::endl;
  boot_mode_command();
  std::cout << std::endl;

  std::cout << "Reset" << std::endl;
  reset_command();
  std::cout << std::endl;

  std::cout << "Read register" << std::endl;
  read_register_command();
  std::cout << std::endl;

  std::cout << "Write register" << std::endl;
  write_register_command();
  std::cout << std::endl;

  std::cout << "Write register with fail expectation" << std::endl;
  write_register_command_with_fail_expectation();
  std::cout << std::endl;
}

static void read_hardware_bin_command()
{
  memset(_packet_buf, 0, sizeof(_packet_buf));

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_HW_VERSION;
  _msg_full->header.flag1.sequence_num = get_next_sequence_num();
  _msg_full->header.flag1.return_expected = FL_TRUE;
  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  hs_send_packet(&_g_hs, _packet_buf, _len);
  wait_for_response();
}

static void read_firmware_bin_command()
{
  memset(_packet_buf, 0, sizeof(_packet_buf));

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_FW_VERSION;
  _msg_full->header.flag1.sequence_num = get_next_sequence_num();
  _msg_full->header.flag1.return_expected = FL_TRUE;
  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  hs_send_packet(&_g_hs, _packet_buf, _len);
  wait_for_response();
}

static void boot_mode_command()
{
  memset(_packet_buf, 0, sizeof(_packet_buf));

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_BOOT_MODE;
  _msg_full->header.flag1.sequence_num = get_next_sequence_num();
  _msg_full->header.flag1.return_expected = FL_TRUE;

  fl_boot_mode_t* boot_mode = (fl_boot_mode_t*)&_msg_full->payload;
  boot_mode->boot_mode = FL_BMODE_BOOTLOADER;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  hs_send_packet(&_g_hs, _packet_buf, _len);
  wait_for_response();
}

static void reset_command()
{
  memset(_packet_buf, 0, sizeof(_packet_buf));

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_RESET;
  _msg_full->header.flag1.sequence_num = get_next_sequence_num();
  _msg_full->header.flag1.return_expected = FL_TRUE;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  hs_send_packet(&_g_hs, _packet_buf, _len);
  wait_for_response();
}

static void read_register_command()
{
  memset(_packet_buf, 0, sizeof(_packet_buf));

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_REGISTER;
  _msg_full->header.flag1.sequence_num = get_next_sequence_num();
  _msg_full->header.flag1.return_expected = FL_TRUE;

  fl_read_reg_t* rreg = (fl_read_reg_t*)&_msg_full->payload;
  rreg->address = 1;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  hs_send_packet(&_g_hs, _packet_buf, _len);
  wait_for_response();
}

static void write_register_command()
{
  memset(_packet_buf, 0, sizeof(_packet_buf));

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_WRITE_REGISTER;
  _msg_full->header.flag1.sequence_num = get_next_sequence_num();
  _msg_full->header.flag1.return_expected = FL_TRUE;

  fl_write_reg_t* wreg = (fl_write_reg_t*)&_msg_full->payload;
  wreg->address = 3;
  wreg->value = 4;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  hs_send_packet(&_g_hs, _packet_buf, _len);
  wait_for_response();
}

static void write_register_command_with_fail_expectation()
{
  memset(_packet_buf, 0, sizeof(_packet_buf));

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_WRITE_REGISTER;
  _msg_full->header.flag1.sequence_num = get_next_sequence_num();
  _msg_full->header.flag1.return_expected = FL_TRUE;

  fl_write_reg_t* wreg = (fl_write_reg_t*)&_msg_full->payload;
  wreg->address = 1;
  wreg->value = 4;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  hs_send_packet(&_g_hs, _packet_buf, _len);
  wait_for_response();
}

static void wait_for_response()
{
  ULONGLONG dwTickStart = GetTickCount64();
  DWORD dwReadBytes = 0;
  fl_status_t ret = FL_OK;

  std::cout << "Waiting a response..." << std::endl;
  fl_bin_msg_parser_clear(&_g_hs.app_bin_parser);

  while (true)
  {
    if ((ReadFile(_g_hs.serial_handle, _g_hs.rx_buf, sizeof(_g_hs.rx_buf), &dwReadBytes, NULL) == TRUE) &&
        (dwReadBytes > 0))
    {
      for (DWORD i = 0; i < dwReadBytes; i++)
      {
        ret = fl_bin_msg_parser_parse(&_g_hs.app_bin_parser, _g_hs.rx_buf[i], NULL);
        if (ret != FL_BIN_MSG_PARSER_PARSING)
        {
          if (ret == FL_OK)
          {
            print_parsed_result(&_g_hs);
          }
          else 
          {
            std::cout << "Response parsing failed." << std::endl;
          }

          fl_bin_msg_parser_clear(&_g_hs.app_bin_parser);
          return;
        }
      }
      
    }
    if ((GetTickCount64() - dwTickStart) > WAIT_TIME)
    {
      std::cout << "Response wait timeout." << std::endl;
      break;
    }
  }

  return;
}

static void print_parsed_result(host_simulator_t* handle)
{
  fl_bin_msg_header_t* header = (fl_bin_msg_header_t*)&_g_hs.app_bin_parser.buf[1];
  fl_bin_msg_full_t* rx_msg_full = (fl_bin_msg_full_t*)_g_hs.app_bin_parser.buf;
  
  switch (header->message_id)
  {
    case FL_MSG_ID_READ_HW_VERSION:
    {
      fl_hw_ver_resp_t* hw_ver = (fl_hw_ver_resp_t*)&(rx_msg_full->payload);
      hw_ver->version[header->length - 6] = 0;
      std::cout << "HW version : " << hw_ver->version << std::endl;
      break;
    }

    case FL_MSG_ID_READ_FW_VERSION:
    {
      fl_fw_ver_resp_t* fw_ver = (fl_fw_ver_resp_t*)&(rx_msg_full->payload);
      fw_ver->version[header->length - 6] = 0;
      std::cout << "FW version : " << fw_ver->version << std::endl;
      break;
    }

    case FL_MSG_ID_BOOT_MODE:
      if (header->error == FL_OK)
      {
        std::cout << "Boot mode OK" << std::endl;
      }
      else
      {
        std::cout << "Boot mode FAIL" << std::endl;
      }
      break;

    case FL_MSG_ID_RESET:
      if (header->error == FL_OK)
      {
        std::cout << "Reset OK" << std::endl;
      }
      else
      {
        std::cout << "Reset FAIL" << std::endl;
      }
      break;

    case FL_MSG_ID_READ_REGISTER:
    {
      fl_read_reg_resp_t* rreg = (fl_read_reg_resp_t*)&(rx_msg_full->payload);
      std::cout << "Address : " << rreg->address << ", Value : " << rreg->value << std::endl;
      break;
    }

    case FL_MSG_ID_WRITE_REGISTER:
      if (header->error == FL_OK)
      {
        std::cout << "Write register OK" << std::endl;
      }
      else
      {
        std::cout << "Write register FAIL" << std::endl;
      }
      break;
  }
}

static uint8_t get_next_sequence_num()
{
  if (_seq_num < FL_BIN_MSG_MAX_SEQUENCE)
  {
    _seq_num++;
  }
  else
  {
    _seq_num = 1;
  }

  return _seq_num;
}
