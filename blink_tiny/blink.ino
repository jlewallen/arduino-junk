#include <Arduino.h>
extern "C" {
#include "usiTwiSlave.h"
}

#define memzero(p, sz)  memset(p, 0, sz)
#define RED_PIN         0
#define GREEN_PIN       9
#define YELLOW_PIN      10   
#define WHITE_PIN       8
#define PURPLE_PIN      1
#define RGB_G_PIN       2
#define RGB_B_PIN       3
#define RGB_R_PIN       5

#define RED_MASK       (1 << 0)
#define YELLOW_MASK    (1 << 1)
#define GREEN_MASK     (1 << 2)
#define PURPLE_MASK    (1 << 3)
#define WHITE_MASK     (1 << 4)

#define SET_LED_MASK   (1 << 5)
#define SET_RGB_MASK   (1 << 6)
#define SET_BLINK_MASK (1 << 7)
#define IS_SET(v, m)   (((v) & (m)) == m)

#define I2C_SLAVE_ADDR  0x01

typedef struct command_t_ {
  uint8_t opcode;
  uint8_t blinks;
  uint8_t timeOn;
  uint8_t timeOff;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  command_t_ *np;
} command_t;

static command_t memory;

class LightBoard {
public:
  void begin() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(PURPLE_PIN, OUTPUT);
    pinMode(WHITE_PIN, OUTPUT);
    pinMode(RGB_R_PIN, OUTPUT);
    pinMode(RGB_G_PIN, OUTPUT);
    pinMode(RGB_B_PIN, OUTPUT);
  }

  void off() {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(PURPLE_PIN, LOW);
    digitalWrite(WHITE_PIN, LOW);
    analogWrite(RGB_R_PIN, 0);
    analogWrite(RGB_G_PIN, 0);
    analogWrite(RGB_B_PIN, 0);
  }

  void on() {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(YELLOW_PIN, HIGH);
    digitalWrite(PURPLE_PIN, HIGH);
    digitalWrite(WHITE_PIN, HIGH);
    analogWrite(RGB_R_PIN, 0);
    analogWrite(RGB_G_PIN, 0);
    analogWrite(RGB_B_PIN, 0);
  }

  void service() {
    if (usiTwiDataInReceiveBuffer()) {
      memzero(&memory, sizeof(command_t));
      deserialize(&memory);
      execute(&memory);
    }
  }

  void deserialize(command_t *reading) {
    reading->opcode = usiTwiReceiveByte();
    if (IS_SET(reading->opcode, SET_RGB_MASK)) {
      reading->r = usiTwiReceiveByte();
      reading->g = usiTwiReceiveByte();
      reading->b = usiTwiReceiveByte();
    }
    else if (IS_SET(reading->opcode, SET_BLINK_MASK)) {
      if (reading->blinks == 0) {
        reading->blinks = usiTwiReceiveByte();
        reading->timeOn = usiTwiReceiveByte();
        reading->timeOff = usiTwiReceiveByte();
        deserialize(reading);
      }
    }
  }

  void execute(command_t *command) {
    if (command->opcode == 0x00) {
      off();
    }
    else if (command->opcode == 0xff) {
      on();
    }
    else if (IS_SET(command->opcode, SET_LED_MASK)) {
      digitalWrite(RED_PIN,    IS_SET(command->opcode,    RED_MASK));
      digitalWrite(YELLOW_PIN, IS_SET(command->opcode, YELLOW_MASK));
      digitalWrite(GREEN_PIN,  IS_SET(command->opcode,  GREEN_MASK));
      digitalWrite(PURPLE_PIN, IS_SET(command->opcode, PURPLE_MASK));
      digitalWrite(WHITE_PIN,  IS_SET(command->opcode,  WHITE_MASK));
    }
    else if (IS_SET(command->opcode, SET_RGB_MASK)) {
      analogWrite(RGB_R_PIN, command->r);
      analogWrite(RGB_G_PIN, command->g);
      analogWrite(RGB_B_PIN, command->b);
    }
  }
};

static LightBoard board;

int main() {
  init();

  usiTwiSlaveInit(I2C_SLAVE_ADDR);

  board.begin();
  board.on();
  delay(100);
  board.off();

  for (;;) {
    /*
    if (millis() - previous > 250) {
      previous = millis();
      if (enabled) {
        board.on();
        enabled = false;
      }
      else {
        board.off();
        enabled = true;
      }
    }
    */
    board.service();
  }

  return 0;
}
