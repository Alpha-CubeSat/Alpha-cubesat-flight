
void commandParse() { //saving commands in here

  int commandData;
  int commandType; // changed from int to long int. so if it breaks this is probably why
  bool l = true;
  String i = "";
  i = receivedChars;

  while (l) {

    commandType = (receivedChars.substring(0, receivedChars.indexOf(","))).toInt();

    commandData = (receivedChars.substring(receivedChars.indexOf(",") + 1, receivedChars.indexOf("!"))).toInt();

    cBuf.commandStack[cBuf.openSpot][0] = commandType;
    cBuf.commandStack[cBuf.openSpot][1] = commandData;
    if (receivedChars.indexOf("!") == receivedChars.length() - 1) { //this assumes we are putting all commands one after the other
      l = false; //if command has an exclamation mark

    } else {

      receivedChars = receivedChars.substring(receivedChars.indexOf("!") + 1);
      i = receivedChars;

    }
    cBuf.openSpot++;
  }
}



void recvWithEndMarker() {
  // for serial
  //  static byte ndx = 0;
  //  char endMarker = '\n';
  //  char rc;

  //  while ( Serial.available() > 0 && newData == false) {
  //
  //    rc = Serial.read();
  //
  //    if (rc != endMarker) {
  //      receivedChars += (char(rc));
  //    }
  //    else {
  //
  //      newData = true;
  //    }
  //  }
  //  char copy[50]; //can be made smaller
  //  receivedChars.toCharArray(copy,7);
  //  Serial.println(copy);
  //is last place an exclamation mark?
  Serial.print("Inside End Marker, received chars is: "); Serial.println(receivedChars);
  Serial.println(receivedChars[0]); //connected to uplink function
  Serial.println(receivedChars[1]); // getting command from rockblock correctly saved
  Serial.println(receivedChars[2]); // onto this function
  Serial.println(receivedChars[3]); // checks if receivedChars[5] is an exclamation mark
  Serial.println(receivedChars[4]); // later on "is input valid" function does the rest of the checks
  Serial.println(receivedChars[5]);
  Serial.println(receivedChars[6]);
  if (receivedChars[5] == '!') { //assumes we do only one command at a time, not saving in the buffer //to do later
    newData = true;
  } else {
    Serial.println("Command does not end in '!'");
  }

}



boolean isInputValid() {
  // todo make timeout
  Serial.print("starting parse on:");
  Serial.println(receivedChars);
  //Check if incoming command string <input> is valid
  int lastPunc = 0; //1 if ",", 2 if "!", 0 Otherwise
  bool valid = true;
  int q = 0;
  int l = receivedChars.length();

  if (l < 4) {
    Serial.println("command too short");
    valid = false;
  }

  while (q < l) {
    char currentChar = receivedChars[q];
    //Serial.println(currentChar,HEX);
    q++;

    if (isPunct(currentChar)) {
      if (currentChar == (',')) {
        //Check if last was a period
        Serial.println("Comma Found");
        if (receivedChars[q - 2] == '!') {
          Serial.println("No First Command Number");
          valid = false;
          break;
        }
        if (lastPunc == 0 || lastPunc == 2) {
          Serial.println("Comma OK");
          lastPunc = 1;
        } else {
          Serial.println("2 Commas");
          valid = false;
          break;
        }
      } else if (currentChar == ('!')) {
        if (receivedChars[q - 2] == ',') {
          Serial.println("No Second Command Number");
          valid = false;
          break;
        }
        Serial.println("Excl Found");
        if (lastPunc == 1) {
          Serial.println("Period ok");
          lastPunc = 2;
        } else {
          Serial.println("2 Excl or No prior comma");
          valid = false;
          break;
        }
      } else if (currentChar == ('-')) {
        Serial.println("Hypen Found");
        if (receivedChars[q - 2] == ',') { //q incremented after value capture
          Serial.println("Negative Sign ok");
        } else {
          Serial.println("Hyphen in wrong place");
          valid = false;
          break;
        }
      } else if (currentChar == ('.')) {
        Serial.println("period ok");
      } else {
        Serial.println("Invalid Punc");
        valid = false;
        break;
      }
    } else if (isAlpha(currentChar)) {
      //Serial.println("Alpha");
      valid = false;
      break;
    } else if (isSpace(currentChar)) {
      //Serial.println("Space");
      valid = false;
      break;
    }

    //Detect no ending exclamation point
    if (q == receivedChars.length() - 1) {
      if (receivedChars[q] != '!') {
        //Serial.println("No Ending");
        valid = false;
        break;
      }
    }
    //Null Character in the middle
    if (currentChar == '\0' && q != receivedChars.length() - 1) {
      Serial.println("null character");
      valid = false;
      break;
    }
  }
  if (valid) {
    Serial.println("valid");
  }
  if (!valid) {
    Serial.println("invalid");
  }
  return valid;
}



void popCommands() {
  //Process all the Incoming Commands
  long start = millis();

  while (cBuf.openSpot > 0 && millis() - start < manualTimeout) {
    if (cBuf.openSpot > 0) {

      //Serial.println (cBuf.openSpot - 1);
      int currentCommand[2] = {cBuf.commandStack[cBuf.openSpot - 1][0], cBuf.commandStack[cBuf.openSpot - 1][1]};
      cBuf.commandStack[cBuf.openSpot - 1][0] = -1;
      cBuf.commandStack[cBuf.openSpot - 1][1] = -1;
      cBuf.openSpot --;

      //Supported Commands
      switch (currentCommand[0]) {
        case (1):
          MSH.PrintStatus();
          int a;

          while (a < MSH.binOpenSpot) {

            Serial.print(MSH.binArr[a]);
            a++;
          }
          Serial.println("");
          Serial.print("binary array length: ");
          Serial.print(MSH.binOpenSpot / 8);
          Serial.println(" bytes");

          MSH.binOpenSpot = 0;


          break;

        case (2):
          Serial.print(F("1,1!\n")); // <---found the secret to Serial1 :)
          break;

        case (3): //Send IMU data over downlink!
          Serial.println(F("1,0!\n"));
          Serial.println("Inside popcommand 03");
          uint8_t tempholder[14] = {71, MSH.MAG , MSH.GYR, MSH.ACC,MSH.MagAve[0], MSH.MagAve[1], MSH.MagAve[2], MSH.GyroAve[0], MSH.GyroAve[1], MSH.GyroAve[2], MSH.AccelAve[0], MSH.AccelAve[1], MSH.AccelAve[2], MSH.ImuTemp};
          //uint8_t tempholder[15] = {71, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 71}; //to test command
          int openSpotkeeper = openSpot;
          while (openSpot < openSpotkeeper + 15) {
            txbuffer[openSpot] = tempholder[openSpot]; //we are filing out the rest of the buf with the rest of the special report
            openSpot++;
          }
          ULC = true;
          Downlink();

          break;

        case (4):
          Serial.println("WEEEE ARRE INNN CASSEEEE 4");
          Serial.print("Magnetometer X:"); Serial.print(MSH.MagAve[0]);
          Serial.print(" Y:");  Serial.print(MSH.MagAve[1]);
          Serial.print(" Z:"); Serial.println(MSH.MagAve[2]);
          Serial.print("Gyroscope X:"); Serial.print(MSH.GyroAve[0]);
          Serial.print(" Y:");  Serial.print(MSH.GyroAve[1]);
          Serial.print(" Z:"); Serial.println(MSH.GyroAve[2]);
          Serial.print("Accelerometer X:"); Serial.print(MSH.AccelAve[0]);
          Serial.print(" Y:");  Serial.print(MSH.AccelAve[1]);
          Serial.print(" Z:"); Serial.println(MSH.AccelAve[2]);
          break;

        case (5):
          Serial.println(currentCommand[1]);
          break;

        case (20): // 20 downlink commands
          MSH.SRFreq = currentCommand[1]; //how many regular dls before a special dl
          break;

        case (21):
          Serial.print("Command recieved, command type is 21, Command data is");
          Serial.println(currentCommand[1]);
          break;

        case (22):
          MSH.SRTimeout = (1000 * currentCommand[1]); //how long should the code wait for the transmission to be sent (seconds)
          break;

        case (23):
          MSH.uplinkPeriod = (1000 * currentCommand[1]);
          break;

        case (30): //toggle active fault //todo test this
          if (MSH.faultReport[currentCommand[1]] == 0) {
            MSH.faultReport[currentCommand[1]] = 1;
          } else {
            MSH.faultReport[currentCommand[1]] = 0;
          }
          break;

        case (40): //Begin Image Downlink

          break;

        case (41): //Take Photos
          delay(1000);
          //MSH.CameraBurst = true; // do we want a burst or a single photo at a time?
          //sendSCommand(F("41,1!"));
          TakePicture();
          Serial.println(F("\nPhotoBurst Initiated"));
          break;

        case (42): { //Set PhotoBurst Time in seconds // do we want this?
            String com = "42," + String(currentCommand[1]) + "!";
            MSH.BurstDuration = currentCommand[1] * 1000;
            Serial.println(F("\nPhotoBurst Time Set"));
            //sendSCommand(com);
            break;
          }

        case (43): { //Get # of Available Photos // function needs to be built. Can depend on how we want to use the variable though
            MSH.numPhotos = getNumPhotos();
            Serial.println("\nPhotos Available: " + String(MSH.numPhotos));
            break;
          }

        case (44): { //switch downlinkSize to 200
            downlinkSize = 200;
            Serial.println("Changed downlinkSize to 200");
            break;
          }


        case (45): { //switch downlinkSize to 50
            downlinkSize = 50;
            Serial.println("Changed downlinkSize to 200");
            break;
          }

        case (50): //Wipe SD Card
          Serial.println(F("\nWiping SD Card"));
          //sendSCommand(F("47,1!"));
          WipeAll();
          break;

        case (51): //Wipe Picture 1 from SD Card //discuss how to erase different pictures at our command
          Serial.println(F("\nWiping chosen picture from SD Card"));
          //sendSCommand(F("47,1!"));
          WipeOne(); //function needs modification to take into account popcommand number
          break;

        case (52): //re-init SD card
          SDInit();

        case (60): //accelerometer scale
          IMURange(1, currentCommand[1]);
          break;

        case (61): //magnetometer scale
          IMURange(2, currentCommand[1]);
          break;

        case (62): //gyroscope scale
          IMURange(3, currentCommand[1]);
          break;

        case (63): // attempt to re-initialize IMU
          IMUInit();

        case (80): // 80 gyro commands
          MSH.XGyroThresh = currentCommand[1];
          break;

        case (81):
          MSH.YGyroThresh = currentCommand[1];
          break;

        case (82):
          MSH.ZGyroThresh = currentCommand[1];
          break;

        case (90): // 90 accelerometer commands
          MSH.XAccelThresh = currentCommand[1];
          break;

        case (91):
          MSH.YAccelThresh = currentCommand[1];
          break;

        case (92):
          MSH.ZAccelThresh = currentCommand[1];
          break;

        case (101): //100s are for security commands such as faultcheck and overwrite operations
          MSH.OVERWRITE = true; //activate OVERWRITE mode. Act responsibly!
          break;

        case (102):
          MSH.OVERWRITE = false; //activate OVERWRITE mode. Act responsibly!
          break;

        case (103): //
          MSH.overdown = currentCommand[1]; //makes downlinkPeriod equal to second number of command
          break;

        case (104):
          MSH.overup = currentCommand[1]; //makes uplinkPeriod equal to second number of command
          break;

        case (105):
          MSH.overmode = currentCommand[1]; //makes nextMode equal to second number of command
          break;

        case (110):
          MSH.faultReport[1] = 1; //trip fault protection for imu init failure
          break;
      }
    } else {
      Serial.println("No Command");
    }
  }
}
