void ACSsetup() {
  Serial.println("STARTING ACS SETUP");
  Timer1.initialize(10000);
  Timer1.attachInterrupt(myISR);
  //LED Pins

  //H-bridge standby pin
  pinMode(STBYpin, OUTPUT);
  pinMode(STBZpin, OUTPUT);

  //Pins for X-Torquer
  pinMode(xPWMpin, OUTPUT);
  pinMode(xout1, OUTPUT);
  pinMode(xout2, OUTPUT);

  //Pins for Y-Torquer
  pinMode(yPWMpin, OUTPUT);
  pinMode(yout1, OUTPUT);
  pinMode(yout2, OUTPUT);

  //Pins for Z-Torquer
  pinMode(zout1, OUTPUT);
  pinMode(zout2, OUTPUT);
  pinMode(zPWMpin, OUTPUT);

  rtObj.initialize();
  //  Serial.println("Setting up IMU");
  //  setupIMU();
  //  Serial.println("IMU SETUP COMPLETE");
  //  Serial.println("SETUP COMPLETE");
}

void myISR(void) {
  if (pinset == HIGH) {
    pinset = LOW;
  }
  else {
    pinset = HIGH;
  }
  permission = 1;
}


void ACSwrite(int torqorder,  float current,  int out1,  int out2, int PWMpin) {
  if (current == 0.0) {
    digitalWrite(out1, LOW);
    digitalWrite(out2, LOW);
  }

  if (torqorder == 0) {
    if (current > 0) {
      digitalWrite(out1, HIGH);
      digitalWrite(out2, LOW);
      analogWrite(PWMpin, current);
    }
    if (current < 0) {
      digitalWrite(out1, LOW);
      digitalWrite(out2, HIGH);
      analogWrite(PWMpin, current);
    }
  }
  if (torqorder == 1) {
    if (current > 0) {
      digitalWrite(out1, LOW);
      digitalWrite(out2, HIGH);
      analogWrite(PWMpin, current);
    }
    if (current < 0) {
      digitalWrite(out1, HIGH);
      digitalWrite(out2, LOW);
      analogWrite(PWMpin, current);
    }
  }

}
