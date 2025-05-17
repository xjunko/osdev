#include <global/types.h>
#include <kernel/hardware/communication/serial/serial.h>
#include <stdio.h>

#include <cstdarg>

using namespace RinOS::Hardware::Communication::Serial;

COMLog RinOS::Hardware::Communication::Serial::COM1(0x3F8);

void RinOS::Hardware::Communication::Serial::InitializeGlobalCOM() {
  RinOS::Hardware::Communication::Serial::COM1.Initialize();

  COM1.Print("[COM] COM1 Initialized!");
}

COMLog::COMLog(u16 base_com)
    : com_port_0(base_com),
      com_port_1(base_com + 1),
      com_port_2(base_com + 2),
      com_port_3(base_com + 3),
      com_port_4(base_com + 4),
      com_port_5(base_com + 5) {}
COMLog::~COMLog() {}

void COMLog::Initialize() {
  this->com_port_1.Write(0x0);
  this->com_port_3.Write(0x80);
  this->com_port_0.Write(0x03);
  this->com_port_1.Write(0x0);
  this->com_port_3.Write(0x03);
  this->com_port_2.Write(0xC7);
  this->com_port_4.Write(0x0B);
}

bool COMLog::IsReady() { return (this->com_port_5.Read() & 0x20) != 0; }

void COMLog::Write(char c) {
  while (!this->IsReady()) {
  }
  this->com_port_0.Write(c);
}

void COMLog::WriteString(char* str) {
  while (*str) {
    if (*str == '\n') {
      this->Write('\r');
    }
    this->Write(*str++);
  }
}

extern "C" void COMLog::Print(const char* fmt, ...) {
  __builtin_va_list args;
  __builtin_va_start(args, fmt);
  printf(fmt, args);
  printf("\n");
  __builtin_va_end(args);
}

extern "C" void putchar_(char c) { COM1.Write(c); }