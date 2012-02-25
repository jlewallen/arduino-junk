#ifndef SERVICABLE_H
#define SERVICABLE_H

#include "Common.h"

class Servicable {
public:
  virtual ~Servicable () { }
  virtual void begin() = 0;
  virtual void service() = 0;
};

#endif
