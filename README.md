# F722ZE_Parser_Example
Text message parser example for NUCLEO F722ZE
- USART, USB CDC interface
- Base proejct templates for text message protocol

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

**4. Implemented commands**
- Read hareware version : RHVER
- Read firmware version : RFVER
- Boot mode : BMODE(protocol implemented, no actual Boot mode set)
- Reset : RESET(protocol implemented, no actual reset)
- Read register : RREG(protocol implemented, no actual register read)
- Write register : WREG(protocol implemented, no actual register write)

**5. Adding common sources to each firmware project**

5.1. Adding a new file

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/new_file.png)

5.2. File link

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/link_to_file_system.png)

5.3. Input file name

![](https://github.com/heecheol-jung/F722ZE_Parser_Example/blob/main/pictures/input_file_name.png)

