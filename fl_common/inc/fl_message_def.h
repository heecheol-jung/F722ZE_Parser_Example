// Firware library message
// fl_message_def.h

#ifndef FL_MESSAGE_DEF_H
#define FL_MESSAGE_DEF_H

#include "fl_def.h"

FL_BEGIN_DECLS

///////////////////////////////////////////////////////////////////////////////
// Message ID's
///////////////////////////////////////////////////////////////////////////////
// Message IDs
#define FL_MSG_ID_BASE                      (0)
#define FL_MSG_ID_UNKNOWN                   (FL_MSG_ID_BASE + 0)

// Read hardware version.
#define FL_MSG_ID_READ_HW_VERSION           (FL_MSG_ID_BASE + 1)

// Read firmware version.
#define FL_MSG_ID_READ_FW_VERSION           (FL_MSG_ID_BASE + 2)

// Set boot mode. 
#define FL_MSG_ID_BOOT_MODE                 (FL_MSG_ID_BASE + 3)

// Reset a target device.
#define FL_MSG_ID_RESET                     (FL_MSG_ID_BASE + 4)

// Read a value from a register.
#define FL_MSG_ID_READ_REGISTER             (FL_MSG_ID_BASE + 5)

// Write a value to a register.
#define FL_MSG_ID_WRITE_REGISTER            (FL_MSG_ID_BASE + 6)

///////////////////////////////////////////////////////////////////////////////
// Defines for general messages.
///////////////////////////////////////////////////////////////////////////////
// Maximum length of argument string.
#define FL_MSG_MAX_STRING_LEN               (32)

// Device IDs
#define FL_DEVICE_ID_UNKNOWN                (0)

#define FL_DEVICE_ID_ALL                    (0xFFFF)  // Device broadcasting.

#define FL_BUTTON_RELEASED                  (0)

#define FL_BUTTON_PRESSED                   (1)

// Message type.
#define FL_MSG_TYPE_COMMAND                 (0)
#define FL_MSG_TYPE_RESPONSE                (1)
#define FL_MSG_TYPE_EVENT                   (2)
#define FL_MSG_TYPE_UNKNOWN                 (0XFF)

// Boot mode : Application
#define FL_BMODE_APP                        (0)
// Boot mode : Bootloader
#define FL_BMODE_BOOTLOADER                 (1)

#define FL_VER_STR_MAX_LEN                  (32)

#define FL_TXT_MSG_ID_MIN_CHAR              ('A')
#define FL_TXT_MSG_ID_MAX_CHAR              ('Z')
#define FL_TXT_DEVICE_ID_MIN_CHAR           ('0')
#define FL_TXT_DEVICE_ID_MAX_CHAR           ('9')
// The last character for a text message.
#define FL_TXT_MSG_TAIL                     ('\n')
// Delimiter for a message id and device id
#define FL_TXT_MSG_ID_DEVICE_ID_DELIMITER   (' ')
// Delimiter for arguments.
#define FL_TXT_MSG_ARG_DELIMITER            (',')

FL_BEGIN_PACK1

///////////////////////////////////////////////////////////////////////////////
// structs for messages.
///////////////////////////////////////////////////////////////////////////////
typedef struct _fl_hw_ver_resp
{
  char        version[FL_VER_STR_MAX_LEN];
} fl_hw_ver_resp_t;

typedef struct _fl_fw_ver_resp
{
  char        version[FL_VER_STR_MAX_LEN];
} fl_fw_ver_resp_t;

typedef struct _fl_boot_mode
{
  uint8_t     boot_mode;  // FL_BMODE_APP, FL_BMODE_BOOTLOADER
} fl_boot_mode_t;

typedef struct _fl_read_reg
{
  uint32_t  address;  // register address
} fl_read_reg_t;

typedef struct _fl_read_reg_resp
{
  uint32_t  address;  // register address
  uint32_t  value;    // register value
} fl_read_reg_resp_t;

typedef struct _fl_write_reg
{
  uint32_t  address;  // register address
  uint32_t  value;    // register value
} fl_write_reg_t;

FL_END_PACK

typedef void(*fl_msg_cb_on_parsed_t)(const void* parser_handle, void* context);

FL_END_DECLS

#endif

