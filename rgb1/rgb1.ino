class Glow {
private:
	byte pinR;
	byte pinG;
	byte pinB;
	int16_t values[3];
	int16_t deltas[3];
	long previousChange;
  word duration;

public:
	Glow(byte pinR, byte pinG, byte pinB) : pinR(pinR), pinG(pinG), pinB(pinB), previousChange(0), duration(2000) {
		values[0] = 255;
		values[1] = 255;
		values[2] = 255;
	}

	void begin() {
		pinMode(pinR, OUTPUT);
		pinMode(pinG, OUTPUT);
		pinMode(pinB, OUTPUT);
	}

	void tick() {
		analogWrite(pinR, values[0]);
		analogWrite(pinG, values[1]);
		analogWrite(pinB, values[2]);

		if (millis() - previousChange > duration) {
			for (byte i = 0; i < 3; ++i) {
				deltas[i] = random(0, 6) - 3;
			}
			duration = random(2000, 5000);
			previousChange = millis();
		}
		else {
			for (byte i = 0; i < 3; ++i) {
				values[i] += deltas[i];
				if (values[i] < 0 || values[i] > 255) {
					values[i] = constrain(values[i], 0, 255);
					deltas[i] = -deltas[i];
				}
			}
		}
	}
};

Glow glow1(9, 10, 11);
Glow glow2(3, 5, 6);

void setup() {
	glow1.begin();
	glow2.begin();
}

void loop() {
	glow1.tick();
	glow2.tick();
	delay(10);
}
