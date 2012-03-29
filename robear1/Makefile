USER_LIBS = jlewallen L3G4200D/L3G4200D LSM303/LSM303 Arduino-PID-Library/PID_v1
SYSTEM_LIBS = Servo Wire Wire/utility
MON_SPEED = 57600
USER_C_FLAGS=-Wl,-u,vfprintf -lprintf_flt -DDEBUG -DDEBUG_VIRTUAL_BUMPER -DDEBUG_ESC
include ../Makefile.env
BOARD = atmega328
include ../Makefile.master
