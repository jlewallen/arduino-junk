#ifndef DEBUGGABLE_H
#define DEBUGGABLE_H

class Debuggable {
public:
  virtual ~Debuggable () { }
  virtual boolean debug() = 0;
};

#ifdef DEBUG
#define DPRINTF(msg, ...)   printf(msg, ## __VA_ARGS__)
#else
#define DPRINTF(msg, ...)   {}
#endif

#endif
