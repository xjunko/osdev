#include <commons/types.h>
#include <hardware/communication/interrupts.h>
#include <hardware/communication/pci.h>
#include <hardware/communication/port.h>
#include <hardware/communication/serial/serial.h>
#include <hardware/driver/driver.h>

using namespace RinOS::Hardware::Communication;
using namespace RinOS::Hardware::Communication::Serial;

PCIDescriptor::PCIDescriptor() {}

PCIDescriptor::~PCIDescriptor() {}

PCIController::PCIController() : data_port(0xCFC), command_port(0xCF8) {}

PCIController::~PCIController() {}

u32 PCIController::Read(u16 bus, u16 device, u16 function,
                        u32 register_offset) {
  u32 id = 0x1 << 31 | (bus & 0xFF) << 16 | ((device & 0x1F) << 11) |
           ((function & 0x07) << 8) | ((register_offset & 0xFC));
  command_port.Write(id);
  return data_port.Read() >> (8 * (register_offset % 4));
}

void PCIController::Write(u16 bus, u16 device, u16 function,
                          u32 register_offset, u32 data) {
  u32 id = 0x1 << 31 | (bus & 0xFF) << 16 | ((device & 0x1F) << 11) |
           ((function & 0x07) << 8) | ((register_offset & 0xFC));
  command_port.Write(id);
  data_port.Write(data);
}

bool PCIController::DeviceHasFunctions(u16 bus, u16 device) {
  return Read(bus, device, 0, 0x0E) & (1 << 7);
}

void PCIController::select_drivers(
    RinOS::Hardware::Driver::DriverManager *driver_manager,
    RinOS::Hardware::Communication::InterruptManager *interrupts) {
  for (int bus = 0; bus < 8; bus++) {
    for (int device = 0; device < 32; device++) {
      int num_functions = DeviceHasFunctions(bus, device) ? 8 : 1;
      for (int function = 0; function < num_functions; function++) {
        PCIDescriptor dev = GetDeviceDescriptor(bus, device, function);

        if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF) {
          continue;
        }

        for (int bar_num = 0; bar_num < 6; bar_num++) {
          BaseAddressRegister bar =
              GetBaseAddressRegister(bus, device, function, bar_num);

          if (bar.address && bar.type == InputOutput) {
            dev.port_base = (u32)bar.address;
          }

          RinOS::Hardware::Driver::Driver *driver = GetDriver(&dev, interrupts);
          if (driver != 0) {
            driver_manager->add_driver(driver);
          }
        }

        COM1.Print("[PCI] Bus: 0x%X", bus & 0xFF);
        COM1.Print("[PCI] Function: 0x%X", function & 0xFF);
        COM1.Print("[PCI] Vendor: 0x%X", (dev.vendor_id & 0xFF00) >> 8);
      }
    }
  };
}

RinOS::Hardware::Driver::Driver *PCIController::GetDriver(
    PCIDescriptor *dev,
    RinOS::Hardware::Communication::InterruptManager *interrupts) {
  return 0;
}

PCIDescriptor PCIController::GetDeviceDescriptor(u16 bus, u16 device,
                                                 u16 function) {
  PCIDescriptor result;

  result.bus = bus;
  result.device = device;
  result.function = function;

  result.vendor_id = Read(bus, device, function, 0x00);
  result.device_id = Read(bus, device, function, 0x02);

  result.class_id = Read(bus, device, function, 0x0b);
  result.subclass_id = Read(bus, device, function, 0x0a);
  result.interface_id = Read(bus, device, function, 0x09);

  result.revision = Read(bus, device, function, 0x08);
  result.interrupt = Read(bus, device, function, 0x3c);

  return result;
}

BaseAddressRegister PCIController::GetBaseAddressRegister(u16 bus, u16 device,
                                                          u16 function,
                                                          u16 bar) {
  BaseAddressRegister result;

  u32 header_type = Read(bus, device, function, 0x0E) & 0x7F;
  int max_bars = 6 - (4 * header_type);

  if (bar >= max_bars) {
    return result;
  }

  u32 bar_value = Read(bus, device, function, 0x10 + 4 * bar);
  result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;

  if (result.type == MemoryMapping) {
    switch ((bar_value >> 1) & 0x3) {
      case 0:  // 32bit
      case 1:  // 20bit
      case 2:  // 64bit
        break;
    }
    result.prefetchable = ((bar_value >> 3) & 0x1) == 0x1;
  } else {
    result.address = (u8 *)(bar_value & ~0x3);
    result.prefetchable = false;
  }

  return result;
}