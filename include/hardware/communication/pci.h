#ifndef __LOVE_PCI_H
#define __LOVE_PCI_H

#include <commons/types.h>
#include <hardware/communication/interrupts.h>
#include <hardware/communication/port.h>
#include <hardware/driver/driver.h>
#include <terminal/term.h>

namespace RinOS {
namespace Hardware {
namespace Communication {

enum BaseAddressRegisterType { MemoryMapping = 0, InputOutput = 1 };

class BaseAddressRegister {
 public:
  bool prefetchable;
  u8 *address;
  u32 size;
  BaseAddressRegisterType type;
};
class PCIDescriptor {
 public:
  u32 port_base;
  u32 interrupt;

  u16 bus;
  u16 device;
  u16 function;

  u16 vendor_id;
  u16 device_id;

  u8 class_id;
  u8 subclass_id;
  u8 interface_id;

  u8 revision;

  PCIDescriptor();
  ~PCIDescriptor();
};

class PCIController {
  RinOS::Hardware::Communication::Port32Bit data_port;
  RinOS::Hardware::Communication::Port32Bit command_port;

 public:
  PCIController();
  ~PCIController();

  u32 Read(u16 bus, u16 device, u16 function, u32 register_offset);
  void Write(u16 bus, u16 device, u16 function, u32 register_offset, u32 data);
  bool DeviceHasFunctions(u16 bus, u16 device);

  void select_drivers(
      RinOS::Hardware::Driver::DriverManager *driver_manager,
      RinOS::Hardware::Communication::InterruptManager *interrupts);
  RinOS::Hardware::Driver::Driver *GetDriver(
      PCIDescriptor *dev,
      RinOS::Hardware::Communication::InterruptManager *interrupts);
  PCIDescriptor GetDeviceDescriptor(u16 bus, u16 device, u16 function);
  BaseAddressRegister GetBaseAddressRegister(u16 bus, u16 device, u16 function,
                                             u16 bar);
};

}  // namespace Communication
}  // namespace Hardware
}  // namespace RinOS

#endif