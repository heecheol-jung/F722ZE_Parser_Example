#include "pch.h"
#include "fl_bin_message_parser.h"
#include "fl_util.h"

#define HW_VER      ("1.2.3")
#define FW_VER      ("2.3.4")

class Fl_Bin_Message_Parser_UnitTest : public testing::Test
{
protected:
  fl_bin_msg_parser_t       _bin_parser;
  fl_bin_msg_full_t         _parsed_msg;
  int                       _len;
  int                       _i;
  uint8_t                   _packet_buf[FL_BIN_MSG_MAX_LENGTH];
  fl_status_t               _ret;
  uint16_t                  _expected_crc;
  uint16_t                  _actual_crc;
  fl_bin_msg_full_t*        _msg_full;

protected:
  void SetUp()
  {
    fl_bin_msg_parser_init(&_bin_parser);
    memset(&_parsed_msg, 0, sizeof(fl_bin_msg_full_t));
    memset(&_packet_buf, 0, sizeof(fl_bin_msg_full_t));
    _expected_crc = 0;
    _actual_crc = 0;
    _msg_full = NULL;
  }

  uint8_t calculate_length_field_value(uint8_t msg_size)
  {
    //                                     device id field    length field 
    //      header size                    size(1)            size(2)                       crc(2)             etx(1)
    return (sizeof(fl_bin_msg_header_t) - sizeof(uint8_t) - sizeof(uint16_t)) + msg_size + sizeof(uint16_t) + sizeof(uint8_t);
  }
};

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestReadHardwareVersionCommandParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 1;
  _msg_full->header.message_id = FL_MSG_ID_READ_HW_VERSION;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_TRUE;
  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)1, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_HW_VERSION, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_COMMAND, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_TRUE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)1, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestReadHardwareVersionOkResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 1;
  _msg_full->header.message_id = FL_MSG_ID_READ_HW_VERSION;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  fl_hw_ver_resp_t* hw_ver = (fl_hw_ver_resp_t*)&_msg_full->payload;
  sprintf(hw_ver->version, HW_VER);

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)1, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value((uint8_t)strlen("1.2.3")), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_HW_VERSION, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)1, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);

  fl_hw_ver_resp_t* payload = (fl_hw_ver_resp_t*)&_parsed_msg.payload;
  EXPECT_EQ(0, strncmp(hw_ver->version, payload->version, _msg_full->header.length - 6));
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestReadHardwareVersionFailResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 1;
  _msg_full->header.message_id = FL_MSG_ID_READ_HW_VERSION;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)1, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_HW_VERSION, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)1, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestReadFirmwareVersionCommandParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 2;
  _msg_full->header.message_id = FL_MSG_ID_READ_FW_VERSION;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_TRUE;
  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)2, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_FW_VERSION, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_COMMAND, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_TRUE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)2, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestReadFirmwareVersionOkResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 2;
  _msg_full->header.message_id = FL_MSG_ID_READ_FW_VERSION;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  fl_fw_ver_resp_t* fw_ver = (fl_fw_ver_resp_t*)&_msg_full->payload;
  sprintf(fw_ver->version, FW_VER);

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)2, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value((uint8_t)strlen("2.3.4")), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_FW_VERSION, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)2, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);

  fl_fw_ver_resp_t* payload = (fl_fw_ver_resp_t*)&_parsed_msg.payload;
  EXPECT_EQ(0, strncmp(fw_ver->version, payload->version, _msg_full->header.length - 6));
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestReadFirmwareVersionFailResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 2;
  _msg_full->header.message_id = FL_MSG_ID_READ_FW_VERSION;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)2, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_FW_VERSION, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)2, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestReadRegisterCommandParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 3;
  _msg_full->header.message_id = FL_MSG_ID_READ_REGISTER;
  _msg_full->header.flag1.sequence_num = 3;
  _msg_full->header.flag1.return_expected = FL_TRUE;

  fl_read_reg_t* rreg = (fl_read_reg_t*)&_msg_full->payload;
  rreg->address = 1;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)3, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(sizeof(fl_read_reg_t)), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_REGISTER, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_COMMAND, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_TRUE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)3, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);

  fl_read_reg_t* payload = (fl_read_reg_t*)&_parsed_msg.payload;
  EXPECT_EQ(rreg->address, payload->address);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestReadGpioOkResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 3;
  _msg_full->header.message_id = FL_MSG_ID_READ_REGISTER;
  _msg_full->header.flag1.sequence_num = 3;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  fl_read_reg_resp_t* rreg = (fl_read_reg_resp_t*)&_msg_full->payload;
  rreg->address = 1;
  rreg->value = 1;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)3, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(sizeof(fl_read_reg_resp_t)), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_REGISTER, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)3, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);

  fl_read_reg_resp_t* payload = (fl_read_reg_resp_t*)&_parsed_msg.payload;
  EXPECT_EQ(rreg->address, payload->address);
  EXPECT_EQ(rreg->value, payload->value);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestReadGpioFailResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 3;
  _msg_full->header.message_id = FL_MSG_ID_READ_REGISTER;
  _msg_full->header.flag1.sequence_num = 3;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)3, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_REGISTER, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)3, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestWriteRegisterCommandParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 4;
  _msg_full->header.message_id = FL_MSG_ID_WRITE_REGISTER;
  _msg_full->header.flag1.sequence_num = 4;
  _msg_full->header.flag1.return_expected = FL_TRUE;

  fl_write_reg_t* wreg = (fl_write_reg_t*)&_msg_full->payload;
  wreg->address = 1;
  wreg->value = 1;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)4, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(sizeof(fl_write_reg_t)), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_WRITE_REGISTER, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_COMMAND, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_TRUE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)4, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);

  fl_write_reg_t* payload = (fl_write_reg_t*)&_parsed_msg.payload;
  EXPECT_EQ(wreg->address, payload->address);
  EXPECT_EQ(wreg->value, payload->value);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestWriteRegisterOkResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 4;
  _msg_full->header.message_id = FL_MSG_ID_WRITE_REGISTER;
  _msg_full->header.flag1.sequence_num = 4;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)4, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_WRITE_REGISTER, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)4, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestWriteRegisterFailResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 4;
  _msg_full->header.message_id = FL_MSG_ID_WRITE_REGISTER;
  _msg_full->header.flag1.sequence_num = 4;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)4, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_WRITE_REGISTER, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)4, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestBootModeCommandParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 8;
  _msg_full->header.message_id = FL_MSG_ID_BOOT_MODE;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_TRUE;

  fl_boot_mode_t* boot_mode = (fl_boot_mode_t*)&_msg_full->payload;
  boot_mode->boot_mode = FL_BMODE_BOOTLOADER;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)8, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(sizeof(fl_boot_mode_t)), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_BOOT_MODE, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_COMMAND, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_TRUE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)1, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestBootModeOkResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 8;
  _msg_full->header.message_id = FL_MSG_ID_BOOT_MODE;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)8, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_BOOT_MODE, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)1, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestBootModeFailResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 8;
  _msg_full->header.message_id = FL_MSG_ID_BOOT_MODE;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)8, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_BOOT_MODE, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)1, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestResetCommandParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 9;
  _msg_full->header.message_id = FL_MSG_ID_RESET;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_TRUE;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)9, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_RESET, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_COMMAND, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_TRUE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)2, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestResetOkResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 9;
  _msg_full->header.message_id = FL_MSG_ID_RESET;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)9, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_RESET, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)2, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.header.error);
}

TEST_F(Fl_Bin_Message_Parser_UnitTest, TestResetFailResponseParse)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = 9;
  _msg_full->header.message_id = FL_MSG_ID_RESET;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint8_t)FL_BIN_MSG_STX, _parsed_msg.stx);
  EXPECT_EQ((uint32_t)9, _parsed_msg.header.device_id);
  EXPECT_EQ((uint8_t)calculate_length_field_value(0), _parsed_msg.header.length);
  EXPECT_EQ((uint8_t)FL_MSG_ID_RESET, _parsed_msg.header.message_id);

  // Flag1
  EXPECT_EQ((uint8_t)FL_MSG_TYPE_RESPONSE, _parsed_msg.header.flag1.message_type);
  EXPECT_EQ((uint8_t)FL_FALSE, _parsed_msg.header.flag1.return_expected);
  EXPECT_EQ((uint8_t)2, _parsed_msg.header.flag1.sequence_num);
  EXPECT_EQ((uint8_t)0, _parsed_msg.header.flag1.reserved);

  // Flag2
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.header.error);
}
