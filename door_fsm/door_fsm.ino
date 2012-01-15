
#define CONSOLE

#include <Arduino.h>
#include <FiniteStateMachine.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Console.h>

class Trigger {
  private:
    boolean value;

  public:
    Trigger() { }
    Trigger(boolean value) : value(value) { }
 
    virtual boolean update(FSM &fsm) {
      return value;
    }
};

class UserFunctionTrigger : public Trigger {
  private:
    boolean (*userUpdate)();
    
  public:
    UserFunctionTrigger(boolean (*userUpdate)()) : userUpdate(userUpdate) { }
 
    virtual boolean update(FSM &fsm) {
      return userUpdate();
    }
};

class TimeInCurrentStateTrigger : public Trigger {
  private:
    unsigned long time;
    
  public:
    TimeInCurrentStateTrigger(long time) : time(time) { }

    virtual boolean update(FSM &fsm) {
      return fsm.timeInCurrentState() >= time;
    }
};

class Transition {
  private:
    State &fromState;
    State &toState;
    Trigger &trigger;
    
  public:
    Transition(State &fromState, State &toState, Trigger &trigger) : fromState(fromState), toState(toState), trigger(trigger) { }
    
  public:
    boolean update(FSM &fsm) {
      if (&fsm.getCurrentState() != &fromState) {
        return false;
      }
      
      if (trigger.update(fsm)) {
        fsm.transitionTo(toState);
        return true;
      }
      return false;
    }
};

#define HOST        "192.168.0.127"
#define PIN_WHITE   8
#define PIN_GREEN   7
#define PIN_RED     6
#define PIN_SWITCH  9
#define PIN_MOTOR   5
#define RETRIES     3

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 5 };
byte gateway[] = { 192, 168, 0, 1 };
byte subnet[] = { 255, 255, 255, 0 };
byte dns[] = { 192, 168, 0, 1 };
static EthernetClient client;
long lastConnection = 0;

extern void waitingEnter();
extern void waitingUpdate();
extern void motorEnter();
extern void motorUpdate();
extern void motorLeave();
extern void doorOpenEnter();
extern void doorOpenUpdate();
extern void doorOpenLeave();
extern void doorClosedEnter();
extern void doorClosedUpdate();
extern void doorOpenLeave();
extern void errorEnter();
extern void errorUpdate();
extern boolean whenDoorClose();
extern boolean whenDoorOpen();

State waitingState(waitingEnter, waitingUpdate);
State motorState(motorEnter, motorUpdate, motorLeave);
State doorOpenState(doorOpenEnter, doorOpenUpdate, doorOpenLeave);
State doorClosedState(doorClosedEnter, doorClosedUpdate);
State errorState(errorEnter, errorUpdate);

UserFunctionTrigger doorOpenTrigger(whenDoorOpen);
UserFunctionTrigger doorCloseTrigger(whenDoorClose);
TimeInCurrentStateTrigger thirtySeconds(30000);
TimeInCurrentStateTrigger fiveSeconds(5000);
Trigger always(true);

#define NUMBER_OF_TRANSITIONS 8

Transition transitions[NUMBER_OF_TRANSITIONS] = {
  Transition(waitingState, doorOpenState, doorOpenTrigger),     // Waiting -> Open (when open)
  Transition(doorOpenState, doorClosedState, doorCloseTrigger), // Open -> Closed (when close)
  Transition(doorOpenState, errorState, thirtySeconds),         // Open -> Error (after 30s)
  Transition(doorClosedState, motorState, always),              // Closed -> Motor (always)
  Transition(motorState, waitingState, fiveSeconds),            // Motor -> Waiting (after 5s)
  Transition(motorState, doorOpenState, doorOpenTrigger),       // Motor -> Door Open (when open)
  Transition(errorState, waitingState, doorCloseTrigger),       // Error -> Waiting (when close)
  Transition(errorState, doorOpenState, doorOpenTrigger)        // Error -> Open (when open)
};

FSM fsm = FSM(waitingState);

void setup() {
  Serial.begin(9600);
  Serial.println("Started!");
  
  delay(1000);
  
  lastConnection = 0;
  
  Ethernet.begin(mac, ip, gateway, dns, subnet);

  pinMode(PIN_WHITE, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_MOTOR, OUTPUT);
  pinMode(PIN_SWITCH, INPUT);

  analogWrite(PIN_MOTOR, 0);
}

void loop() {
  for (int i = 0; i < NUMBER_OF_TRANSITIONS; ++i) {
    if (transitions[i].update(fsm)) {
      break;
    }
  }
  fsm.update();
  delay(10);
}

boolean whenDoorClose() {
  return digitalRead(PIN_SWITCH) == LOW;
}

boolean whenDoorOpen() {
  return !whenDoorClose();
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
    lastConnection = millis();
    return true;
  }
  
  else {
    LOG("POST Failed...");
    Serial.print("Uptime: ");
    Serial.println(millis());
    Serial.print("Last Connection: ");
    Serial.println(lastConnection);
    Ethernet.begin(mac, ip, gateway, dns, subnet);
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
      digitalWrite(PIN_RED, HIGH);
      delay(500);
      digitalWrite(PIN_RED, LOW);
    }
  }
}

void waitingEnter() {
  LOG("Waiting...");
}

void waitingUpdate() {
}

void doorOpenEnter() {
  LOG("Opened...");
  digitalWrite(PIN_GREEN, HIGH);
}

void doorOpenUpdate() {
}

void doorOpenLeave() {
  LOG("Closing...");
  LOG(fsm.timeInCurrentState());
  digitalWrite(PIN_GREEN, LOW);
}

void doorClosedEnter() {
  LOG("Closed...");
  
  if (fsm.timeInPreviousState() > 100 && fsm.timeInPreviousState() < 15000) {
    digitalWrite(PIN_WHITE, HIGH);
    notify(fsm.timeInPreviousState());
    digitalWrite(PIN_WHITE, LOW);
  }
}

void doorClosedUpdate() {
}

void errorEnter() {
  LOG("Error");
}

void errorUpdate() {
  digitalWrite(PIN_RED, HIGH);
  delay(100);
  digitalWrite(PIN_RED, LOW);
}

void motorEnter() {
  analogWrite(PIN_MOTOR, 200);
}

void motorUpdate() {
}

void motorLeave() {
  analogWrite(PIN_MOTOR, 0);
}
