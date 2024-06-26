#include <hardware/driver/driver.h>

using namespace RinOS::Hardware::Driver;

Driver::Driver() {}

Driver::~Driver() {}

void Driver::activate() {}

void Driver::deactivate() {}

int Driver::reset() {}

DriverManager::DriverManager() { num_drivers = 0; }

void DriverManager::add_driver(Driver* drv) {
  drivers[num_drivers] = drv;
  num_drivers++;
}

void DriverManager::activate_all() {
  for (int i = 0; i < num_drivers; i++) {
    drivers[i]->activate();
  }
}