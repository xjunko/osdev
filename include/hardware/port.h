#ifndef __LOVE_PORT_H
#define __LOVE_PORT_H

#include <commons/types.h>

class Port {
 protected:
  u16 port_number;
  Port(u16 port_number);
  ~Port();
};

class Port8Bit : public Port {
 public:
  Port8Bit(u16 port_number);
  ~Port8Bit();

  virtual void Write(u8 data);
  virtual u8 Read();
};

class Port8BitSlow : public Port8Bit {
 public:
  Port8BitSlow(u16 port_number);
  ~Port8BitSlow();

  virtual void Write(u8 data);
};

class Port16Bit : public Port {
 public:
  Port16Bit(u16 port_number);
  ~Port16Bit();

  virtual void Write(u16 data);
  virtual u16 Read();
};

class Port32Bit : public Port {
 public:
  Port32Bit(u16 port_number);
  ~Port32Bit();

  virtual void Write(u32 data);
  virtual u32 Read();
};

#endif  // __LOVE_PORT_H