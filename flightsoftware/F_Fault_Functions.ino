void faultCheck() { // todo make this track which fault was tripped
  // reads fault string and checks that no threshold has been tripped
  // todo if a sensor is off be sure you gave it enough time to init
  int f = 0;
  bool fault = false;

  if (!faultTripped) {
    ReportOpenSpot = 0;
  }


  while (f < faultSize) {

    int CurInt = MSH.faultReport[f];

    if (CurInt = 0) {
      //ignore
    } else {

      switch (f) { //f++ after switch, still inside while //cases can neverthless be added

        case (1):
          if (DLFailCounter > 3) {
            Serial.println("case 1 tripped");
            fault = true;
            DLFailCounter = 0;
          }
          break;

        case (2):
          //Serial.println("case 2 tripped");
          break;

        case (3):
          // camera active?
          if (!MSH.CamActive) {
            MSH.faultReport[ReportOpenSpot + 1]  = MSH.CamActive;
            fault = true;
          }
          break;

        case (4):
          // SD card active?
          //if (SlaveCompFlag) { not needed anymore
          if (MSH.SDActive) {
            break;
          } else {
            MSH.faultReport[ReportOpenSpot + 1]  = MSH.SDActive;
            fault = true;
          }

          break;


        case (6):
          if (!MSH.IMUActive) {
            MSH.faultReport[ReportOpenSpot + 1]  = MSH.IMUActive;
            fault = true;
          }
          break;

        case (7):
          if (!MSH.RBActive) {
            MSH.faultReport[ReportOpenSpot + 1]  = MSH.RBActive;
            fault = true;
          }
          break;
        case (8): //IMU Magnetorquer x off
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (9): //IMU Magnetorquer y off
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (10): //IMU Magnetorquer z off
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (11): //Battery reading off
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (12): //Solar Panel Voltage Reading off (?or maybe other issues, need to work with them to notice possible problems))
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (13): //Current Sensor is off
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (14): //Rockblock signal quality is zero for too long
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (15): //Rockblock has failed to downlink for more than 5 times
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (16): //Uplink had not been fully uploaded (need code on each transmit to keep track of packages)
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (17): //Rockblock has no signal at all (ISBD_SUCCESS is false for too long)
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (18): //Temp sensor is off
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (19): //photoresistor is off
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (20): //A door button is being pressed when it shouldn't (there will be a case for each)
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (21): //Nitinol Wire actiavted, but photoresistor does not detect difference in light
          if (true) { //juts to pass compile   //have we failed to open the compartment? Have it take a pic and the rockblock send a picture
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (22):
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (23):
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;
        case (24):
          if (true) { //juts to pass compile
            MSH.faultReport[ReportOpenSpot + 1] = 1;
          } else {
            fault = true;
          }
          break;


      }
      if (fault) {
        faultTripped = true; // enter safe hold mode
        MSH.faultReport[ReportOpenSpot] = f; //we send the numbers, each corresponding to a specific fault
        ReportOpenSpot++;
      }
      fault = false;
    }
    f++;
  }

  //this comes after all fault cases have been checked
  //fault has a part of its downlink here because we dont want that as part of normal operations
  if (faultTripped) {
    Serial.println("A fault has been tripped!");
    //sendFault = true; //comment this for testing, probably RB is disconnected
    for (int g = 0; g < ReportOpenSpot; g++) {
      txbuffer[g] = MSH.faultReport[g];
    }
    openSpot = ReportOpenSpot; //setting openSpot here inside, not on a downlink loop/structure
    if (sendFault == true) {
      Downlink();
    } else {
      openSpot = 0;
    }
  }
  if (faultTripped && !sendFault) {
    faultTripped = false;
  }
}

void buildFaults() { //fills faultReport with zeroes, with 1s (ones) representing Faults tripped
  int f = 0;
  while (f < faultSize) {
    MSH.faultReport[f] = 0;
    f++;
  }
}

void FaultDownlink() { //take faultReport and puts into the buf for downlink for routine special report
  //int f = 0;
  //String g = "";
  //assumes openSpot = 0
  while (openSpot < faultSize) {
    txbuffer[openSpot] = MSH.faultReport[openSpot];
    openSpot++;
  }
  //MSH.FaultString = g;
}
