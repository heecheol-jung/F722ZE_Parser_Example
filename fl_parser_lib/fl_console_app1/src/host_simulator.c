#include <stdio.h>
#include <tchar.h>
#include <process.h>
#include <time.h>
#include "host_simulator.h"
#include "fl_util.h"

#define SERIAL_INPUT_Q_SIZE     (4096)
#define SERIAL_OUTPUT_Q_SIZE    (4096)

static void print_error_message(DWORD error_code);
static void serial_open(host_simulator_t* handle);
static void serial_close(host_simulator_t* handle);

///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////
void hs_init(host_simulator_t* handle)
{
  memset(handle, 0, sizeof(host_simulator_t));
  handle->serial_handle = INVALID_HANDLE_VALUE;
  handle->thread_handle = INVALID_HANDLE_VALUE;

  fl_bin_msg_parser_init(&handle->app_bin_parser);
}

// Start a host simulator.
int hs_start(host_simulator_t* handle)
{
  serial_open(handle);

  if (hs_is_serial_open(handle) != 1)
  {
    return 1;
  }

  return 0;
}

// Stop a host simulator.
int hs_stop(host_simulator_t* handle)
{
  if (hs_is_serial_open(handle) == 1)
  {
    serial_close(handle);
  }

  return 0;
}

uint8_t hs_is_serial_open(host_simulator_t* handle)
{
  if (handle->serial_handle == INVALID_HANDLE_VALUE)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

// Send a packet.
int hs_send_packet(host_simulator_t* handle, uint8_t* buf, uint16_t len)
{
  if (hs_is_serial_open(handle) != 1)
  {
    return 1;
  }

  DWORD written_bytes = 0;

  if (WriteFile(handle->serial_handle, buf, len, &written_bytes, NULL) == TRUE)
  {
    return 0;
  }
  else
  {
    return 2;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Private functions
///////////////////////////////////////////////////////////////////////////////
static void serial_open(host_simulator_t* handle)
{
  HANDLE serial_handle = INVALID_HANDLE_VALUE;
  TCHAR path[MAX_PATH] = { 0 };
  const int buf_size = sizeof(path) / sizeof(path[0]) - 1;
  int len = 0;
  DCB             dcb;
  COMMTIMEOUTS    cto;

  if (strlen(handle->com_port_name) == 0)
  {
    APP_LIB_DEBUG_PRINT(("[%s] Invalid COM port name\n", __FUNCTION__));
  }

  len = _sntprintf_s(path, buf_size, MAX_PATH - 1, _T("\\\\.\\%s"), handle->com_port_name);
  serial_handle = CreateFile(
    path,     // address of name of the communications device
    GENERIC_READ | GENERIC_WRITE,          // access (read-write) mode
    0,                  // share mode
    NULL,               // address of security descriptor
    OPEN_EXISTING,      // how to create
    0,                  // file attributes
    NULL                // handle of file with attributes to copy
  );

  if (serial_handle == INVALID_HANDLE_VALUE)
  {
    print_error_message(GetLastError());
    APP_LIB_DEBUG_PRINT(("[%s] CreateFile failed\n", __FUNCTION__));
    return;
  }

  if (SetCommMask(serial_handle, EV_RXCHAR) != TRUE)
  {
    print_error_message(GetLastError());
    APP_LIB_DEBUG_PRINT(("[%s] SetCommMask failed\n", __FUNCTION__));

    serial_close(handle);
    return;
  }

  if (SetupComm(serial_handle, SERIAL_INPUT_Q_SIZE, SERIAL_OUTPUT_Q_SIZE) != TRUE)
  {
    print_error_message(GetLastError());
    APP_LIB_DEBUG_PRINT(("[%s] SetupComm failed\n", __FUNCTION__));

    serial_close(handle);
    return;
  }

  if (PurgeComm(serial_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR) != TRUE)
  {
    print_error_message(GetLastError());
    APP_LIB_DEBUG_PRINT(("[%s] PurgeComm failed\n", __FUNCTION__));

    serial_close(handle);
    return;
  }

  cto.ReadIntervalTimeout = MAXDWORD;
  cto.ReadTotalTimeoutMultiplier = 0;
  cto.ReadTotalTimeoutConstant = 0;
  cto.WriteTotalTimeoutConstant = 0;
  if (SetCommTimeouts(serial_handle, &cto) != TRUE)
  {
    print_error_message(GetLastError());
    APP_LIB_DEBUG_PRINT(("[%s] SetCommTimeouts failed\n", __FUNCTION__));

    serial_close(handle);
    return;
  }

  if (GetCommState(serial_handle, &dcb) != TRUE)
  {
    print_error_message(GetLastError());
    APP_LIB_DEBUG_PRINT(("[%s] GetCommState failed\n", __FUNCTION__));

    serial_close(handle);
    return;
  }

  dcb.BaudRate = 115200;
  dcb.ByteSize = 8;
  dcb.fParity = FALSE;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  if (SetCommState(serial_handle, &dcb) != TRUE)
  {
    print_error_message(GetLastError());
    APP_LIB_DEBUG_PRINT(("[%s] SetCommState failed\n", __FUNCTION__));

    serial_close(handle);
    return;
  }
  handle->serial_handle = serial_handle;
  APP_LIB_DEBUG_PRINT(("[%s] Serial port opened.\n\n", __FUNCTION__));
}

static void serial_close(host_simulator_t* handle)
{
  if (handle->serial_handle != INVALID_HANDLE_VALUE)
  {
    CloseHandle(handle->serial_handle);
    handle->serial_handle = INVALID_HANDLE_VALUE;

    APP_LIB_DEBUG_PRINT(("[%s] Serial port closed\n", __FUNCTION__));
  }
}

static void print_error_message(DWORD error_code)
{
  LPVOID lpMsgBuf = NULL;
  LPVOID lpDisplayBuf = NULL;

  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    error_code,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR)&lpMsgBuf,
    0, NULL);

  if (lpMsgBuf != NULL)
  {
    _tprintf(_T("%s"), (TCHAR*)lpMsgBuf);
    LocalFree(lpMsgBuf);
  }
}