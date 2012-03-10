#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>

#define memzero(a, sz)   memset(a, 0, sz)

#define printlnf(s, ...) printf(s "\n\r", ## __VA_ARGS__)

#endif
