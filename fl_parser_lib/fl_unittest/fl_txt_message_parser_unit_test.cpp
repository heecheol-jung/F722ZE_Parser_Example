#include "pch.h"
#include "fl_txt_message.h"
#include "fl_txt_message_parser.h"
#include "fl_util.h"

#define DEVICE_ID   (1)
#define HW_VER      ("a.1.2.3")
#define FW_VER      ("a.2.3.4")

class Fl_Txt_Message_Parser_UnitTest : public testing::Test
{
protected:
  fl_txt_msg_parser_t  _txt_parser;
  fl_txt_msg_t         _parsed_msg;
  int                   _len;
  int                   _i;
  uint8_t               _packet_buf[FL_TXT_MSG_MAX_LENGTH];
  fl_status_t          _ret;

protected:
  void SetUp()
  {
    fl_txt_msg_parser_init(&_txt_parser);
    memset(&_parsed_msg, 0, sizeof(fl_txt_msg_t));
  }
};

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestReadHardwareVersionCommandParse)
{
  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_READ_HW_VERSION, NULL, 0, _packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_command(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_HW_VERSION, _parsed_msg.msg_id);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestReadHardwareVersionOkResponseParse)
{
  // Version string.
  fl_hw_ver_resp_t hw_ver;
  sprintf(hw_ver.version, HW_VER);
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_HW_VERSION, FL_OK,
    &hw_ver, sizeof(hw_ver),
    _packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_HW_VERSION, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.error);

  fl_hw_ver_resp_t* parsed_hw_ver = (fl_hw_ver_resp_t*)&_parsed_msg.payload;
  EXPECT_EQ((uint8_t)0, (uint8_t)strcmp(hw_ver.version, parsed_hw_ver->version));
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestReadHardwareVersionFailResponseParse)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_HW_VERSION, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_HW_VERSION, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.error);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestReadFirmwareVersionCommandParse)
{
  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_READ_FW_VERSION, NULL, 0, _packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_command(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_FW_VERSION, _parsed_msg.msg_id);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestReadFirmwareVersionOkResponseParse)
{
  // Version string.
  fl_fw_ver_resp_t fw_ver;
  sprintf(fw_ver.version, FW_VER);
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_FW_VERSION, FL_OK,
    &fw_ver, sizeof(fw_ver),
    _packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_FW_VERSION, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.error);

  fl_fw_ver_resp_t* parsed_fw_ver = (fl_fw_ver_resp_t*)&_parsed_msg.payload;
  EXPECT_EQ((uint8_t)0, (uint8_t)strcmp(fw_ver.version, parsed_fw_ver->version));
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestReadFirmwareVersionFailResponseParse)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_FW_VERSION, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_FW_VERSION, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.error);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestReadRegisterCommandParse)
{
  // Register address.
  fl_read_reg_t rreg;
  rreg.address = 2;

  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_READ_REGISTER,
    &rreg, sizeof(fl_read_reg_t),
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_command(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_REGISTER, _parsed_msg.msg_id);

  fl_read_reg_t* payload = (fl_read_reg_t*)&_parsed_msg.payload;
  EXPECT_EQ(2, payload->address);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestReadRegisterOkResponseParse)
{
  fl_read_reg_resp_t rreg;

  // Register address.
  rreg.address = 2;
  // Register value.
  rreg.value = 0;

  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_REGISTER, FL_OK,
    &rreg, sizeof(fl_read_reg_resp_t),
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_REGISTER, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.error);

  fl_read_reg_resp_t* payload = (fl_read_reg_resp_t*)&_parsed_msg.payload;
  EXPECT_EQ(2, payload->address);
  EXPECT_EQ(0, payload->value);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestReadGpioFailResponseParse)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_REGISTER, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_READ_REGISTER, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.error);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestWriteGpioCommandParse)
{
  fl_write_reg_t wreg;

  // Register address.
  wreg.address = 3;
  // Register value.
  wreg.value = 1;

  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_WRITE_REGISTER,
    &wreg, sizeof(fl_write_reg_t),
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_command(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_WRITE_REGISTER, _parsed_msg.msg_id);

  fl_write_reg_t* payload = (fl_write_reg_t*)&_parsed_msg.payload;
  EXPECT_EQ(3, payload->address);
  EXPECT_EQ(1, payload->value);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestWriteGpioOkResponseParse)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_WRITE_REGISTER, FL_OK,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_WRITE_REGISTER, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.error);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestWriteGpioFailResponseParse)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_WRITE_REGISTER, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_WRITE_REGISTER, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.error);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestBootModeCommandParse)
{
  fl_boot_mode_t bmode;

  // Boot mode.
  bmode.boot_mode = FL_BMODE_BOOTLOADER;
  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_BOOT_MODE,
    &bmode, sizeof(fl_boot_mode_t),
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_command(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_BOOT_MODE, _parsed_msg.msg_id);

  fl_boot_mode_t* payload = (fl_boot_mode_t*)&_parsed_msg.payload;
  EXPECT_EQ((uint8_t)FL_BMODE_BOOTLOADER, payload->boot_mode);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestBootModeOkResponseParse)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_BOOT_MODE, FL_OK,
    NULL, 0,
    _packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_BOOT_MODE, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.error);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestBootModeFailResponseParse)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_BOOT_MODE, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_BOOT_MODE, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.error);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestResetCommandParse)
{
  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_RESET,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_command(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_RESET, _parsed_msg.msg_id);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestResetOkResponseParse)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_RESET, FL_OK,
    NULL, 0,
    _packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_RESET, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_OK, _parsed_msg.error);
}

TEST_F(Fl_Txt_Message_Parser_UnitTest, TestResetFailResponseParse)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_RESET, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  for (_i = 0; _i < _len; _i++)
  {
    _ret = fl_txt_msg_parser_parse_response_event(&_txt_parser, _packet_buf[_i], &_parsed_msg);
  }

  EXPECT_EQ((fl_status_t)FL_OK, _ret);
  EXPECT_EQ((uint32_t)DEVICE_ID, _parsed_msg.device_id);
  EXPECT_EQ((uint8_t)FL_MSG_ID_RESET, _parsed_msg.msg_id);
  EXPECT_EQ((uint8_t)FL_ERROR, _parsed_msg.error);
}
