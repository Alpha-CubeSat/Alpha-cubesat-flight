void defaultFunctions() {
  //faultCheck();
  //if (newData) {
  recvWithEndMarker(); //makes new data true if there is
  if (isInputValid() && (receivedChars != "")) {
    Serial.println("Starting CommandParse");
    commandParse();
  }
  popCommands();
  //incorrect format, tell ground //missing function here
  newData = false;
  //Serial.println("wiping serial buffer");
  receivedChars = "";
  //}
  ModeSwitch();
  powerMan(); //we must be able to command it if we wish so, unless we get into low power mode. danger is having the rockblock interrupt its work. could it damage the radio?
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////Setup & loop//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void setup() {
  //initialize
  Serial.begin (74880);
  //Serial1.begin (74880); //either 1 or 3. Must be consistent with header of the script
  Serial5.begin (38400); //camera
  //Serial1.begin (38400); //rockblock
  //Wire.begin();
  delay(1000);
  cBuf = commandBuffer();
  buildFaults();
  sensorInit();
  Serial.println("Setup Stage 1 Complete!");
  delay(1000);
  //    while (MSH.FREEHUB == false){
  //      //wait here for dorman cruise until hub is free
  //    }
  RBInit(); //rockblock deactivated. deactivate/activate downlink and uplink functions
  updateNormalSensors();
  Serial.println(MSH.Battery);
  Serial.println("Setup Stage 2 Complete!");
}

void loop() {
  //delay(3000); //for testing purposes
  //delay(500);
  defaultFunctions();
  updateNormalSensors(); //this should be within MNormal
  Serial.print("Calling modeCon with following nextMode: "); Serial.println(nextMode);
  modeCon(); //if we command a mode, we must be able to ignore this line, a boolean?
  nextMode = 1; //for testing purposes
  Serial.println("Calling commCon (Modified)");
  //delay(1000);
  commCon(); //has DLConf inside of it, which calls the downlink.uplink functions //obeys variables set by modeCon
  //faultCheck(); //at end of first loop
  //  while (picoperation<1){
  //  picoperation = 1;
  //  TakePicture();}
  Serial.println("///////////////////////////////////////////////////////////////////////////////////////");
}
