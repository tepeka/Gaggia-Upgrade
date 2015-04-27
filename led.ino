int val = 0;
int pin;
boolean up = true;

void ledInit(int p) {
  pin = p;
  pinMode(pin, OUTPUT);
}

void ledPulseStep() {
  if (up) {
    val = val + 10;
  } else {
    val = val - 10;
  }
  if (val > 255) {
    val = 255;
    up = false;
  }
  if (val < 0) {
    val = 0;
    up = true;
  }

  analogWrite(pin, val);
  //if(val > 0)
  //  delay(20);
  //else
  //  delay(200);
}

void ledOn() {
  analogWrite(pin, 255);
}

void ledOff() {
  analogWrite(pin, 0);
}
