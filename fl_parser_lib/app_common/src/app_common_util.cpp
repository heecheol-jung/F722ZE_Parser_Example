#include <iostream>
#include "app_common_util.h"

APP_LIB_DECLARE(void) print_menu(std::map<std::string, app_func_ptr_t>* map_ptr)
{
  if (map_ptr != NULL && map_ptr->size() > 0)
  {
    std::cout << APP_LIB_STR_COMMANDS << std::endl;
    for (std::map<std::string, app_func_ptr_t>::iterator it = map_ptr->begin(); it != map_ptr->end(); it++)
    {
      std::cout << it->first.c_str() << std::endl;
    }
    std::cout << APP_LIB_STR_PROMPT;
  }
}
