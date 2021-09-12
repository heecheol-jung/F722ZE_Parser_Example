#pragma once

#ifdef __cplusplus
#define APP_LIB_BEGIN_DECLS              extern "C" {
#define APP_LIB_END_DECLS                }
#else
#define APP_LIB_BEGIN_DECLS
#define APP_LIB_END_DECLS
#endif

#if defined(APPCOMMON_EXPORTS)
#define APP_LIB_DECLARE(type)            __declspec(dllexport) type __stdcall
#define APP_LIB_DECLARE_NONSTD(type)     __declspec(dllexport) type __cdecl
#define APP_LIB_DECLARE_DATA             __declspec(dllexport)
#else
#define APP_LIB_DECLARE(type)            __declspec(dllimport) type __stdcall
#define APP_LIB_DECLARE_NONSTD(type)     __declspec(dllimport) type __cdecl
#define APP_LIB_DECLARE_DATA             __declspec(dllimport)
#endif

#define APP_LIB_STR_COMMANDS            "commands : "
#define APP_LIB_STR_PROMPT              "> "
#define APP_LIB_STR_UNKNOWN_COMMANDD    "!!! Unknown command."
#define APP_LIB_STR_QUIT                "quit"
#define APP_LIB_STR_START               "start"
#define APP_LIB_STR_STOP                "stop"
#define APP_LIB_STR_BIN_MSG             "bin msg"
#define APP_LIB_STR_TXT_MSG             "txt msg"
#define APP_LIB_STR_APP                 "app"
#define APP_LIB_STR_BOOTLOADER          "bootloader"
#define APP_LIB_STR_MESSAGE_TYPE        "message type"
#define APP_LIB_STR_FIRMWARE_TYPE       "firmware type"
#define APP_LIB_STR_COM_PORT_NAME       "com port name"

#define APP_LIB_DEBUG_PRINT(x)          do { printf x; } while (0)

// Binary message
#define APP_LIB_MSG_BIN                 (0)
// Text message
#define APP_LIB_MSG_TXT                 (1)

// Application firmware
#define APP_LIB_FW_APP                  (0)
// Bootloader firmware
#define APP_LIB_FW_BOOTLOADER           (1)

#define APP_LIB_MAX_BUF_LEN             (2048)
#define APP_LIB_COM_PORT_NAME_BUF_LEN   (64)

#define APP_LIB_PARSE_RESULT_NONE           (0)
#define APP_LIB_PARSE_REUSLT_WITH_CALLBACK  (1)
#define APP_LIB_PARSE_RESULT_WITH_OUT_PARAM (2)

typedef void(*app_func_ptr_t)(void);
