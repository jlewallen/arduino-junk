#ifndef DISPLAY_H
#define DISPLAY_H

#define LIGHT_BOARD_ADDRESS 1

class Display {
private:
public:
  void begin() {
  }

  void ready() {
    for (byte i = 0; i < 3; ++i) {
      send(BLINK_SET_LED(WHITE_MASK | PURPLE_MASK));
      delay(50);
      send(BLINK_ALL_OFF);
      delay(100);
    }
  }

  void stopped() {
    send(BLINK_ALL_OFF);
  }

  void stalled() {
    send(BLINK_ALL_OFF, BLINK_SET_LED(RED_MASK));
  }

  void searching() {
    send(BLINK_ALL_OFF, BLINK_SET_LED(GREEN_MASK));
  }

  void obstructed() {
    send(BLINK_ALL_OFF, BLINK_SET_LED(YELLOW_MASK));
  }

  void avoiding() {
    send(BLINK_ALL_OFF, BLINK_SET_LED(YELLOW_MASK));
  }

  void send(byte b0, byte b1) {
    Wire.beginTransmission(LIGHT_BOARD_ADDRESS);
    Wire.write(b0);
    Wire.write(b1);
    Wire.endTransmission();
  }

  void send(byte b) {
    Wire.beginTransmission(LIGHT_BOARD_ADDRESS);
    Wire.write(b);
    Wire.endTransmission();
  }
};

#endif
