#pragma once
#include <string>
#include <map>
#include "app_common.h"

APP_LIB_DECLARE(void) print_menu(std::map<std::string, app_func_ptr_t>* map_ptr);
