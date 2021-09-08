#include "pch.h"
#include <string.h>
#include "fl_txt_message.h"
#include "fl_util.h"

#define DEVICE_ID   (1)
#define HW_VER      ("a.1.2.3")
#define FW_VER      ("a.2.3.4")

class Fl_Txt_Message_UnitTest : public testing::Test
{
protected:
  uint8_t   _packet_buf[FL_TXT_MSG_MAX_LENGTH];
  uint8_t   _len;

protected:
  void SetUp()
  {
    memset(_packet_buf, 0, sizeof(_packet_buf));
  }
};

TEST_F(Fl_Txt_Message_UnitTest, TestReadHardwareVersionCommandMessageBuild) 
{
  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_READ_HW_VERSION, NULL, 0, _packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)8, _len);
  EXPECT_EQ(0, strcmp("RHVER 1\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestReadHardwareVersionOkResponseMessageBuild)
{
  fl_hw_ver_t hw_ver;

  sprintf(hw_ver.version, HW_VER);
  
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_HW_VERSION, FL_OK,
    &hw_ver, sizeof(fl_hw_ver_t),
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)18, _len);
  EXPECT_EQ(0, strcmp("RHVER 1,0,a.1.2.3\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestReadHardwareVersionFailResponseMessageBuild)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_HW_VERSION, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);
  EXPECT_EQ(0, strcmp("RHVER 1,1\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestReadFirmwareVersionCommandMessageBuild)
{
  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_READ_FW_VERSION, NULL, 0, _packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)8, _len);
  EXPECT_EQ(0, strcmp("RFVER 1\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestReadFirmwareVersionOkResponseMessageBuild)
{
  fl_fw_ver_t fw_ver;

  sprintf(fw_ver.version, FW_VER);

  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_FW_VERSION, FL_OK,
    &fw_ver, sizeof(fl_fw_ver_t),
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)18, _len);
  EXPECT_EQ(0, strcmp("RFVER 1,0,a.2.3.4\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestReadFirmwareVersionFailResponseMessageBuild)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_FW_VERSION, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);
  EXPECT_EQ(0, strcmp("RFVER 1,1\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestReadRegisterCommandMessageBuild)
{
  // Register address.
  fl_read_reg_t rreg;
  rreg.address= 2;
  
  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_READ_REGISTER,
    &rreg, sizeof(fl_read_reg_t),
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)9, _len);
  EXPECT_EQ(0, strcmp("RREG 1,2\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestReadGpioOkResponseMessageBuild)
{
  fl_read_reg_resp_t rreg;

  // Register address.
  rreg.address = 2;
  // Register value.
  rreg.value = 0;

  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_REGISTER, FL_OK,
    &rreg, sizeof(fl_read_reg_resp_t),
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)13, _len);
  EXPECT_EQ(0, strcmp("RREG 1,0,2,0\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestReadGpioFailResponseMessageBuild)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_READ_REGISTER, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)9, _len);
  EXPECT_EQ(0, strcmp("RREG 1,1\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestWriteGpioCommandMessageBuild)
{
  fl_write_reg_t wreg;

  // Register address.
  wreg.address = 3;
  // Register value.
  wreg.value = 1;

  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_WRITE_REGISTER,
    &wreg, sizeof(fl_write_reg_t),
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)11, _len);
  EXPECT_EQ(0, strcmp("WREG 1,3,1\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestWriteGpioOkResponseMessageBuild)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_WRITE_REGISTER, FL_OK,
    NULL, 0,
    _packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)9, _len);
  EXPECT_EQ(0, strcmp("WREG 1,0\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestWriteGpioFailResponseMessageBuild)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_WRITE_REGISTER, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)9, _len);
  EXPECT_EQ(0, strcmp("WREG 1,1\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestBootModeCommandMessageBuild)
{
  fl_boot_mode_t bmode;

  // Boot mode.
  bmode.boot_mode = FL_BMODE_BOOTLOADER;
  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_BOOT_MODE,
    &bmode, sizeof(fl_boot_mode_t),
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);
  EXPECT_EQ(0, strcmp("BMODE 1,1\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestBootModeOkResponseMessageBuild)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_BOOT_MODE, FL_OK,
    NULL, 0,
    _packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);
  EXPECT_EQ(0, strcmp("BMODE 1,0\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestBootModeFailResponseMessageBuild)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_BOOT_MODE, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);
  EXPECT_EQ(0, strcmp("BMODE 1,1\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestResetCommandMessageBuild)
{
  _len = fl_txt_msg_build_command(DEVICE_ID, FL_MSG_ID_RESET,
    NULL, 0,
    _packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)8, _len);
  EXPECT_EQ(0, strcmp("RESET 1\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestResetOkResponseMessageBuild)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_RESET, FL_OK,
    NULL, 0,
    _packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);
  EXPECT_EQ(0, strcmp("RESET 1,0\n", (const char*)_packet_buf));
}

TEST_F(Fl_Txt_Message_UnitTest, TestResetFailResponseMessageBuild)
{
  _len = fl_txt_msg_build_response(DEVICE_ID, FL_MSG_ID_RESET, FL_ERROR,
    NULL, 0,
    (uint8_t*)_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);
  EXPECT_EQ(0, strcmp("RESET 1,1\n", (const char*)_packet_buf));
}
