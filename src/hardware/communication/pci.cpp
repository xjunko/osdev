#include <commons/types.h>
#include <hardware/communication/interrupts.h>
#include <hardware/communication/pci.h>
#include <hardware/communication/port.h>
#include <hardware/driver/driver.h>
#include <terminal/term.h>

using namespace RinOS::Hardware::Communication;

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
    RinOS::Hardware::Driver::DriverManager *driverManager) {
  for (int bus = 0; bus < 8; bus++) {
    for (int device = 0; device < 32; device++) {
      int num_functions = DeviceHasFunctions(bus, device) ? 8 : 1;
      for (int function = 0; function < num_functions; function++) {
        PCIDescriptor dev = GetDeviceDescriptor(bus, device, function);

        if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF) {
          break;  // No more functions.
        }

        RinOS::Terminal::log("PCI", "BUS:");
        RinOS::Terminal::printf("-> ");
        RinOS::Terminal::print_hex(bus & 0xFF);

        RinOS::Terminal::log("PCI", "Device:");
        RinOS::Terminal::printf("-> ");
        RinOS::Terminal::print_hex(device & 0xFF);

        RinOS::Terminal::log("PCI", "Function:");
        RinOS::Terminal::printf("-> ");
        RinOS::Terminal::print_hex(function & 0xFF);

        RinOS::Terminal::log("PCI", "Vendor:");
        RinOS::Terminal::printf("-> ");
        RinOS::Terminal::print_hex((dev.vendor_id & 0xFF00) >> 8);
        RinOS::Terminal::print_hex(dev.vendor_id & 0xFF);
        RinOS::Terminal::printf("-> ");
        RinOS::Terminal::log("PCI", "Device:");
        RinOS::Terminal::printf("-> ");
        RinOS::Terminal::print_hex((dev.device_id & 0xFF00) >> 8);
        RinOS::Terminal::print_hex(dev.device_id & 0xFF);
        RinOS::Terminal::printf("\n");
      }
    }
  };
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