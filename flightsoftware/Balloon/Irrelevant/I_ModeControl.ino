void ModeSwitch() { // to do plan out the switching mechanisms //switches to lowPower once voltage gets low enough that powerman can't solve it
  uint8_t i = MSH.Battery;
  if ((MSH.OVERWRITE) && (i > 924)) { //only works when OVERWRITE = true, and we are above safety voltage (3.7V for now) //should we include the voltage safety here?
    //OVERWRITE IS TRUE //U BETTA KNOW WHATCHU DUIN
    // int MSH.overmode = ; //mode user order from popcomands

    switch (MSH.overmode) {
      case (0):
        nextMode = 0; //MNormal
        break;  //we need the break?

      case (1):
        nextMode = 1; //detumble
        break;  //we need the break?

      case (2):
        nextMode = 2; //MSafeHold
        break;  //we need the break?

      case (4):
        nextMode = 4; //eclipse // now just MLowPower
        break;  //we need the break?

      case (5):
        nextMode = 5; //MDeploy
        break;  //we need the break?
    }

  } else { //only used if overwrite is off //should we turn it off after a while? Say a week?
    //joao's comment is asking if we should have a timer for the overwrite mode. discussed, should probably
    // be based on battery voltage rather than timer

    //How to get into NORMAL MODE
    //if voltage is above 3.2
    //if current/power is above a certain threshold-> not recommended. better to stick with voltage
    //will not transition to detumble
    //if a piture is being taken, will drop voltage threshold to 3.0 to complete action, and then it raises back to 3.2
    if (i > 178) { //always gets in this if it has enough power // is this needed
      Serial.println("Setting nextMode == 0");
      nextMode = 0; //MNormal //important to have it biased towards normal operations
    }

    //How to get into DETUMBLE MODE
    //Functions for inhibitors were activated by change -> recognizes it has been released
    //Or IMU numbers have destabilized
    //Or by expressive order
    float g = .2;
    if (MSH.SolarCurrent >= (MSH.CurrentZero + g)) {//todo determine true g //use current sensor to determine when to use ACS
      //nextMode = 4; //eclipse // now just MLowPower //handled by powerman
    }

    //How to get into MSAFEHOLD MODE
    //Fault tripped
    if (faultTripped == true) { //has a fault been tripped
      //nextMode = 2; //MSafeHold
    }

    //How to get into LOWPOWER MODE
    //If voltage drops below 3.2
    //if current/power is above a certain threshold -> not recommended

    if (false) { //spinning too fast on any axis //current sensor value here!
      //nextMode = 1; //detumble
    }

    //How to get into DEPLOY MODE
    //Two keys (booleans) transition by manual order
    if (DLKey1 && DLKey2) { //we need to send a command for this one to activate. We don;t want it running the OP by itself without us noticing. (probably DLKey1&2)
      nextMode = 5; //MDeploy
    }
  }
}


void modeCon() { //does not need overwrite mod //must be called specifically
  z = millis();
  //  faultCheck(); activate when fault check is ready

  switch (nextMode) {

    case 0:
      // normal ops
      MNormal();
      break;

    case 1:
      // detumble
      MDetumble();
      break;

    case 2:
      // safe hold
      MSafeHold();
      break;

    //    case 3:
    //      MEclipse(); // to erase when sure
    //      break;

    case 4:
      MLowPower();
      MSH.LowPowerTimer = millis();
      break;

    case 5:
      MDeploy();
      break;
  }
  // use to check cycle speed
  //  Serial.println("////////////////////////////////////////");
  //  Serial.print("cycle completed in ");
  //  Serial.print(millis() - z);
  //  Serial.println(" milliseconds");
  //  Serial.println("////////////////////////////////////////");


}

void MNormal() { //each mode should have a set of initial conditions in case we need to enforce them //perhaps, we should have presets, but the normal operations are run in a more fluid way?
  //Write presets for normla operations //Basically, reset parameters for RB and read sensors
  Serial.println("MNORMAL Starting!");
  updateNormalSensors();
}

void MDetumble() { //ACS
  //Serial.println ("yay im detumbling");
  //ACS Code Here
  if (permission == 1) { //we have to integrate this permision thing with setup/first loops

    sensors_event_t a, m, g, temp;
    lsm.getEvent(&a, &m, &g, &temp);

    rtObj.rtU.angularvelocity[0] = g.gyro.x * 3.14159 / 180.0; //these must be changed to accomodate the new IMU model
    rtObj.rtU.angularvelocity[1] = g.gyro.y * 3.14159 / 180.0;
    rtObj.rtU.angularvelocity[2] = g.gyro.z * 3.14159 / 180.0;
    rtObj.rtU.Bfield_body[0] = m.magnetic.x;
    rtObj.rtU.Bfield_body[1] = m.magnetic.y;
    rtObj.rtU.Bfield_body[2] = m.magnetic.z;
    rtObj.step();
    count = count + 1;
    //rtObj.rtY.detuble;
    //rtObj.rtY.point;

    //double check with davide on what the mode numbers mean
    // understand the permission = 1 thing. permission = 0 at end, so what makes permission =1;
    if (ACSmode == 0) { //detumble //i have to create a mechanis for this.
      current1 = rtObj.rtY.detuble[0];
      current2 = rtObj.rtY.detuble[1];
      current3 = rtObj.rtY.detuble[2];
    }
    if (ACSmode == 1) { //normal acs operations
      current1 = rtObj.rtY.point[0];
      current2 = rtObj.rtY.point[1];
      current3 = rtObj.rtY.point[2];
    }

    Serial.println("Run Count");
    Serial.println(count);
    Serial.println("Angular Velocity p");
    Serial.println(rtObj.rtU.angularvelocity[0]);
    Serial.println("Angular Velocity q");
    Serial.println(rtObj.rtU.angularvelocity[1]);
    Serial.println("Angular Velocity r");
    Serial.println(rtObj.rtU.angularvelocity[2]);
    Serial.println("Bfield_body");
    Serial.println(rtObj.rtU.Bfield_body[0]);
    Serial.println(rtObj.rtU.Bfield_body[1]);
    Serial.println(rtObj.rtU.Bfield_body[2]);
    permission = 0;
  }

  Serial.println("I have left MDeTumble");

  nextMode = 0; //is this the right place? Don't we have to loop this?
}

void MSafeHold() {
  Serial.println("im safe... for now");
  //Change booleans for usage by modeCon, and also run specific trnamisison to identify fault
  //Here must be code that is able to recieve commands to ignore a certain fault
}

void MLowPower() {
  //Serial.println("low power");
  //change rockbloc frequency //but already done with powerman? //Have PowerMan call MOdeSwitch to set to this stage, which then houses new parameters. This mode is runseveral times through loops,
  MSH.uplinkPeriod = 14400000; //4 hours to charge //subjected to change from mission rehearsals
  MSH.downlinkPeriod = 14400000; //these two must be switched back!
  checkBatteryVoltage(); //check voltage specifically!
  //if statement: we must charge, so only use the next number if we have waited either four hours of no operations, or the battery is back to over 3.9
  updateLowSensors();
  if ((millis() - MSH.LowPowerTimer) >= 14400000 ||  MSH.Battery > 240) { //random integer, needs mission rehearsal
    nextMode = 0; //back to work baby!
  }
  //always checking and making adjustments

}

void MDeploy() { //mission related. receives th two keys and executes the commands i the right order. Once data is taken, it analyzes and sends a check to us, as well as the data it used to confirm the mission's success
  int g; //todo determine proper value for g also check door sense logic

  if (deployStage == 0) {
    digitalWrite(35, HIGH); //todo has the burnwire pin changed
    burnTimer = millis();
    deployStage = 1;
  }

  if ((deployStage == 1) && (MSH.lightSense >= g) && (MSH.DoorButton = 1)) { //confirm that this is VERY consistent

    deployStage = 2;
    digitalWrite(35, LOW); //todo has the burnwire pin changed
    MSH.burstStart = millis();
  }

  if (deployStage == 2) {

    if ((MSH.BurstDuration + MSH.burstStart) <= millis()) {
      //success!!!
      deployStage = 0;
      DLKey1 = false;
      DLKey2 = false;
    } else {
      TakePicture();
    }
  }

  if (((burnTimer + 45000) <= millis()) && deployStage == 1) {
    digitalWrite(35, LOW); //todo has the burnwire pin changed
    //burnwire fail fault
    DLKey1 = false;
    DLKey2 = false;
    deployStage = 0;
  }

}


void InhibitorsCheck() { //read pin, and change value //please check PINs they are all wrong
  MSH.Inhibitor_19a = digitalRead(39);
  MSH.Inhibitor_10b = digitalRead(14);
  MSH.Inhibitor_2 = digitalRead(38);

  if (MSH.Inhibitor_19a == false && MSH.Inhibitor_10b == false && MSH.Inhibitor_2 == false) {
    MSH.FREEHUB = true; //CUBESAT is out of the P-POD! YAY!
  }
}

void GRENNLIGHT() { //This function is used to specifically detect if a manual GREENLIGHT for mission operation has been given (sail release)
  //This is normal function which may be overwritten to ignore rest of conditions
  //ends up switching mode to MDEPLOY
  if (true) { //conditions:
    //two keys have been engaged
    //detumble has been switched out - >manual command for this
    //HUB is FREE




    MDeploy();
  }
}
