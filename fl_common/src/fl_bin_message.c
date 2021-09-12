#include <string.h>
#include "fl_bin_message.h"
#include "fl_util.h"

static void build_header_crc(uint8_t* packet_buf, uint16_t msg_size, uint8_t msg_type);
static fl_bool_t check_bin_msg_args(uint8_t* packet_buf, uint16_t packet_buf_len);

FL_DECLARE(uint8_t) fl_bin_msg_build_command(
  uint8_t* packet_buf,
  uint16_t packet_buf_len)
{
  uint16_t msg_size = sizeof(fl_bin_msg_header_t);

  if (check_bin_msg_args(packet_buf, packet_buf_len) != FL_TRUE)
  {
    return 0;
  }

  switch (((fl_bin_msg_full_t*)packet_buf)->header.message_id)
  {
    case FL_MSG_ID_READ_REGISTER:
      msg_size += sizeof(fl_read_reg_t);
      break;

    case FL_MSG_ID_WRITE_REGISTER:
      msg_size += sizeof(fl_write_reg_t);
      break;

    case FL_MSG_ID_BOOT_MODE:
      msg_size += sizeof(fl_boot_mode_t);
      break;
  }

  build_header_crc(packet_buf, msg_size, FL_MSG_TYPE_COMMAND);

  // Packet length = stx(1 byte) + message size + crc size(2 byte) + etx size(1 byte).
  return (msg_size + 4);
}

FL_DECLARE(uint8_t) fl_bin_msg_build_response(
  uint8_t* packet_buf,
  uint16_t packet_buf_len)
{
  uint8_t msg_size = sizeof(fl_bin_msg_header_t);

  if (check_bin_msg_args(packet_buf, packet_buf_len) != FL_TRUE)
  {
    return 0;
  }

  if (((fl_bin_msg_full_t*)packet_buf)->header.error == FL_OK)
  {
    switch (((fl_bin_msg_full_t*)packet_buf)->header.message_id)
    {
      case FL_MSG_ID_READ_HW_VERSION:
      {
        fl_hw_ver_resp_t* hw_ver = (fl_hw_ver_resp_t*)&(((fl_bin_msg_full_t*)packet_buf)->payload);
        uint32_t len = strlen(hw_ver->version);
        if (len > sizeof(fl_hw_ver_resp_t))
        {
          return 0;
        }
        msg_size += (uint8_t)len;
        break;
      }

      case FL_MSG_ID_READ_FW_VERSION:
      {
        fl_fw_ver_resp_t* fw_ver = (fl_fw_ver_resp_t*)&(((fl_bin_msg_full_t*)packet_buf)->payload);
        uint32_t len = strlen(fw_ver->version);
        if (len > sizeof(fl_fw_ver_resp_t))
        {
          return 0;
        }
        msg_size += (uint8_t)len;
        break;
      }

      case FL_MSG_ID_READ_REGISTER:
      {
        msg_size += sizeof(fl_read_reg_resp_t);
        break;
      }
    }
  }

  build_header_crc(packet_buf, msg_size, FL_MSG_TYPE_RESPONSE);

  // Packet length = stx(1 byte) + message size + crc size(2 byte) + etx size(1 byte).
  return (msg_size + 4);
}

static void build_header_crc(uint8_t* packet_buf, uint16_t msg_size, uint8_t msg_type)
{
  fl_bin_msg_full_t* msg_full = (fl_bin_msg_full_t*)packet_buf;

  msg_full->stx = FL_BIN_MSG_STX;

  // length field value = Message size - device ID field(1) - Length field(2) + CRC(2) + ETX(1)
  msg_full->header.length = msg_size - (sizeof(uint8_t) + sizeof(uint16_t)) + 3;

  msg_full->header.flag1.message_type = msg_type;

  *((uint16_t*)(&packet_buf[1] + msg_size)) = fl_crc_16(&packet_buf[1], msg_size);
  (&packet_buf[1] + msg_size)[2] = FL_BIN_MSG_ETX;
}

static fl_bool_t check_bin_msg_args(uint8_t* packet_buf, uint16_t packet_buf_len)
{
  if (packet_buf == NULL)
  {
    return FL_FALSE;
  }
  else if ((packet_buf_len == 0) ||
           (packet_buf_len < (sizeof(fl_bin_msg_full_t) - sizeof(fl_hw_ver_resp_t)))) // The shortest message : header only message.
  {
    return FL_FALSE;
  }

  return FL_TRUE;
}
