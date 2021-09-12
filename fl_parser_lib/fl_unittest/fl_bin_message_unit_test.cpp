#include "pch.h"
#include "fl_bin_message.h"
#include "fl_util.h"

#define DEVICE_ID   (1)
#define HW_VER      ("a.1.2.3")
#define FW_VER      ("a.2.3.4")

class Fl_Bin_Message_UnitTest : public testing::Test
{
protected:
  uint8_t _packet_buf[FL_BIN_MSG_MAX_LENGTH];
  uint16_t _expected_crc;
  uint16_t _actual_crc;
  uint8_t _len;
  fl_bin_msg_full_t* _msg_full;

protected:
  void SetUp()
  {
    memset(_packet_buf, 0, sizeof(_packet_buf));
    _expected_crc = 0;
    _actual_crc = 0;
    _len = 0;
    _msg_full = NULL;
  }

  uint8_t calculate_length_field_value(uint8_t msg_size)
  {
    //                                     device id field    length field 
    //      header size                    size(1)            size(2)                       crc(2)             etx(1)
    return (sizeof(fl_bin_msg_header_t) - sizeof(uint8_t) - sizeof(uint16_t)) + msg_size + sizeof(uint16_t) + sizeof(uint8_t);
  }
};

TEST_F(Fl_Bin_Message_UnitTest, TestReadHardwareVersionCommandMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_HW_VERSION;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_TRUE;
  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_READ_HW_VERSION, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_COMMAND, _msg_full->header.flag1.message_type);
  EXPECT_EQ(1, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_TRUE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(0, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestReadHardwareVersionOkResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_HW_VERSION;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  fl_hw_ver_resp_t* hw_ver = (fl_hw_ver_resp_t*)&_msg_full->payload;
  sprintf(hw_ver->version, HW_VER);

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)17, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value((uint8_t)strlen(HW_VER)), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_READ_HW_VERSION, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(1, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_OK, _msg_full->header.error);

  fl_hw_ver_resp_t* payload = (fl_hw_ver_resp_t*)&_msg_full->payload;
  EXPECT_EQ(0, strncmp(HW_VER, payload->version, strlen(HW_VER)));

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t) + strlen(HW_VER));
  _actual_crc = *((uint16_t*)&_packet_buf[14]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[16]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestReadHardwareVersionFailResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_HW_VERSION;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_READ_HW_VERSION, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(1, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_ERROR, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestReadFirmVersionCommandMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_FW_VERSION;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_TRUE;
  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_READ_FW_VERSION, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_COMMAND, _msg_full->header.flag1.message_type);
  EXPECT_EQ(2, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_TRUE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(0, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestReadFirmwareVersionOkResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_FW_VERSION;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  fl_fw_ver_resp_t* fw_ver = (fl_fw_ver_resp_t*)&_msg_full->payload;
  sprintf(fw_ver->version, FW_VER);

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)17, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value((uint8_t)strlen(FW_VER)), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_READ_FW_VERSION, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(2, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_OK, _msg_full->header.error);

  fl_fw_ver_resp_t* payload = (fl_fw_ver_resp_t*)&_msg_full->payload;
  EXPECT_EQ(0, strncmp(FW_VER, payload->version, strlen(FW_VER)));

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t) + strlen(FW_VER));
  _actual_crc = *((uint16_t*)&_packet_buf[14]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[16]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestReadFirmwareVersionFailResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_FW_VERSION;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_READ_FW_VERSION, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(2, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_ERROR, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestReadRegisterCommandMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_REGISTER;
  _msg_full->header.flag1.sequence_num = 3;
  _msg_full->header.flag1.return_expected = FL_TRUE;

  fl_read_reg_t* rreg = (fl_read_reg_t*)&_msg_full->payload;
  rreg->address = 1;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)14, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(sizeof(fl_read_reg_t)), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_READ_REGISTER, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_COMMAND, _msg_full->header.flag1.message_type);
  EXPECT_EQ(3, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_TRUE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(0, _msg_full->header.error);

  fl_read_reg_t* payload = (fl_read_reg_t*)&_msg_full->payload;
  EXPECT_EQ(1, payload->address);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t) + sizeof(fl_read_reg_t));
  _actual_crc = *((uint16_t*)&_packet_buf[11]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[13]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestReadRegisterOkResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_REGISTER;
  _msg_full->header.flag1.sequence_num = 3;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  fl_read_reg_resp_t* rreg = (fl_read_reg_resp_t*)&_msg_full->payload;
  rreg->address = 1;
  rreg->value = 1;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)18, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value((uint8_t)sizeof(fl_read_reg_resp_t)), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_READ_REGISTER, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(3, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_OK, _msg_full->header.error);

  fl_read_reg_resp_t* payload = (fl_read_reg_resp_t*)&_msg_full->payload;
  EXPECT_EQ(1, payload->address);
  EXPECT_EQ(1, payload->value);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t) + sizeof(fl_read_reg_resp_t));
  _actual_crc = *((uint16_t*)&_packet_buf[15]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[17]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestReadRegisterFailResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_READ_REGISTER;
  _msg_full->header.flag1.sequence_num = 3;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_READ_REGISTER, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(3, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_ERROR, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestWriteRegisterCommandMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_WRITE_REGISTER;
  _msg_full->header.flag1.sequence_num = 4;
  _msg_full->header.flag1.return_expected = FL_TRUE;

  fl_write_reg_t* wreg = (fl_write_reg_t*)&_msg_full->payload;
  wreg->address = 1;
  wreg->value = 1;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)18, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(sizeof(fl_write_reg_t)), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_WRITE_REGISTER, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_COMMAND, _msg_full->header.flag1.message_type);
  EXPECT_EQ(4, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_TRUE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(0, _msg_full->header.error);

  fl_write_reg_t* payload = (fl_write_reg_t*)&_msg_full->payload;
  EXPECT_EQ(1, payload->address);
  EXPECT_EQ(1, payload->value);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t) + sizeof(fl_write_reg_t));
  _actual_crc = *((uint16_t*)&_packet_buf[15]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[17]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestWriteRegisterOkResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_WRITE_REGISTER;
  _msg_full->header.flag1.sequence_num = 4;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_WRITE_REGISTER, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(4, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_OK, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestWriteRegisterFailResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_WRITE_REGISTER;
  _msg_full->header.flag1.sequence_num = 4;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_WRITE_REGISTER, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(4, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_ERROR, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestBootModeCommandMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_BOOT_MODE;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_TRUE;

  fl_boot_mode_t* boot_mode = (fl_boot_mode_t*)&_msg_full->payload;
  boot_mode->boot_mode = FL_BMODE_BOOTLOADER;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)11, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(sizeof(fl_boot_mode_t)), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_BOOT_MODE, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_COMMAND, _msg_full->header.flag1.message_type);
  EXPECT_EQ(1, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_TRUE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(0, _msg_full->header.error);

  fl_boot_mode_t* payload = (fl_boot_mode_t*)&_msg_full->payload;
  EXPECT_EQ(FL_BMODE_BOOTLOADER, payload->boot_mode);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t) + sizeof(fl_boot_mode_t));
  _actual_crc = *((uint16_t*)&_packet_buf[8]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[10]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestBootModeOkResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_BOOT_MODE;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_BOOT_MODE, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(1, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_OK, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestBootModeFailResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_BOOT_MODE;
  _msg_full->header.flag1.sequence_num = 1;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_BOOT_MODE, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(1, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_ERROR, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestResetCommandMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_RESET;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_TRUE;

  _len = fl_bin_msg_build_command(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_RESET, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_COMMAND, _msg_full->header.flag1.message_type);
  EXPECT_EQ(2, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_TRUE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(0, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestResetOkResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_RESET;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_OK;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_RESET, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(2, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_OK, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}

TEST_F(Fl_Bin_Message_UnitTest, TestResetFailResponseMessageBuild)
{
  _msg_full = (fl_bin_msg_full_t*)_packet_buf;

  _msg_full->header.device_id = DEVICE_ID;
  _msg_full->header.message_id = FL_MSG_ID_RESET;
  _msg_full->header.flag1.sequence_num = 2;
  _msg_full->header.flag1.return_expected = FL_FALSE;
  _msg_full->header.error = FL_ERROR;

  _len = fl_bin_msg_build_response(_packet_buf, sizeof(_packet_buf));

  EXPECT_EQ((uint8_t)10, _len);

  EXPECT_EQ(FL_BIN_MSG_STX, _msg_full->stx);
  EXPECT_EQ(DEVICE_ID, _msg_full->header.device_id);
  EXPECT_EQ(calculate_length_field_value(0), _msg_full->header.length);
  EXPECT_EQ(FL_MSG_ID_RESET, _msg_full->header.message_id);
  EXPECT_EQ(FL_MSG_TYPE_RESPONSE, _msg_full->header.flag1.message_type);
  EXPECT_EQ(2, _msg_full->header.flag1.sequence_num);
  EXPECT_EQ(FL_FALSE, _msg_full->header.flag1.return_expected);
  EXPECT_EQ(0, _msg_full->header.flag1.reserved);
  EXPECT_EQ(FL_ERROR, _msg_full->header.error);

  _expected_crc = fl_crc_16((const unsigned char*)&_packet_buf[1], sizeof(fl_bin_msg_header_t));
  _actual_crc = *((uint16_t*)&_packet_buf[7]);
  EXPECT_EQ(_actual_crc, _expected_crc);

  // ETX
  EXPECT_EQ((uint8_t)FL_BIN_MSG_ETX, _packet_buf[9]);
}
