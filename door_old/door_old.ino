#include <SPI.h>
#include <Ethernet.h>

#define CONSOLE

#if defined CONSOLE
#define LOG(v)   Serial.println(v)
#else
#define LOG(v)
#endif

#define HOST        "192.168.0.127"
#define PIN_LED     8
#define PIN_READY   7
#define PIN_ERROR   6
#define PIN_SWITCH  9
#define RETRIES     3

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 5 };
byte gateway[] = { 192, 168, 0, 1 };
byte subnet[] = { 255, 255, 255, 0 };
byte dns[] = { 192, 168, 0, 1 };

static boolean opened = false;
static boolean suppressed = false;
static long previous = 0;
static EthernetClient client;

void setup() {
  #if defined(CONSOLE)
  Serial.begin(9600);
  #endif

  LOG("Ready..");

  delay(1000);

  Ethernet.begin(mac, ip, gateway, dns, subnet);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_READY, OUTPUT);
  pinMode(PIN_ERROR, OUTPUT);
  pinMode(PIN_SWITCH, INPUT);
}

boolean doorClosed() {
  return !digitalRead(PIN_SWITCH);
}

boolean notify(const char *message) {
  LOG("Posting...");
  if (client.connect(HOST, 8080)) {
    client.println("POST /tweet HTTP/1.0");
    client.println("Host: " HOST);
    client.print("Content-Length: ");
    client.println(strlen(message) + strlen("status="));
    client.println();
    client.print("status=");
    client.println(message);
    delay(50);
    client.stop();
    LOG("POST Done...");
  }
  else {
    LOG("POST Failed...");
    return false;
  }
}

void notify(long elapsed) {
  String message = "door open for ";
  message += elapsed;
  message += "ms";
  char messageBody[64];
  message.toCharArray(messageBody, 64);
  for (int i = 0; i < RETRIES; ++i) {
    if (notify(messageBody)) {
      break;
    }
    else {
      digitalWrite(PIN_ERROR, HIGH);
      delay(500);
      digitalWrite(PIN_ERROR, LOW);
    }
  }
}

void loop() {
  if (suppressed && millis() - previous < 1000) {
    delay(10);
    return;
  }
  if (suppressed) {
    LOG("Disabling Suppressing...");
  }

  suppressed = false;
  digitalWrite(PIN_LED, LOW);
  if (doorClosed()) {
    if (opened) {
      opened = false;
      long elapsed = millis() - previous;
      #if defined(CONSOLE)
      Serial.print("Closed: ");
      Serial.print(elapsed);
      Serial.print(" ");
      #endif
      if (elapsed > 50) {
        LOG("Recording");
        digitalWrite(PIN_READY, HIGH);
        notify(elapsed);
        digitalWrite(PIN_READY, LOW);
        digitalWrite(PIN_LED, HIGH);
        previous = millis();
        suppressed = true;
      }
      else {
        LOG("Ignoring");
      }
    }
    digitalWrite(PIN_ERROR, LOW);
  }
  else {
    if (!opened) {
      opened = true;
      previous = millis();
      LOG("Opened");
    }
    else {
      if (millis() - previous > 10000) {
        digitalWrite(PIN_ERROR, HIGH);
      }
    }
  }
  delay(10);
}
