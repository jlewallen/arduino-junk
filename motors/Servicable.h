#ifndef SERVICABLE_H
#define SERVICABLE_H

class Servicable {
public:
  virtual ~Servicable () { }
  virtual void service() = 0;
};

#endif
