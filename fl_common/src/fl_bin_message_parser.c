#include <string.h>
#include <stdlib.h>
#include "fl_bin_message_parser.h"
#include "fl_util.h"

static fl_bool_t check_header_payload(fl_bin_msg_parser_t* parser_handle);

FL_DECLARE(void) fl_bin_msg_parser_init(fl_bin_msg_parser_t* parser_handle)
{
  memset(parser_handle, 0, sizeof(fl_bin_msg_parser_t));
}

FL_DECLARE(void) fl_bin_msg_parser_clear(fl_bin_msg_parser_t* parser_handle)
{
  memset(&parser_handle->buf, 0, sizeof(parser_handle->buf));
  parser_handle->buf_pos = 0;
  parser_handle->receive_state = FL_BIN_MSG_PARSER_RCV_STS_STX;
  parser_handle->count = 0;
}

FL_DECLARE(fl_status_t) fl_bin_msg_parser_parse(fl_bin_msg_parser_t* parser_handle, uint8_t data, fl_bin_msg_full_t* message)
{
  fl_status_t ret = FL_BIN_MSG_PARSER_PARSING;

  switch (parser_handle->receive_state)
  {
  case FL_BIN_MSG_PARSER_RCV_STS_STX:
    if (data == FL_BIN_MSG_STX)
    {
      parser_handle->buf[parser_handle->buf_pos++] = data;
      parser_handle->receive_state = FL_BIN_MSG_PARSER_RCV_STS_DEVICE_ID;
    }
    else
    {
      ret = FL_ERROR;
    }
    break;

  case FL_BIN_MSG_PARSER_RCV_STS_DEVICE_ID:
    if (parser_handle->count < FL_BIN_MSG_DEVICE_ID_LENGTH)
    {
      parser_handle->buf[parser_handle->buf_pos++] = data;
      parser_handle->count++;
      if (parser_handle->count == FL_BIN_MSG_DEVICE_ID_LENGTH)
      {
        parser_handle->count = 0;
        parser_handle->receive_state = FL_BIN_MSG_PARSER_RCV_STS_LENGTH;
      }
    }
    else
    {
      ret = FL_ERROR;
    }
    break;

  case FL_BIN_MSG_PARSER_RCV_STS_LENGTH:
    if (parser_handle->count < FL_BIN_MSG_LENGTH_FIELD_LENGTH)
    {
      parser_handle->buf[parser_handle->buf_pos++] = data;
      parser_handle->count++;
      if (parser_handle->count == FL_BIN_MSG_LENGTH_FIELD_LENGTH)
      {
        parser_handle->count = 0;
        parser_handle->receive_state = FL_BIN_MSG_PARSER_RCV_STS_HDR_DATA;
      }
    }
    else
    {
      ret = FL_ERROR;
    }
    // TODO : Check length value.
    break;

  case FL_BIN_MSG_PARSER_RCV_STS_HDR_DATA:
  {
    fl_bin_msg_header_t* header = (fl_bin_msg_header_t*)&parser_handle->buf[1];
    if (parser_handle->count < header->length)
    {
      parser_handle->buf[parser_handle->buf_pos++] = data;
      parser_handle->count++;
      if (parser_handle->count == header->length)
      {
        if (check_header_payload(parser_handle) == FL_TRUE)
        {
          ret = FL_OK;
        }
        else
        {
          ret = FL_ERROR;
        }
      }
    }
  }
  break;

  default:
    ret = FL_ERROR;
    break;
  }

  if (ret != FL_BIN_MSG_PARSER_PARSING)
  {
    if (ret == FL_OK)
    {
      if (parser_handle->on_parsed_callback != NULL)
      {
        parser_handle->on_parsed_callback((const void*)parser_handle, parser_handle->context);
        fl_bin_msg_parser_clear(parser_handle);
      }
      else
      {
        if (message != NULL)
        {
          memcpy(message, parser_handle->buf, parser_handle->buf_pos);
          fl_bin_msg_parser_clear(parser_handle);
        }
      }
    }
  }

  return ret;
}

static fl_bool_t check_header_payload(fl_bin_msg_parser_t* parser_handle)
{
  fl_bin_msg_header_t* header = (fl_bin_msg_header_t*)&parser_handle->buf[1];
  uint8_t msg_size = 0;
  uint16_t calculated_crc = 0;
  uint16_t actual_crc = 0;

  if (parser_handle->buf_pos < FL_BIN_MSG_MIN_LENGTH)
  {
    return FL_FALSE;
  }

  if (parser_handle->buf[parser_handle->buf_pos - 1] != FL_BIN_MSG_ETX)
  {
    return FL_FALSE;
  }

  msg_size = sizeof(fl_bin_msg_header_t);
  if (header->flag1.message_type == FL_MSG_TYPE_COMMAND)
  {
    if (header->message_id == FL_MSG_ID_READ_REGISTER)
    {
      msg_size += sizeof(fl_read_reg_t);
    }
    else if (header->message_id == FL_MSG_ID_WRITE_REGISTER)
    {
      msg_size += sizeof(fl_write_reg_t);
    }
    else if (header->message_id == FL_MSG_ID_BOOT_MODE)
    {
      msg_size += sizeof(fl_boot_mode_t);
    }
  }
  else if (header->flag1.message_type == FL_MSG_TYPE_RESPONSE)
  {
    if (header->error == FL_OK)
    {
      if (header->message_id == FL_MSG_ID_READ_HW_VERSION)
      {
        //                                                  device id field    length field 
        //                   header size(6)                 size(1)            size(2)                       crc(2)             etx(1)
        // header->length =  (sizeof(fl_bin_msg_header_t) - sizeof(uint8_t) - sizeof(uint16_t)) + msg_size + sizeof(uint16_t) + sizeof(uint8_t);
        // msg_size = header->length - 6;
        msg_size += (header->length - 6);
      }
      else if (header->message_id == FL_MSG_ID_READ_FW_VERSION)
      {
        msg_size += (header->length - 6);
      }
      else if (header->message_id == FL_MSG_ID_READ_REGISTER)
      {
        msg_size += sizeof(fl_read_reg_resp_t);
      }
    }
  }
  else
  {
    return FL_FALSE;
  }

  calculated_crc = fl_crc_16(&parser_handle->buf[1], msg_size);
  actual_crc = *((uint16_t*)&parser_handle->buf[parser_handle->buf_pos - 3]);
  if (calculated_crc != actual_crc)
  {
    return FL_FALSE;
  }

  return FL_TRUE;
}
