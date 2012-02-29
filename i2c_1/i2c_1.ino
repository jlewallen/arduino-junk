#include <Wire.h>

#define MASTER

typedef struct _message_t {
  char code;
  _message_t *next;
} message_t;

static message_t *queue = NULL;

void blink(int16_t times, int16_t onTime, int16_t offTime) {
  for (int i = 0; i < times; ++i) {
    digitalWrite(13, HIGH);
    delay(onTime);
    digitalWrite(13, LOW);
    delay(offTime);
  }
}

void blink(int16_t times) {
  blink(times, 250, 250);
}

void received(int16_t howMany) {
  while (Wire.available()) {
    char m = Wire.read();
    message_t *message = (message_t *)malloc(sizeof(message_t));
    message->next = queue;
    message->code = m;
    queue = message;
  }
}

static byte number = 0;
static byte states[] = {
  0b00000000,
  0b00100001,
  0b00100011,
  0b00100111,
  0b00101111,
  0b00111111
};


void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  
  #if defined(MASTER)
  Wire.begin();
  #else
  Wire.begin(1);
  Wire.onReceive(received);
  blink(3);
  #endif
}

void loop() {
  digitalWrite(13, HIGH);
  Wire.beginTransmission(1);
  Wire.write(states[number]);
  Wire.endTransmission();
  delay(100);
  digitalWrite(13, LOW);
  delay(1000);

  number = (number + 1) % (sizeof(states));
  if (number == 0) {
    Wire.beginTransmission(1);
    Wire.write(1 << 6);
    Wire.write((byte)100);
    Wire.write((byte)0);
    Wire.write((byte)0);
    Wire.endTransmission();
    delay(1000);
    Wire.beginTransmission(1);
    Wire.write(1 << 6);
    Wire.write((byte)0);
    Wire.write((byte)100);
    Wire.write((byte)0);
    Wire.endTransmission();
    delay(1000);
    Wire.beginTransmission(1);
    Wire.write(1 << 6);
    Wire.write((byte)0);
    Wire.write((byte)0);
    Wire.write((byte)100);
    Wire.endTransmission();
    delay(1000);
  }

  /*
  Wire.requestFrom(0x1, 1);
  if (Wire.available()) {
    number = Wire.read() % 5;
  }

  while (queue != NULL) {
    message_t *next = queue->next;
    number = queue->code % 5;
    free(queue);
    queue = next;
  }
  */
}
