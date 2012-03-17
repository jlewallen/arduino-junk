#include <Wire.h>
#include <printf.h>

#define MASTER

typedef struct _message_t {
  uint8_t size;
  uint8_t data[64];
  _message_t *next;
} message_t;

static message_t *queue = NULL;
static uint32_t previous = 0;
static uint32_t bytesReceived = 0;
static uint32_t bytesReceivedPrinted = 0;
static uint32_t bytesSent = 0;
static uint32_t bytesSentPrinted = 0;

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
  #ifdef CAPTURE
  message_t *message = (message_t *)malloc(sizeof(message_t));
  message->size = 0;
  // uint8_t previous = millis();
  while (message->size < 1/* && millis() - previous < 50*/) {
    if (Wire.available()) {
      message->data[message->size++] = Wire.read();
      // previous = millis();
      bytesReceived++;
    }
  }
  message->next = queue;
  queue = message;
  #else
  while (1) {
    while (Wire.available() == 0) { }
    uint8_t i = Wire.read();
    bytesReceived++;
    if (i == 0) {
      break;
    }
  }
  #endif
}

void requested() {
  Wire.write((byte)0xdd);
  bytesSent++;
}

void setup() {
  Serial.begin(9600);
  printf_begin();
  pinMode(13, OUTPUT);
  
  Wire.begin(0x21);
  Wire.onReceive(received);
  Wire.onRequest(requested);
  blink(3);

  printf("Listening on 0x21\n\r");
}

void loop() {
  if (millis() - previous > 500) {
    printf(".");
    digitalWrite(13, !digitalRead(13));
    previous = millis();
  }
  /*
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
  */

  /*
  Wire.requestFrom(0x1, 1);
  if (Wire.available()) {
    number = Wire.read() % 5;
  }
  */
  if (bytesReceivedPrinted != bytesReceived || bytesSentPrinted != bytesSent) {
    bytesReceivedPrinted = bytesReceived;
    bytesSentPrinted = bytesSent;
    printf("Received %lu %lu\n\r", bytesReceived, bytesSent);
  }
  while (queue != NULL) {
    message_t *next = queue->next;
    printf("Message sz=%d (%lu)\n\r", queue->size, bytesReceived);
    free(queue);
    queue = next;
  }

  delay(100);
}
