// fl_console_app1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <map>
#include "app_common_util.h"
#include "app_constant.h"
#include "general_example.h"
#include "host_simulator_example.h"

int main()
{
  bool loop = true;
  std::map<std::string, app_func_ptr_t> map_menus;

  map_menus.insert(std::make_pair(APP_LIB_STR_QUIT, (app_func_ptr_t)NULL));
  map_menus[STR_HOST_SIMULATOR] = host_simulator_example;
  map_menus[STR_GENERAL_EXAMPLE] = general_example;

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

  std::cout << "main done." << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
