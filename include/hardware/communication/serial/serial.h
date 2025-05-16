#ifndef __LOVE_HW_COM_SERIAL_DEBUG_H
#define __LOVE_HW_COM_SERIAL_DEBUG_H

#include <commons/types.h>
#include <hardware/communication/port.h>

namespace RinOS {
namespace Hardware {
namespace Communication {
namespace Serial {

class COMLog {
  RinOS::Hardware::Communication::Port8Bit com_port_0;
  RinOS::Hardware::Communication::Port8Bit com_port_1;
  RinOS::Hardware::Communication::Port8Bit com_port_2;
  RinOS::Hardware::Communication::Port8Bit com_port_3;
  RinOS::Hardware::Communication::Port8Bit com_port_4;
  RinOS::Hardware::Communication::Port8Bit com_port_5;

 public:
  explicit COMLog(u16 base_com);
  ~COMLog();
  void Initialize();
  bool IsReady();
  void Write(char c);
  void WriteString(char* str);
  void Print(char* str);
};

extern COMLog COM1;
void InitializeGlobalCOM();

}  // namespace Serial
}  // namespace Communication
}  // namespace Hardware
}  // namespace RinOS

#endif