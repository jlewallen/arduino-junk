#ifndef Console_h
#define Console_h

#if defined CONSOLE
#define LOG(v)   Serial.println(v)
#else
#define LOG(v)
#endif

#endif
