#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "fl_txt_message.h"

FL_DECLARE(uint8_t) fl_txt_msg_build_command(
  const uint32_t device_id,
  const uint8_t message_id,
  void* arg_buf,
  uint32_t arg_buf_len,
  uint8_t* packet_buf,
  uint32_t packet_buf_len)
{
  uint32_t len = 0;

  if ((packet_buf == NULL) ||
      (packet_buf_len == 0))
  {
    return len;
  }

  if (arg_buf != NULL)
  {
    if (arg_buf_len == 0)
    {
      return len;
    }
    else if (arg_buf_len > sizeof(fl_hw_ver_resp_t))
    {
      return len;
    }
  }
  else
  {
    if (arg_buf != 0)
    {
      return len;
    }
  }

  switch (message_id)
  {
  case FL_MSG_ID_READ_HW_VERSION:
  {
    // RHVER device_id\n
    // ex) RHVER 1\n
    len = sprintf((char*)packet_buf, "%s %ld%c", fl_txt_msg_get_message_name(message_id), device_id, FL_TXT_MSG_TAIL);
    break;
  }

  case FL_MSG_ID_READ_FW_VERSION:
  {
    // RFVER device_id\n
    // ex) RFVER 1\n
    len = sprintf((char*)packet_buf, "%s %ld%c", fl_txt_msg_get_message_name(message_id), device_id, FL_TXT_MSG_TAIL);
    break;
  }

  case FL_MSG_ID_READ_REGISTER:
  {
    // RREG device_id,reg_addr\n
    // ex) RREG 1,2\n
    fl_read_reg_t* rreg = (fl_read_reg_t*)arg_buf;
    if ((rreg != NULL) && 
        (arg_buf_len == sizeof(fl_read_reg_t)))
    {
      len = sprintf((char*)packet_buf, "%s %ld,%ld%c", fl_txt_msg_get_message_name(message_id), device_id, rreg->address, FL_TXT_MSG_TAIL);
    }
    break;
  }

  case FL_MSG_ID_WRITE_REGISTER:
  {
    // WREG device_id,button_num,button_value\n
    // ex) WREG 1,1,1\n
    fl_write_reg_t* wreg = (fl_write_reg_t*)arg_buf;
    if ((wreg != NULL) &&
        (arg_buf_len == sizeof(fl_write_reg_t)))
    {
      len = sprintf((char*)packet_buf, "%s %ld,%ld,%ld%c", fl_txt_msg_get_message_name(message_id), device_id, wreg->address, wreg->value, FL_TXT_MSG_TAIL);
    }
    break;
  }

  case FL_MSG_ID_BOOT_MODE:
  {
    // BMODE device_id,mode\n
    // ex) BMODE 1,1\n
    fl_boot_mode_t* bmode = (fl_boot_mode_t*)arg_buf;
    if ((bmode != NULL) &&
        (arg_buf_len == sizeof(fl_boot_mode_t)))
    {
      len = sprintf((char*)packet_buf, "%s %ld,%d%c", fl_txt_msg_get_message_name(message_id), device_id, bmode->boot_mode, FL_TXT_MSG_TAIL);
    }
    break;
  }

  case FL_MSG_ID_RESET:
    // RESET device_id\n
    // ex) RESET 1\n
    len = sprintf((char*)packet_buf, "%s %ld%c", fl_txt_msg_get_message_name(message_id), device_id, FL_TXT_MSG_TAIL);
    break;
  }

  if (len > packet_buf_len)
  {
    len = 0;
  }

  return len;
}

FL_DECLARE(fl_status_t) fl_txt_msg_build_response(
  const uint32_t device_id,
  const uint8_t message_id,
  fl_status_t error,
  void* arg_buf,
  uint32_t arg_buf_len,
  uint8_t* packet_buf,
  uint32_t packet_buf_len)
{
  uint8_t len = 0;

  if ((packet_buf == NULL) ||
      (packet_buf_len == 0))
  {
    return len;
  }

  if (arg_buf != NULL)
  {
    if (arg_buf_len == 0)
    {
      return len;
    }
    else if (arg_buf_len > sizeof(fl_hw_ver_resp_t))
    {
      return len;
    }
  }
  else
  {
    if (arg_buf != 0)
    {
      return len;
    }
  }

  if (error == FL_OK)
  {
    switch (message_id)
    {
      case FL_MSG_ID_READ_HW_VERSION:
      {
        // RHVER device_id,error,version_string\n
        // ex) RHVER 1,0,1.2.3\n
        fl_hw_ver_resp_t* hw_ver = (fl_hw_ver_resp_t*)arg_buf;
        if ((arg_buf_len <= sizeof(fl_hw_ver_resp_t)) &&
            (strlen(hw_ver->version) > 0))
        {
          len = sprintf((char*)packet_buf, "%s %ld,%d,%s%c", 
            fl_txt_msg_get_message_name(message_id), 
            device_id, 
            error, 
            hw_ver->version, 
            FL_TXT_MSG_TAIL);
        }
        break;
      }

      case FL_MSG_ID_READ_FW_VERSION:
      {
        // RFVER device_id,error,version_string\n
        // ex) RFVER 1,0,2.3.4\n
        fl_fw_ver_resp_t* fw_ver = (fl_fw_ver_resp_t*)arg_buf;
        if ((arg_buf_len <= sizeof(fl_fw_ver_resp_t)) &&
          (strlen(fw_ver->version) > 0))
        {
          len = sprintf((char*)packet_buf, "%s %ld,%d,%s%c", 
            fl_txt_msg_get_message_name(message_id), 
            device_id, 
            error, 
            fw_ver->version, 
            FL_TXT_MSG_TAIL);
        }
        break;
      }

      case FL_MSG_ID_READ_REGISTER:
      {
        // RREG device_id,error,address,value\n
        // ex) RREG 1,0,2,0\n
        if (arg_buf_len == sizeof(fl_read_reg_resp_t))
        {
          fl_read_reg_resp_t* rreg = (fl_read_reg_resp_t*)arg_buf;
          len = sprintf((char*)packet_buf, "%s %ld,%d,%ld,%ld%c",
            fl_txt_msg_get_message_name(message_id), 
            device_id, 
            error, 
            rreg->address, 
            rreg->value, 
            FL_TXT_MSG_TAIL);
        }
        break;
      }

      case FL_MSG_ID_WRITE_REGISTER:
      case FL_MSG_ID_BOOT_MODE:
      case FL_MSG_ID_RESET:
      {
        // WGPIO device_id,error\n
        // ex) WGPIO 1,0\n
        len = sprintf((char*)packet_buf, "%s %ld,%d%c", fl_txt_msg_get_message_name(message_id), device_id, error, FL_TXT_MSG_TAIL);
        break;
      }
    }
  }
  else
  {
    len = sprintf((char*)packet_buf, "%s %ld,%d%c", fl_txt_msg_get_message_name(message_id), device_id, error, FL_TXT_MSG_TAIL);
  }

  if (len > packet_buf_len)
  {
    len = 0;
  }

  return len;
}

FL_DECLARE(char*) fl_txt_msg_get_message_name(const uint8_t message_id)
{
  switch (message_id)
  {
  case FL_MSG_ID_READ_HW_VERSION:
    return FL_TXT_RHVER_STR;

  case FL_MSG_ID_READ_FW_VERSION:
    return FL_TXT_RFVER_STR;

  case FL_MSG_ID_BOOT_MODE:
    return FL_TXT_BMODE_STR;

  case FL_MSG_ID_RESET:
    return FL_TXT_RESET_STR;

  case FL_MSG_ID_READ_REGISTER:
    return FL_TXT_RREG_STR;

  case FL_MSG_ID_WRITE_REGISTER:
    return FL_TXT_WREG_STR;
  }

  return NULL;
}
