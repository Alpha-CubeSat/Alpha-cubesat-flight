void IMURange(int sensor, int sensitivity) { // TODO double check that I work //function to be used by command
  switch (sensor) {
    case (1):
      //accelerometer
      switch (sensitivity) {
        case (1):
          lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
          break;
        case (2):
          lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_4G);
          break;
        case (3):
          lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_8G);
          break;
        case (4):
          lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
          break;
      }
      break;
    case (2):
      //magnetometer
      switch (sensitivity) {
        case (1):
          lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
          break;
        case (2):
          lsm.setupMag(lsm.LSM9DS1_MAGGAIN_8GAUSS);
          break;
        case (3):
          lsm.setupMag(lsm.LSM9DS1_MAGGAIN_12GAUSS);
          break;
        case (4):
          lsm.setupMag(lsm.LSM9DS1_MAGGAIN_16GAUSS);

      }
      break;
    case (3):
      //gyroscope
      switch (sensitivity) {
        case (1):
          lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
          break;
        case (2):
          lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_500DPS);
          break;
        case (3):
          lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_2000DPS);
          break;
      }
      break;
  }
}

void setupIMUSensor() {
  // 1.) Set the accelerometer range
  if (MSH.ACC == 2) {
    lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  } if (MSH.ACC == 4) {
    lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_4G);
  } if (MSH.ACC == 8) {
    lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_8G);
  } else {
    lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  }

  // 2.) Set the magnetometer sensitivity
  if (MSH.ACC == 4) {
    lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  } if (MSH.ACC == 8) {
    lsm.setupMag(lsm.LSM9DS1_MAGGAIN_8GAUSS);
  } if (MSH.ACC == 12) {
    lsm.setupMag(lsm.LSM9DS1_MAGGAIN_12GAUSS);
  } else {
    lsm.setupMag(lsm.LSM9DS1_MAGGAIN_16GAUSS);
  }

  // 3.) Setup the gyroscope
  if (MSH.GYR == 245) {
    lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
  } if (MSH.GYR == 500) {
    lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_500DPS);
  } else {
    lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_2000DPS);
  }
}

void IMUInit() {
  lsm.begin();
  if (!lsm.begin())
  {
    //imu startup failure. trip fault
    MSH.IMUActive = false;
    return;
  }
  MSH.IMUActive = true;
  IMURange(1, 1); //for first measurement
  IMURange(2, 1);
  IMURange(3, 1);
}
