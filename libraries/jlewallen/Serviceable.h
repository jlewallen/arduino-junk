#ifndef SERVICEABLE_H
#define SERVICEABLE_H

class Serviceable {
public:
  virtual ~Serviceable() { }
  virtual void service() = 0;
};

#endif
