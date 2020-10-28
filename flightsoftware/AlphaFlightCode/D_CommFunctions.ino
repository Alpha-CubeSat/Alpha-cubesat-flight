////////////////////////////////////////////////////////
/////////////////////Initialization/////////////////////
////////////////////////////////////////////////////////

void RBInit() {
  //initalize rockblock
  IridiumSerial.begin(19200);
  modem.setPowerProfile(IridiumSBD::DEFAULT_POWER_PROFILE);
  Serial.println("Starting modem...");
  err = modem.begin();
  if (err != ISBD_SUCCESS)
  {
    Serial.print("Begin failed: error ");
    Serial.println(err);
    if (err == ISBD_NO_MODEM_DETECTED)
      Serial.println("No modem detected: check wiring.");
    return;
  }
  for (int i = 0; i < 256; i++) { //create a for testing
    a[i] = i;
  }


  if (true) {
    MSH.RBActive = true;
  } else {
    MSH.RBActive = false;
  }
  modem.adjustSendReceiveTimeout(60); //change before mission
}

////////////////////////////////////////////////////////
///////////////// Mode Selection ///////////////////////
////////////////////////////////////////////////////////


void commCon() { //does not need overwrite mod
  Serial.println("Calling ComCon: ");
  //    uint8_t testdata[] = {70,0,1,2,3,4,5,6,7,8,9,10,11,12};
  //    for(int i = 0; i<14; i++){
  //      buf[i] = testdata[i];
  //      openSpot++;
  //    }
  //    Serial.print("openSpot = ");
  //    Serial.println(openSpot);
  Serial.print("If statement for Downlink:");
  MSH.downlinkPeriod = 5000;
  Serial.println(MSH.downlinkPeriod);
  //    Serial.print("millis()");
  //    Serial.println(millis());
  if ((lastSend + MSH.downlinkPeriod) <= (millis()) && !DLConf) { // check if we're due for a downlink, also dont run when we're still waiting for DL confirmation
    Serial.println("Starting Downlink");
    Downlink();
  }
  openSpot = 0; //resets openspot anyway, how should we change this?
  //    Serial.print("openSpot = ");
  //    Serial.println(openSpot);
  Serial.print("If statemente for uplink:");
  MSH.uplinkPeriod = 5000;
  Serial.println(MSH.uplinkPeriod);
  //    Serial.print("millis()");
  //    Serial.println(millis());

  if (DLConf) {
    DownlinkCheck();
  }

  if (lastReceive + MSH.uplinkPeriod <= millis()) {
    Uplink();
    //sbd sendrecieve text
  }
  // if there are any incoming messages. if so call read function
  // or maybe don't? every time we downlink we'll know if there are
  // incoming messages
}

//the following functions are called inide downlink, so we always get the freshest data, and if we can't trasnmit, we do it again.

void SpecialDownLink() { //Fills buffer with Special Report

  uint8_t SpecialReport[] = {34, MSH.IMUActive, MSH.MAG, MSH.GYR, MSH.ACC, MSH.MagAve[0], MSH.MagAve[1], MSH.MagAve[2], MSH.GyroAve[0], MSH.GyroAve[1], MSH.GyroAve[2], MSH.AccelAve[0], MSH.AccelAve[1], MSH.AccelAve[2], MSH.MagLog[0][0], MSH.GyroLog[0][0], MSH.AccelLog[0][0], MSH.MagLog[0][1], MSH.GyroLog[0][0], MSH.AccelLog[0][0], MSH.MagLog[0][0], MSH.GyroLog[0][0], MSH.AccelLog[0][0], MSH.ImuTemp, MSH.Temp, MSH.SolarCurrent, MSH.Battery, MSH.DoorButton, MSH.Inhibitor_19a, MSH.Inhibitor_10b, MSH.Inhibitor_2, MSH.FREEHUB, nextMode, downlinkSize, MSH.downlinkPeriod, MSH.uplinkPeriod, MSH.MTQueued, MSH.SBDIXFails, 31};
  //uint8_t SpecialReport[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34};

  int openSpotkeeper = openSpot;
  while (openSpot < openSpotkeeper + 36) { //why 36?
    txbuffer[openSpot] = SpecialReport[openSpot]; //we are filing out the rest of the txbuffer with the rest of the special report
    openSpot++;
  }
}

void routineDownlinkData() { //Fills buffer with Routine Report //updating the variable data we receive with downlink, useful to save data in case we need to retrieve it
  // do we want to receive MagLog, GyroLog, AccelLog?
  // do we want to include DoorButton, lightSense and CurrentZero?

  if (nextMode != 2) { //not safehold
    uint8_t tempholder[17] = {21, MSH.MAG, MSH.GYR, MSH.ACC, MSH.MagAve[0], MSH.MagAve[1], MSH.MagAve[2], MSH.GyroAve[0], MSH.GyroAve[1], MSH.GyroAve[2], MSH.AccelAve[0], MSH.AccelAve[1], MSH.AccelAve[2], MSH.ImuTemp, MSH.Temp, MSH.SolarCurrent, MSH.Battery};
    //uint8_t tempholder[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    for (int i = 0; i < 17; i++) {
      MSH.VarHolder[i] = tempholder[i]; //do we even want to store this data?
    }
    MSH.lastSR++; //ask joao about this line

    //SpecialReport above has 14 array elemtns, so it goes from 0 to 13
    int openSpotkeeper = openSpot;
    Serial.println("Filling txbuffer");
    while (openSpot < openSpotkeeper + 17) {
      txbuffer[openSpot] = MSH.VarHolder[openSpot]; //we are filing out the rest of the buf with the rest of the special report
      Serial.print(txbuffer[openSpot]); Serial.print(", ");
      openSpot++;
    }
    Serial.println(""); Serial.print("Final openSpot for Routine"); Serial.print(openSpot);

    //after buffer is completed, make sure this function is proceeded by calling downlink

  } else { //scrap this, if we on fault mode, we send a special report everytime we can, instead of typical routine
    uint8_t tempholder[14] = {22, MSH.MagAve[0], MSH.MagAve[1], MSH.MagAve[2], MSH.GyroAve[0], MSH.GyroAve[1], MSH.GyroAve[2], MSH.AccelAve[0], MSH.AccelAve[1], MSH.AccelAve[2], MSH.ImuTemp, MSH.Temp, MSH.SolarCurrent, MSH.Battery};
    for (int i = 0; i < 14; i++) {
      MSH.VarHolder[i] = tempholder[i];
    }

  }
}

void PicDownlink() {
  Serial.println("We've entered PicDownlink Function");
  //First we start with the segmentation of an image on array a[5120], which happens procedurally, according to the success of the transmission
  if (isFirst) { //for first segment, where we set up bytesleft
    Serial.println("It seems this is the first segment of our image");
    bytesleft = photosize2;
    openSpot = min(340, bytesleft);
    isFirst = false; //set up complete
  }

  //for (uint16_t counter=0; counter<=photosize2; counter += min(340,bytesleft)){ //counter to go through the array, goes up to photosize2(name to change)
  //build correct txbuffer with the correct spots on image array, run once per counter loop
  Serial.println("building correct txbuffer for transmission");
  txbuffer[0] = 71; //serial code to identify this buf has a picture, we load the picture afterwards
  for (int i = 1; i < min(340, bytesleft); i++) {
    //if
    txbuffer[i] = a[piccounter + i];
  }
  Serial.print("How many spots of the txbuffer we are using: ");  Serial.println(min(340, bytesleft));

  for (int g = 0; g < min(340, bytesleft); g++) {
    Serial.print(txbuffer[g]); Serial.print(", "); //for testing purposes, take this out and uncomment next line for real action
  }
  //x = ISBD_SUCCESS; //for testing purposes, the RB must return this value, in case transmission is successful
  Serial.println("...picdownlink txbuffer print terminated");

  x = modem.sendSBDBinary(txbuffer, min(340, bytesleft)); // sends from 0 to openspot - 1 (all space in array used)
  // for now assume that we could fit the regular transmission and the special report on the same downlink
  // clear buffer of all sent data. be sure that the message has been sucessfully transmitted.
  imgsent = true; //last img sent is an image, needed for downlink check!
  DLConf = true; //this will call the checker indirectly

}

void Downlink() { // outgoing data must be placed in txbuffer before downlink is called
  //delay(1000);
  bool success = false;
  unsigned long Begin = millis(); //see what time we are starting
  unsigned long difference = millis() - Begin;
  MSH.absolute1 = abs(difference);
  Serial.println("MSH.absolute1: "); Serial.println(MSH.absolute1);

  while (MSH.absolute1 < 300000) {
    int signalQuality = -1;
    int err;
    Serial.println("Inside Downlink While Loop");

    //uncomment for action
    err = modem.getSignalQuality(signalQuality);
    if (err != ISBD_SUCCESS)
    {
      Serial.print("SignalQuality failed: error ");
      Serial.println(err);
      return;
    }

    Serial.print("On a scale of 0 to 5, signal quality is currently ");
    Serial.print(signalQuality);
    Serial.println("."); //if zero, lol
    //delay(1000);

    //for testing purposes:
    //signalQuality = 1;
    Serial.print("DLConf: "); Serial.println(DLConf);
    Serial.print("MSH.lastSR :"); Serial.println(MSH.lastSR);
    Serial.print("MSH.SRFreq :"); Serial.println(MSH.SRFreq);
    Serial.print("openSpot :"); Serial.println(openSpot);
    Serial.print("sendFault :"); Serial.println(sendFault);


    if (DLConf == false && signalQuality > 0) {// is there still an outgoing message awaiting confirmation //make sure this is no a waste of time, rockblock may be able to transmit with signal quality zero!
      sendFault = false;
      if (sendFault != true && ULC != true) {
        if (MSH.lastSR > MSH.SRFreq /*|| booleanforspecialreportrequest*/) { // int can change based on how often you'd like special data.
          //openSpot = 0;//rewriting txbuffer
          Serial.println("Special Report Called");
          SpecialDownLink();
          MSH.lastSR = 0;
        } else { //routine downlink then!
          //openSpot = 0;//rewriting txbuffer
          Serial.println("Routine Report Called");
          routineDownlinkData();
        }
      } //if there's a fault, txbuffer has been filled and we are ready to send
      if (sendFault != true && ULC == true) {
        //for an uplink
        Serial.println("ULC == true, sending an uplink data request!");
        //routineDownlinkData();
      }
      if (sendFault == true) {
        Serial.println("Printing fault report");
      }
      for (int g = 0; g < openSpot; g++) {
        Serial.print(txbuffer[g]); Serial.print(", "); //for testing purposes, take this out and uncomment next line for real action
      }
      Serial.println("...downlink txbuffer print terminated");
      x = modem.sendSBDBinary(txbuffer, openSpot); // sends from 0 to openspot - 1 (all space in array used)
      openSpot = 0; // max and I added this in an attempt to fix extra zeros
      imgsent = false; //check picture code again
      sendFault = false; // after fault report has been sent
      lastSend = millis();
      // for now assume that we could fit the regular transmission and the special report on the same downlink
      // clear buffer of all sent data. be sure that the message has been sucessfully transmitted.
      DLConf = true;
      //MSH.absolute1 = 300001;
      success = true;

      if (sendFault != true && ULC == true && DLConf == true) {
        Serial.println("ULC == true, calling downlinkcheck!");
        DownlinkCheck();
      }
    }
    //read comments for uplink below (100 lines or so below)
    unsigned long difference = millis() - Begin;
    MSH.absolute1 = abs(difference);
    if (success) {
      MSH.absolute1 = 300001;
    }
    //teensy clock resets after 52 days. has to be an absolute value (ie positive...) so that this works
    //otherwise stuck forever
  }
}


void DownlinkCheck() { // Should now be finished
  Serial.print("downlinkcheck: ");
  Serial.println(DLFailCounter);
  Serial.print("ISBD Sucess Status: ");
  Serial.println(ISBD_SUCCESS);
  Serial.print("X value: ");
  Serial.println(x);

  if (x == ISBD_SUCCESS && ULC == true) { //Everything worked out, reset everything needed to go again
    Serial.println("Success! We sent data requested by Uplink");
    ULC = false;
    MSH.latestmsgcount = modem.getWaitingMessageCount();
    openSpot = 0;
    lastSend = millis();
    DLConf = false;
    DLFailCounter = 0;
    //exit; //what is this?
  } if (ULC == true && x != ISBD_SUCCESS && DLFailCounter < 4) {
    //TRY AGAIN!
    Downlink();
  } if (ULC == true && x != ISBD_SUCCESS && DLFailCounter > 3) {
    //call fault?
    Serial.println("Too many tries, what do?");
  }


  if (x == ISBD_SUCCESS && imgsent) { //imgsent is a boolean used to verify if the last msg sent was a segment of our image
    Serial.println("Last msg was an imag, and we have received confirmation it was successfully transmitted");
    imgsent = false;
    Serial.println("What we received: ");
    for (int i = 0; i < min(downlinkSize, bytesleft); i++) {
      Serial.print(txbuffer[i]); Serial.print(",");
      if (i % downlinkSize == 0) {
        Serial.println("");
      }
    }
    piccounter += min(downlinkSize, bytesleft); //advances the beginning of the copy from array a to txbuffer
    bytesleft -= min(downlinkSize, bytesleft); //tells us how much of th txbuffer will be busy with bytes from image //bytesleft, imgsent, and counter should only be changed by the checker, guaranteeing we are always sending fresh bytes at the right time
    openSpot = min (downlinkSize, bytesleft);
    Serial.print("bytesleft: "); Serial.println(bytesleft);
    Serial.print("piccounter: "); Serial.println(piccounter);


    if (bytesleft == 0) {
      Serial.print("bytesleft is equal to zero. Message sent. Setting counter = 0; isFirst = true;");
      piccounter = 0; //resets counter for next image
      isFirst = true; //resets boolean, since we will be strating again.
    } else {
      PicDownlink();
    }


  }
  if (x == ISBD_SUCCESS && ULC == true) { //Everything worked out, reset everything needed to go again
    Serial.println("Success! We sent data requested by Uplink");
    ULC = false;
    MSH.latestmsgcount = modem.getWaitingMessageCount();
    openSpot = 0;
    lastSend = millis();
    DLConf = false;
    DLFailCounter = 0;
    //exit; //what is this?
  }

  if (x == ISBD_SUCCESS) { //Everything worked out, reset everything needed to go again
    Serial.println("Success!");
    openSpot = 0;
    lastSend = millis();
    DLConf = false;
    DLFailCounter = 0;
    //exit; //what is this?
  }
  if ((lastSend + MSH.downlinkPeriod + MSH.SRTimeout) <= millis()) {  //sr timeout is the max wait after the supposed transmission
    DLFailCounter++;
    if (!imgsent) {
      openSpot = 0;
      lastSend = millis();
      DLConf = false;
    } else {
      PicDownlink();
    }
    if (DLFailCounter >= 3) {
      DLConf = 0;
      openSpot = 0;
      imgsent = false;
      piccounter = 0;
      //faultCheck(); //run faultCheck
      //DLFailCounter = 0; //done within the fault case in above function
    }
  }

}

void Uplink() { // function to receive commands //gotta load the receive commands in a string, so we can check it with the other functions that is valid
  Serial.println("top of uplink");
  //delay(1000);
  bool success = false;
  unsigned long Begin = millis(); //see what time we are starting
  unsigned long difference = millis() - Begin;
  MSH.absolute2 = abs(difference);

  Serial.print("MSH.absolute2: "); Serial.println(MSH.absolute2);

  while (MSH.absolute2 < 300000) {

    Serial.print("MSH.absolute2: "); Serial.println(MSH.absolute2);
    int signalQuality = -1;
    int err;
    err = modem.getWaitingMessageCount();
    Serial.print("Message Count: "); Serial.println(err);

    err = modem.getSignalQuality(signalQuality);
    if (err != 0)
    {
      Serial.print("SignalQuality failed: error ");
      Serial.println(err);
      //exit(1);
    }
    Serial.print("Signal quality is ");
    Serial.println(signalQuality);

    // Read/Write the first time or if there are any remaining messages
    if (signalQuality > 0) {
      //if ( modem.getWaitingMessageCount() > 0){
      Serial.println("Attempting Uplink with Quality>0!");


      err = modem.sendReceiveSBDBinary(txbuffer, txbufferSize, rxbuffer, rxbufferSize); //this is the actual one
      // err = modem.sendReceiveSBDBinary("", txbufferSize, rxbuffer, rxbufferSize); //this is a test to silence downlinks

      Serial.println("sendReceiveBinary Executed!");

      if (err != ISBD_SUCCESS)
      {
        Serial.print("sendReceiveSBD* failed: error ");
        Serial.println(err);
      }
      else // success!
      {
        //messageSent = true;
        lastReceive = millis();
        Serial.print("Inbound buffer size is ");
        Serial.println(rxbufferSize);
        for (int i = 0; i < rxbufferSize; ++i)
        {
          Serial.print(rxbuffer[i], HEX);
          if (isprint(rxbuffer[i]))
          {
            Serial.print("Write(");
            Serial.write(rxbuffer[i]);
            Serial.print(")");
            Serial.print(" ");
            Serial.print("Print(");
            Serial.print(rxbuffer[i]);
            Serial.print(")");
            rxOutput = rxbuffer[i];
            rxOutputCom += String(rxOutput);
            Serial.print(" ");
            Serial.print("String(");
            Serial.print(rxOutput);
            Serial.print(")");
          }
        }

        Serial.println(" ");
        //was a success, so now tell land it was all good!
        Serial.print("does it check with test code?: "); Serial.println(rxOutputCom == "03,01!");
        uint8_t succ[3] = {255, 200, 255}; //success signal
        uint8_t s = 3;
        int err2 = modem.sendSBDBinary(succ, s); //this may work, and we don't really care if it does //to test
        Serial.println("   RECEIVE SUCCESS!!");
        Serial.println();
        Serial.print("Total String: ");
        Serial.println(rxOutputCom); //here we have the command correctly made into a string!
        Serial.println();
        Serial.print("Messages remaining to be retrieved: ");
        Serial.println(modem.getWaitingMessageCount()); //this will only update the message
        //count if there's been a successful message exchanged
        //check this part
        receivedChars = rxOutputCom; //final output that function emits from rockblock
        Serial.println("receivedchars: "); Serial.println(receivedChars);
        receivedSize = rxbufferSize; //should always be 6
        rxOutput = ""; //erases values so it doesn't interfere with future comms
        rxOutputCom = "";
        //MSH.absolute2 = 300001; //breaks loop, and we only return once uplink period allows
        success = true;
      }

    }
    //tries to send data for 5 minutes
    //this time window prevents it from constantly trying
    //if not getting transmissions, won't get data for a while
    // better to turn the radio off for a while and then try again later  (triggered again by ground)

    difference = millis() - Begin; //always negative
    MSH.absolute2 = abs(difference); //this makes it positive
    if (success) { //if successful
      MSH.absolute2 = 300001; //breaks away (5 min) can edit if you think you should (could be a variable)
    }
  }
  //on 52nd day, code does only one loop but it does not break
  //clock resets, does not stay in the while loop forever.
}
//everything about uplink is the same for downlink
