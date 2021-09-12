#include <limits.h>
#include <iostream>
#include <string>
#include <map>
#include <list>
#include "fl_message_def.h"
#include "fl_bin_message.h"
#include "app_common_util.h"
#include "app_constant.h"

using namespace std;

static void show_struct_info(void);
static void show_ascii_protocol_examples(void);

void general_example()
{
  bool loop = true;
  std::map<std::string, app_func_ptr_t> map_menus;

  map_menus.insert(std::make_pair(APP_LIB_STR_QUIT, (app_func_ptr_t)NULL));
  map_menus[STR_SHOW_STRUCT_INFO] = show_struct_info;
  map_menus[STR_SHOW_ASCII_PROTOCOL_EXAMPLES] = show_ascii_protocol_examples;

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
      }
      else
      {
        std::cout << APP_LIB_STR_UNKNOWN_COMMANDD << std::endl;
      }
    }
  }

  std::cout << "general_example done." << std::endl << std::endl;
}

static void show_struct_info(void)
{
  std::map<std::string, uint32_t> map_structs;
  std::list<std::string> min_list;
  std::list<std::string> max_list;
  uint32_t min_value = 0xffffffff;
  uint32_t max_value = 0;

  map_structs["fl_hw_ver_t"] = sizeof(fl_hw_ver_resp_t);
  map_structs["fl_fw_ver_t"] = sizeof(fl_fw_ver_resp_t);
  map_structs["fl_read_reg_t"] = sizeof(fl_read_reg_t);
  map_structs["fl_read_reg_resp_t"] = sizeof(fl_read_reg_resp_t);
  map_structs["fl_write_reg_t"] = sizeof(fl_write_reg_t);
  map_structs["fl_boot_mode_t"] = sizeof(fl_boot_mode_t);
  map_structs["fl_bin_msg_header_t"] = sizeof(fl_bin_msg_header_t);

  std::map<std::string, uint32_t>::iterator it;

  cout << endl;
  for (it = map_structs.begin(); it != map_structs.end(); it++)
  {
    min_value = min(min_value, it->second);
    max_value = max(max_value, it->second);

    cout << it->first << " = " << it->second << endl;
  }
  cout << endl;

  for (it = map_structs.begin(); it != map_structs.end(); it++)
  {
    if (it->second == min_value)
    {
      min_list.push_back(it->first);
    }

    if (it->second == max_value)
    {
      max_list.push_back(it->first);
    }
  }
  cout << "Minimum bytes : " << min_value << endl;
  cout << "Maximum bytes : " << max_value << endl;
  cout << endl;


  list<string>::iterator it_min;
  cout << "Min list" << "(" << min_value << ")" << endl;
  for (it_min = min_list.begin(); it_min != min_list.end(); it_min++)
  {
    cout << *it_min << endl;
  }
  cout << endl;

  cout << "Max list" << "(" << max_value << ")" << endl;
  for (it_min = max_list.begin(); it_min != max_list.end(); it_min++)
  {
    cout << *it_min << endl;
  }
  cout << endl;
}

static void show_ascii_protocol_examples(void)
{
  char buf[1024];
  int len;

  len = sprintf(buf, "RHVER %u\n", UINT_MAX);
  cout << buf << ", length=" << len << endl;

  len = sprintf(buf, "RHVER %u,0,0.0.1\n", UINT_MAX);
  cout << buf << ", length=" << len << endl;

  len = sprintf(buf, "WREG %u,%u,%u\n", UINT_MAX, UINT_MAX, UINT_MAX);
  cout << buf << ", length=" << len << endl;
}