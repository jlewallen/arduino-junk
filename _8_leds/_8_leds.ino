const int SER_Pin = 8;    // pin 14 on the 75HC595
const int RCLK_Pin = 9;   // pin 12 on the 75HC595
const int SRCLK_Pin = 10; // pin 11 on the 75HC595

#define number_of_74hc595s 1 
#define numOfRegisterPins number_of_74hc595s * 8
#define number_of_frames 33

boolean frames[number_of_frames][numOfRegisterPins] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 1 },
  { 0, 1, 0, 0, 0, 0, 1, 0 },
  { 0, 0, 1, 0, 0, 1, 0, 0 },
  { 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 0,10, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 1 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  // 10
  { 1, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 1 },
  { 0, 0, 0, 0, 0, 0, 0, 1 },
  { 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 0, 0 },
  // 18
  { 1, 1, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 1, 1, 1 },
  // 20
  { 0, 0, 0, 0, 0, 0, 0, 0 },  
  { 0, 0, 0, 1, 1, 0, 0, 0 },  
  { 0, 0, 1, 1, 1, 1, 0, 0 },  
  { 0, 1, 1, 1, 1, 1, 1, 0 },  
  { 1, 1, 1, 1, 1, 1, 1, 1 },  
  // 25
};

boolean *registers = NULL;

void setup() {
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);
  registers = frames[0];
  clearRegisters();
  writeRegisters();
}               

void clearRegisters() {
  for(int i = numOfRegisterPins - 1; i >=  0; i--) {
     registers[i] = LOW;
  }
} 

void writeRegisters() {
  digitalWrite(RCLK_Pin, LOW);
  for(int i = numOfRegisterPins - 1; i >= 0; i--) {
    digitalWrite(SRCLK_Pin, LOW);
    digitalWrite(SER_Pin, registers[i]);
    digitalWrite(SRCLK_Pin, HIGH);
  }
  digitalWrite(RCLK_Pin, HIGH);
}

void setRegisterPin(int index, int value) {
  registers[index] = value;
}

int state = 0;

void loop() {
  registers = frames[state];
  /*
  setRegisterPin(0, HIGH);
  setRegisterPin(1, HIGH);
  setRegisterPin(2, HIGH);
  setRegisterPin(3, HIGH);
  setRegisterPin(4, HIGH);
  setRegisterPin(5, HIGH);
  setRegisterPin(6, HIGH);
  setRegisterPin(7, HIGH);
  */
  writeRegisters();
  delay(75);
  state++;
  state = state % number_of_frames;
}
