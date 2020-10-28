////////////////////////////////////////////////////////////////////////////
////////////////Sensor Functions////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void updateNormalSensors() { //do a full sensor sweep
  Serial.println("//////////////updateNormalSensors//////////////");
  delay(1000);
  checkTime(); //already prints time string
  //readIMU();
  buildIMULog(); Serial.println("FOR BUILD IMU LOG, serial.prints inside are to be used!");
  IMUAve(); Serial.println("FOR IMU AVE, serial.prints inside are to be used!");
  checkBatteryVoltage(); Serial.print("Battery Voltage: "); Serial.println(MSH.Battery);
  currentSense(); Serial.print("Current Sensor: "); Serial.println(MSH.SolarCurrent);
  zeroCurrent(); Serial.print("Current Averager (does it work? It might need ten readings before it makes sense!): "); Serial.println(MSH.CurrentZero );
  lightSense(); Serial.print("Light Sensor: "); Serial.println(MSH.lightSense);
  doorSense(); Serial.print("Door Button: "); Serial.println(MSH.DoorButton);
  checkTemp(); Serial.print("Temperature: "); Serial.println(MSH.Temp); Serial.println("Temperature in Celsius is printed above.");
  InhibitorsCheck(); Serial.print("Inhibitor 1: "); Serial.print(MSH.Inhibitor_19a); Serial.print("; Inhibitor 2: "); Serial.print(MSH.Inhibitor_10b); Serial.print("; Inhibitor 3: "); Serial.print(MSH.Inhibitor_2); Serial.print("; FREEHUB: "); Serial.println(MSH.FREEHUB);
}

void updateLowSensors() {
  Serial.println("updateLowSensors");
  checkTime();
  //readIMU();
  buildIMULog(); //do we need this? for Low Power
  IMUAve(); //do we need this? for Low Power
  checkBatteryVoltage();
  currentSense();
  zeroCurrent();
  lightSense();
  doorSense(); //do we need this? for Low Power
  checkTemp();
  InhibitorsCheck(); //do we need this? for Low Power

}

void checkTime() { //todo
  long timeNow = millis();
  String timeString = "";

  int days = timeNow / day ;//number of days
  int hours = (timeNow % day) / hour;
  int minutes = ((timeNow % day) % hour) / minute ;//and so on...
  int seconds = (((timeNow % day) % hour) % minute) / second;

  // digital clock display of current time
  //  Serial.print(days, DEC);
  //  printDigits(hours);
  //  printDigits(minutes);
  //  printDigits(seconds);
  //  Serial.println();

  timeString += String(days, DEC) + ":";
  timeString += String(hours, DEC) + ":";
  timeString += String(minutes, DEC) + ":";
  timeString += String(seconds, DEC);

  Serial.println(timeString);

}

void printDigits(byte digits) {
  // utility function for digital clock display: prints colon and leading 0
  Serial.print(":");
  if (digits < 10) {
    Serial.print('0');
    Serial.print(digits, DEC);
  }
}

void readIMU() { //theres no elegant way to do this ;_;
  lsm.read();
  sensors_event_t a, m, g, temp;
  lsm.getEvent(&a, &m, &g, &temp);

  //Guide to mapping: IMU has 4 settings to accel and mag. 3 settings for gyro. Setting is setup on pop commands as well as initial conditions!
  if (m.acceleration.x > 2 || m.acceleration.y > 2 || m.acceleration.z > 2) {
    MSH.ACC = 4;
    IMURange(1, 2);
    //Serial.println("ACC 4");
  }
  if (m.acceleration.x > 4 || m.acceleration.y > 4 || m.acceleration.z > 4) {
    MSH.ACC = 8;
    IMURange(1, 3);
    //Serial.println("ACC 8");
  }
  if (m.acceleration.x > 8 || m.acceleration.y > 8 || m.acceleration.z > 8) {
    MSH.ACC = 16;
    IMURange(1, 4);
    //Serial.println("ACC 16");
  } else {
    MSH.ACC = 2;
    IMURange(1, 1);
    //Serial.println("ACC 2");
  }

  if (m.magnetic.x > 4 || m.magnetic.y > 4 || m.magnetic.z > 4) {
    MSH.MAG = 8;
    IMURange(2, 2);
    //Serial.println("MAG 8");
  }
  if (m.magnetic.x > 8 || m.magnetic.y > 8 || m.magnetic.z > 8) {
    MSH.MAG = 12;
    IMURange(2, 3);
    //Serial.println("MAG 12");
  }
  if (m.magnetic.x > 12 || m.magnetic.y > 12 || m.magnetic.z > 12) {
    MSH.MAG = 16;
    IMURange(2, 4);
    //Serial.println("MAG 16");
  } else {
    MSH.MAG = 4;
    IMURange(2, 1);
    //Serial.println("MAG 4");
  }

  if (m.gyro.x > 245 || m.gyro.y > 245 || m.gyro.z > 245) {
    MSH.GYR = 500;
    IMURange(3, 2);
    //Serial.println("GYR 500");
  }
  if (m.gyro.x > 500 || m.gyro.y > 500 || m.gyro.z > 500) {
    MSH.GYR = 2000;
    IMURange(3, 3);
    //Serial.println("GYR 2000");
  } else {
    MSH.GYR = 245;
    IMURange(3, 1);
    //Serial.println("GYR 245");
  }

  lsm.read();
  //sensors_event_t a, m, g, temp;
  lsm.getEvent(&a, &m, &g, &temp);

  MSH.Mag[0] = map(m.magnetic.x, -MSH.MAG, MSH.MAG, 0, 255) ;
  MSH.Mag[1] = map(m.magnetic.y, -MSH.MAG, MSH.MAG, 0, 255);
  MSH.Mag[2] = map(m.magnetic.z, -MSH.MAG, MSH.MAG, 0, 255);
  MSH.Gyro[0] = map(g.gyro.x, -MSH.GYR, MSH.GYR, 0, 255);
  MSH.Gyro[1] = map(g.gyro.y, -MSH.GYR, MSH.GYR, 0, 255);
  MSH.Gyro[2] = map(g.gyro.z, -MSH.GYR, MSH.GYR, 0, 255);
  MSH.Accel[0] = map(a.acceleration.x, -MSH.ACC, MSH.ACC, 0, 255);
  MSH.Accel[1] = map(a.acceleration.y, -MSH.ACC, MSH.ACC, 0, 255);
  MSH.Accel[2] = map(a.acceleration.z, -MSH.ACC, MSH.ACC, 0, 255);
  MSH.ImuTemp = map(lsm.temperature, -1024, 1024, 0, 255); //this must tested. we do not currently know what scale the imu uses for temperature, neither the unit (C/K/F)
  //     Serial.println("Data in uint8 (mapped):");
  //     Serial.print(MSH.Mag[0]); Serial.print(" "); Serial.print(MSH.Mag[1]); Serial.print(" "); Serial.println(MSH.Mag[2]);
  //     Serial.print(MSH.Gyro[0]); Serial.print(" "); Serial.print(MSH.Gyro[1]); Serial.print(" "); Serial.println(MSH.Gyro[2]);
  //     Serial.print(MSH.Accel[0]); Serial.print(" "); Serial.print(MSH.Accel[1]); Serial.print(" "); Serial.println(MSH.Accel[2]);
  //     Serial.println(MSH.ImuTemp);
  //     Serial.println("Original data:");
  //     Serial.print(m.magnetic.x); Serial.print(" "); Serial.print(m.magnetic.y); Serial.print(" "); Serial.println(m.magnetic.z);
  //     Serial.print(g.gyro.x); Serial.print(" "); Serial.print(g.gyro.y); Serial.print(" "); Serial.println(g.gyro.z);
  //     Serial.print(a.acceleration.x); Serial.print(" "); Serial.print(a.acceleration.y); Serial.print(" "); Serial.println(a.acceleration.z);
  //     Serial.println(lsm.temperature);
  //     Serial.println("Data from uint8_t back to float");

  //    float magx = map(float(MSH.Mag[0]), 0, 255, -MSH.MAG, MSH.MAG);
  //    float magy = map(float(MSH.Mag[1]), 0, 255, -MSH.MAG, MSH.MAG);
  //    float magz = map(float(MSH.Mag[2]), 0, 255, -MSH.MAG, MSH.MAG);
  //    float gyrox = map(float(MSH.Gyro[0]), 0, 255, -MSH.GYR, MSH.GYR);
  //    float gyroy = map(float(MSH.Gyro[1]), 0, 255, -MSH.GYR, MSH.GYR);
  //    float gyroz = map(float(MSH.Gyro[2]), 0 , 255, -MSH.GYR, MSH.GYR);
  //    float accelx = map(float(MSH.Accel[0]), 0, 255, -MSH.ACC, MSH.ACC);
  //    float accely = map(float(MSH.Accel[1]), 0, 255, -MSH.ACC, MSH.ACC);
  //    float accelz = map(float(MSH.Accel[2]), 0, 255, -MSH.ACC, MSH.ACC);

  //     Serial.print(magx); Serial.print(" "); Serial.print(magy); Serial.print(" "); Serial.println(magz);
  //     Serial.print(gyrox); Serial.print(" "); Serial.print(gyroy); Serial.print(" "); Serial.println(gyroz);
  //     Serial.print(accelx); Serial.print(" "); Serial.print(accely); Serial.print(" "); Serial.println(accelz);



  //    MSH.Mag[0] = ((long int)lsm.magData.x);
  //    MSH.Mag[1] = ((long int)lsm.magData.y);
  //    MSH.Mag[2] = ((long int)lsm.magData.z);
  //    MSH.Gyro[0] = ((long int)lsm.gyroData.x);
  //    MSH.Gyro[1] = ((long int)lsm.gyroData.y);
  //    MSH.Gyro[2] = ((long int)lsm.gyroData.z);
  //    MSH.Accel[0] = ((long int)lsm.accelData.x);
  //    MSH.Accel[1] = ((long int)lsm.accelData.x);
  //    MSH.Accel[2] = ((long int)lsm.accelData.x);
  //    MSH.ImuTemp = ((int)lsm.temperature);
}

void buildIMULog() {
  for (int SenseCounter = 0; SenseCounter <= SenseNumb; SenseCounter++) {

    for (int j = 0; j < 3; j++) {
      readIMU(); //read values to store them in
      MSH.MagLog[j][SenseCounter] = MSH.Mag[j];
      MSH.GyroLog[j][SenseCounter] = MSH.Gyro[j];
      MSH.AccelLog[j][SenseCounter] = MSH.Accel[j];
    }
    SenseCounter++;
  }
}

void IMUAve() { //should we also do the median?
  for (int j = 0; j < 3; j++) { // j=0 -> x, 1 -> y, 2 -> z axis!
    for (int i = SenseNumb; i > 0; i--) {
      //Serial.print("SenseNumb: ");Serial.println(i);
      MSH.MagAve[j] += MSH.MagLog[j][i - 1];
      MSH.GyroAve[j] += MSH.GyroLog[j][i - 1];
      MSH.AccelAve[j] += MSH.AccelLog[j][i - 1];
    }//close for i
    MSH.MagAve[j] = (MSH.MagAve[j]) / 10;
    MSH.GyroAve[j] = (MSH.GyroAve[j]) / 10;
    MSH.AccelAve[j] = (MSH.AccelAve[j]) / 10;

  }//close for j

  // very helpful for debugging
  //     Serial.print(MSH.GyroLog[0][0]); Serial.print(" "); Serial.print(MSH.GyroLog[1][0]); Serial.print(" "); Serial.println(MSH.GyroLog[2][0]) ;
  //     Serial.print(MSH.GyroLog[0][1]); Serial.print(" "); Serial.print(MSH.GyroLog[1][1]); Serial.print(" "); Serial.println(MSH.GyroLog[2][1]) ;
  //     Serial.print(MSH.GyroLog[0][2]); Serial.print(" "); Serial.print(MSH.GyroLog[1][2]); Serial.print(" "); Serial.println(MSH.GyroLog[2][2]) ;
  //     Serial.print(MSH.GyroLog[0][3]); Serial.print(" "); Serial.print(MSH.GyroLog[1][3]); Serial.print(" "); Serial.println(MSH.GyroLog[2][3]) ;
  //     Serial.print(MSH.GyroLog[0][4]); Serial.print(" "); Serial.print(MSH.GyroLog[1][4]); Serial.print(" "); Serial.println(MSH.GyroLog[2][4]) ;
  //     Serial.print(MSH.GyroLog[0][5]); Serial.print(" "); Serial.print(MSH.GyroLog[1][5]); Serial.print(" "); Serial.println(MSH.GyroLog[2][5]) ;
  //     Serial.print(MSH.GyroLog[0][6]); Serial.print(" "); Serial.print(MSH.GyroLog[1][6]); Serial.print(" "); Serial.println(MSH.GyroLog[2][6]) ;
  //     Serial.print(MSH.GyroLog[0][7]); Serial.print(" "); Serial.print(MSH.GyroLog[1][7]); Serial.print(" "); Serial.println(MSH.GyroLog[2][7]) ;
  //     Serial.print(MSH.GyroLog[0][8]); Serial.print(" "); Serial.print(MSH.GyroLog[1][8]); Serial.print(" "); Serial.println(MSH.GyroLog[2][8]) ;
  //     Serial.print(MSH.GyroLog[0][9]); Serial.print(" "); Serial.print(MSH.GyroLog[1][9]); Serial.print(" "); Serial.println(MSH.GyroLog[2][9]) ;
  //     Serial.print("Gyro Average: "); Serial.print(MSH.GyroAve[0]); Serial.print(" "); Serial.print(MSH.GyroAve[1]); Serial.print(" "); Serial.println(MSH.GyroAve[2]) ;
  //delay(5000);
}

void checkBatteryVoltage() {//TODO determine battery range Map to true values
  MSH.Battery = map(analogRead(32), 0, 1023, 0, 255);
  //MSH.Battery = map(MSH.Battery, 0, 1023, 0, 4.2); //->check this mapping see if it can read the top //better do this on ground by ourselves
}

void checkTemp() {
  Serial.print("Printing Temp Sensor Data: ");
  MSH.Temp = map(analogRead(16), 0, 1023, 0, 255); //calibrate top voltage of Tempsensor, until then values are off
  Serial.print(MSH.Temp); Serial.println(", ");
  int reading = analogRead(16);
  Serial.print("Reading:"); Serial.println(reading);
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 3.3;
  voltage /= 1024.0;

  // print out the voltage
  Serial.print(voltage); Serial.println(" volts");

  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  Serial.print(temperatureC); Serial.println(" degrees C");
  //MSH.Temp = map(MSH.Temp,0,255,-40,150); //PS: accuracy diminshes greatly past 125 Celsius, sensor limitation, no fix.
  //Serial.println(MSH.Temp);
}

void currentSense() {// this value will need to be zeroed before practical use, also needs the meth
  float sensorValue = analogRead(15);              // Analog Signal Value
  float volt;                     // Voltage after conversion from analog signal
  float amp;

  volt = (sensorValue * MSH.VOLTAGE_REF) / 1023;
  // Follow the equation given by the INA169 datasheet to
  // determine the current flowing through RS.
  // Reason for this is to obtain 50-100mV per INA169 specifications.
  // Is = (Vout x 1k) / (RS x RL)
  // 20 mA of approximate error error
  amp = volt / (29.75 * MSH.RS);
  //    Serial.print("Calculated Amperage: "); Serial.println(amp);
  MSH.SolarCurrent = map(sensorValue, 0, 1024, 0, 255);
  //    Serial.print("Calculated uint8_t SolarCurrent: "); Serial.println(MSH.SolarCurrent);
  //    Serial.print("Calculated analog: "); Serial.println(sensorValue);

  //test this please, and sign off!
}

void zeroCurrent() { // If averaging is okay, just check if it works well. Also verify what units this value has since it must fit the uin8_t format
  int accumulator = 0;
  for (int i = 0; i < 10; i++) {
    currentSense();
    accumulator += MSH.SolarCurrent;
  }
  MSH.CurrentZero = (accumulator / 10);
}

void lightSense() { //resistance goes down with more light -> 2.5V when light out at least with 0V (or close) when its dark
  //
  MSH.lightSense = map(analogRead(17), 0, 1023, 0, 255);
  //MSH.lightSense = map(MSH.lightSense, 0, 1023, 0, 3.3);
  //if value above a threshold, say 600, change boolean to positive sunlight. Or maybe just send back the number. r/ANd, add to requirements for mission sucess from CueSat perspective
}

void doorSense() {
  MSH.DoorButton = map(digitalRead(13), 0, 1023, 0, 255); //done, its a simple HIGH or LOW-> confirm which means door is closed/open
}
/*
 //this function works when it was all in one file, but it's unused anyway
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
*/
void sensorInit() { //TODO Ensure that every sensor input is declared an input here // Relevant for all sensors: 3.3/1023 = .00322 -> discrete values //verfy pins are correct!

  //SDInit();
  InitializeSdCam();
  pinMode(17, INPUT); //verify its photoresistor
  pinMode(16, INPUT); //verify its for Temp
  pinMode(35, OUTPUT); //for burn wire, the door trig
  pinMode(38, INPUT); //verify its for first inhibitor
  pinMode(14, INPUT); //verify its for second inhibitor
  pinMode(39, INPUT); //verify its for third inhibitor
  pinMode(15, INPUT); //verify its currentsensor
  pinMode(32, INPUT); //Battery
  pinMode(28, INPUT); //EXTRA
  pinMode(24, OUTPUT); //LOW_POWER_TRIG
  pinMode(31, INPUT); //Hatch Button
  //STBD 1 missing
  //STBY 2 missing
  //X1
  //X2
  //Y1
  //Y2
  //Z1
  //Z2
  //missing temp sensor //for the Temp sensor, the output range goes from 0V to 1.75V
  IMUInit();
  //RBF is physical
}
