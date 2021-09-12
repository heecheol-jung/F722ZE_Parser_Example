#include <iostream>
#include <string>
#include <map>
#include "app_common_util.h"
#include "device_simulator.h"
#include "app_constant.h"

static device_simulator _g_ds;

static void message_type();
static void set_bin_msg();
static void set_txt_msg();
static void start();
static void stop();
static void print_ds_info();
static void com_port_name();
static void set_handling_parse_result();
static void set_handling_result_none();
static void set_handling_result_with_callback();
static const char* get_handle_parse_result_string(uint8_t how_to_handle_parse_result);
static void button_press();
static void button_release();

void device_simulator_example()
{
  bool loop = true;
  std::map<std::string, app_func_ptr_t> map_menus;

  map_menus.insert(std::make_pair(APP_LIB_STR_QUIT, (app_func_ptr_t)NULL));
  map_menus[APP_LIB_STR_MESSAGE_TYPE] = message_type;
  map_menus[APP_LIB_STR_START] = start;
  map_menus[APP_LIB_STR_STOP] = stop;
  map_menus[APP_LIB_STR_COM_PORT_NAME] = com_port_name;
  map_menus[STR_RESULT_HANDLING] = set_handling_parse_result;
  map_menus[STR_BUTTON_PRESS] = button_press;
  map_menus[STR_BUTTON_RELEASE] = button_release;

  ds_init(&_g_ds);

  set_bin_msg();
  sprintf(_g_ds.com_port_name, "COM14");
  
  while (loop)
  {
    std::string str_line;

    print_ds_info();
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

  std::cout << "device_simulator_example done." << std::endl << std::endl;
}

static void set_bin_msg()
{
  if (_g_ds.msg_type != APP_LIB_MSG_BIN)
  {
    fl_bin_msg_parser_init(&_g_ds.app_bin_parser);
  }
  _g_ds.msg_type = APP_LIB_MSG_BIN;
  std::cout << "Binary message" << std::endl;
}

static void set_txt_msg()
{
  if (_g_ds.msg_type != APP_LIB_MSG_TXT)
  {
    fl_txt_msg_parser_init(&_g_ds.app_txt_parser);
  }
  _g_ds.msg_type = APP_LIB_MSG_TXT;
  std::cout << "Text message" << std::endl;
}

static void message_type()
{
  bool loop = true;
  std::map<std::string, app_func_ptr_t> map_menus;

  map_menus.insert(std::make_pair(APP_LIB_STR_QUIT, (app_func_ptr_t)NULL));
  map_menus[APP_LIB_STR_BIN_MSG] = set_bin_msg;
  map_menus[APP_LIB_STR_TXT_MSG] = set_txt_msg;

  while (loop)
  {
    std::string str_line;

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
        loop = false;
      }
      else
      {
        std::cout << APP_LIB_STR_UNKNOWN_COMMANDD << std::endl;
      }
    }
  }
}

static void start()
{
  if (strlen(_g_ds.com_port_name) > 0)
  {
    ds_start(&_g_ds);
  }
  else
  {
    std::cout << "Invalid COM port name(set a valid COM port name)." << std::endl;
  }
}

static void stop()
{
  ds_stop(&_g_ds);
}

static void print_ds_info()
{
  std::cout << "==============================================================" << std::endl;
  
  if (_g_ds.msg_type == APP_LIB_MSG_BIN)
  {
    std::cout << "Binary message" << std::endl;
  }
  else if (_g_ds.msg_type == APP_LIB_MSG_TXT)
  {
    std::cout << "Text message" << std::endl;
  }
  else
  {
    std::cout << "Message type unknown!!!" << std::endl;
  }

  if (strlen(_g_ds.com_port_name) > 0)
  {
    std::cout << "COM port : " << _g_ds.com_port_name << std::endl;
  }
  else
  {
    std::cout << "COM port : NOT SET" << std::endl;
  }

  if (ds_is_serial_open(&_g_ds) == 1)
  {
    std::cout << "COM port opened" << std::endl;
  }
  else
  {
    std::cout << "COM port closed" << std::endl;
  }

  std::cout << "Result handling : " << get_handle_parse_result_string(_g_ds.how_to_handle_parse_result) << std::endl;
  std::cout << "==============================================================" << std::endl << std::endl;
}

static void com_port_name()
{
  std::string str_line;

  std::cout << "COM port name : ";

  std::getline(std::cin, str_line);
  if (str_line.size() > 0)
  {
    sprintf(_g_ds.com_port_name, "%s", str_line.c_str());
  }
  else
  {
    std::cout << "Invalid COM port name!!!" << std::endl;
  }
}

static void set_handling_parse_result()
{
  bool loop = true;
  std::map<std::string, app_func_ptr_t> map_menus;

  map_menus.insert(std::make_pair(APP_LIB_STR_QUIT, (app_func_ptr_t)NULL));
  map_menus[STR_NONE] = set_handling_result_none;
  map_menus[STR_CALLBACK] = set_handling_result_with_callback;

  while (loop)
  {
    std::string str_line;

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
        loop = false;
      }
      else
      {
        std::cout << APP_LIB_STR_UNKNOWN_COMMANDD << std::endl;
      }
    }
  }
}

static void set_handling_result_none()
{
  if (_g_ds.how_to_handle_parse_result != APP_LIB_PARSE_RESULT_NONE)
  {
    ds_set_how_to_handle_parse_result(&_g_ds, APP_LIB_PARSE_RESULT_NONE);
  }
  
  std::cout << "Result handling : " << get_handle_parse_result_string(_g_ds.how_to_handle_parse_result) << std::endl;
}

static void set_handling_result_with_callback()
{
  if (_g_ds.how_to_handle_parse_result != APP_LIB_PARSE_REUSLT_WITH_CALLBACK)
  {
    ds_set_how_to_handle_parse_result(&_g_ds, APP_LIB_PARSE_REUSLT_WITH_CALLBACK);
  }

  std::cout << "Result handling : " << get_handle_parse_result_string(_g_ds.how_to_handle_parse_result) << std::endl;
}

static const char* get_handle_parse_result_string(uint8_t how_to_handle_parse_result)
{
  switch (how_to_handle_parse_result)
  {
  case APP_LIB_PARSE_RESULT_NONE:
    return "None";

  case APP_LIB_PARSE_REUSLT_WITH_CALLBACK:
    return "Callback";

  case APP_LIB_PARSE_RESULT_WITH_OUT_PARAM:
    return "Out param";
  }

  return "NOT SET";
}

static void button_press()
{
  ds_button_press(&_g_ds);
}

static void button_release()
{
  ds_button_release(&_g_ds);
}