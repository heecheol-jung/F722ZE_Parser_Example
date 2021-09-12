# F722ZE_Parser_Example
Text and binary message parser example for NUCLEO F722ZE
- USART, USB CDC interface
- Base proejct templates for text and binary message protocol

**1. Tools**
- Board : NUCLEO F722ZE
- STM32CubeIDE : 1.6.1
- STM32CubeMX : 6.2.1
- Visual Studio 2019

**2. Directories**
- fl_parser_lib : Visual Studio 2019 proejct, parser unit test
- fl_common : Common source files for firmware projects
- F722ZE_Parser_Poll : USART polling firmware project
- F722ZE_Parser_INT_RX : USART RX interrupt firmware project
- F722ZE_Parser_INT_RXTX : USART RX/TX interrupt firmware project
- F722ZE_Parser_DMA_RX : USART RX DMA firmware project
- F722ZE_Parser_DMA_RXTX : USART RX/TX DMA firmware project
- F722ZE_Parser_CDC : USB CDC firmware project
- Refer to [F722ZE USART Example](https://github.com/heecheol-jung/F722ZE_USART_Example) for USART only projects

**3. Protocol format**

3.1 Text protocol
![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/command_format.png)

Example1) Read hardware version</br>
Command</br>
RHVER 1\n</br>
(Read hareware version for device1)</br>
</br>
Response</br>
RHVER 1,0,0.0.1\n</br>
(Device1 response, No error, version 0.0.1)</br>
</br>
Example2) Write a value 10 for a register 3 of Device 1</br>
Command</br>
WREG 1,3,10\n</br>
</br>
Response</br>
WREG 1,0\n</br>
(Device1 response, No error)</br>

3.1.1. Text protocol test
- Program : SerialPortMon

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/serialportmon_open.png)

- Example
  Last character shoud be \n

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/text_cmd_send.png)

3.2. Binary protocol

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/bin_protocol_format.png)

3.2.1 Binary protocol test
- Visual Studio 2019 project : fl_console_app1

3.2.1.1 Host simulator

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/cli_host_simulator.png)

3.2.1.2 Select COM port
- Default : COM3

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/cli_com_port_name.png)


3.2.1.3 Open COM port
- Make sure that the selected COM port is opened after 'start' command

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/cli_start.png)


3.2.1.4 Binary protocol test

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/cli_bin_protocol_test.png)


3.2.1.5 Binary protocol test result
- The last Write register is for testing FAIL message(FAIL is expected result)

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/cli_bin_protocol_test_result.png)



**4. Implemented commands**
- Read hareware version : RHVER(text command)
- Read firmware version : RFVER(text command)
- Boot mode : BMODE(text command)</br>
  protocol implemented, no actual Boot mode set
- Reset : RESET(text command)</br>
  protocol implemented, no actual reset
- Read register : RREG(text command)</br>
  protocol implemented, no actual register read
- Write register : WREG(text command)</br>
  protocol implemented, no actual register write

**5. Adding common sources to each firmware project**

5.1. Adding a new file

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/new_file.png)

5.2. File link

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/link_to_file_system.png)

5.3. Input file name

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/input_file_name.png)


**6. Parser function usage example**

6.1. Getting parsed result as a function out parameter
- Visual Studio 2019 : fl_bin_message_parser_unit_test.cpp (fl_unittest project)
<pre>
<code>
fl_bin_msg_full_t         _parsed_msg;
                     :
for (_i = 0; _i < _len; _i++)
{
  _ret = fl_bin_msg_parser_parse(&_bin_parser, _packet_buf[_i], &_parsed_msg);
}          
</code>
</pre>

6.2. Using parser's internal buffer
- F722ZE_Parser_Poll : main.c, message_processing, FL_MSG_ID_READ_REGISTER parsing
<pre>
<code>
if (g_app.proto_mgr.parser_handle.arg_count == 1)
{
  fl_read_reg_t* rreg = (fl_read_reg_t*)&(g_app.proto_mgr.parser_handle.payload);

  // TODO : Read a value from a register.
  g_app.proto_mgr.out_length = sprintf((char*)g_app.proto_mgr.out_buf, "%s %ld,%d,%ld,%d%c",
              fl_txt_msg_get_message_name(g_app.proto_mgr.parser_handle.msg_id),
              g_app.proto_mgr.parser_handle.device_id,
              FL_OK,
              rreg->address,
              10,
              FL_TXT_MSG_TAIL);
}     
</code>
</pre>

6.3. Using callback function
- F722ZE_Parser_INT_RX : fw_app.c, on_message_parsed
