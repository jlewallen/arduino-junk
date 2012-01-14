#include <Wire.h>

#define MASTER

typedef struct _message_t {
  char code;
  _message_t *next;
} message_t;

static message_t *queue = NULL;

void blink(int16_t times) {
  blink(times, 250, 250);
}

void blink(int16_t times, int16_t onTime, int16_t offTime) {
  for (int i = 0; i < times; ++i) {
    digitalWrite(13, HIGH);
    delay(onTime);
    digitalWrite(13, LOW);
    delay(offTime);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  
  #if defined(MASTER)
  Wire.begin();
  blink(2);
  #else
  Wire.begin(1);
  Wire.onReceive(received);
  blink(3);
  #endif
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

void loop() {
  #if defined(MASTER)
  Wire.beginTransmission(1);
  Wire.write("1");
  Wire.endTransmission();
  #else
  
  #endif
  delay(1000);
  
  while (queue != NULL) {
    message_t *next = queue->next;
    switch (queue->code) {
      case '1':
        blink(3, 100, 50);
        break;
    }
    free(queue);
    queue = next;
  }
}
