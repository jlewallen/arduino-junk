#include <SPI.h>
#include <Ethernet.h>
#include <HttpRequest.h>

static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static byte ip[] = { 10, 0, 0, 2 };
static byte gateway[] = { 10, 0, 0, 1 };
static byte subnet[] = { 255, 255, 0, 0 };
static EthernetServer server(80);

class MyWebRequest : public HttpRequest {
  public:
    MyWebRequest(EthernetClient &c) : HttpRequest(c) { }

  public:
    virtual void respond(const char *url) {
      respondWithJsonHeaders();
      const char *leds[] = { "l0", "l1", "l2" };
      for (int l = 0; l < 3; ++l) {
        const char *v = parameter(leds[l]);
        if (v != NULL) {
          digitalWrite(7 + l, atoi(v));
        }
      }
    }
};

void setup() {
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    MyWebRequest active = MyWebRequest(client);
    active.handle();
  }
}

