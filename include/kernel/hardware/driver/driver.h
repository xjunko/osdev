#ifndef __LOVE_DRIVER_H
#define __LOVE_DRIVER_H

#include <global/types.h>
namespace RinOS {
namespace Hardware {
namespace Driver {
class Driver {
 public:
  Driver();
  ~Driver();

  virtual void activate();
  virtual int reset();
  virtual void deactivate();
};

class DriverManager {
 private:
  Driver* drivers[512];
  int num_drivers;

 public:
  DriverManager();
  ~DriverManager();

  void add_driver(Driver*);
  void activate_all();
};

}  // namespace Driver
}  // namespace Hardware
}  // namespace RinOS

#endif