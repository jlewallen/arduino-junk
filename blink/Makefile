# Your Arduino environment.
ARD_REV = 22
ARD_HOME = /Applications/Arduino.app/Contents/Resources/Java
AVR_HOME = $(ARD_HOME)/hardware/tools/avr
ARD_BIN = $(AVR_HOME)/bin
AVRDUDE = $(ARD_BIN)/avrdude
AVRDUDE_CONF = $(AVR_HOME)/etc/avrdude.conf

# Your favorite serial monitor.
MON_CMD = screen
MON_SPEED = 9600

# Board settings.
BOARD = uno
PORT = /dev/tty.usbserial-AH00S85C
VARIANTS = $(ARD_HOME)/hardware/arduino/variants/standard
# PROGRAMMER = stk500v1
PROGRAMMER = arduino

# Where to find header files and libraries.
INC_DIRS = ./inc $(VARIANTS)
LIB_DIRS = $(addprefix $(ARD_HOME)/libraries/, $(LIBS))
LIBS =

include ../Makefile.master

