void SDInit() {
  Serial.println("Starting SD");
  MSH.SDActive = true;
  if (!SD.begin(BUILTIN_SDCARD))
  {
    Serial.println("SD Failed");
    MSH.SDActive = false;
  }
}
  void InitializeSdCam() {
  Serial.println("Starting SD");
  //delay(3000);
  MSH.SDActive = true; //second time called should not assume it is working
  if (!SD.begin(BUILTIN_SDCARD)) //need to check if it is also #4 on teensy 3.5
  {
    Serial.println("SD Failed");
    MSH.SDActive = false;
  }
  delay(3000);
  cam.reset();
  delay(3000);
  if (cam.begin()) {
    Serial.println("Camera Found:");
    MSH.CamActive = true;
  } else {
    Serial.println("No camera found?");
    return;
  }
  cam.setImageSize(VC0706_160x120); // executed twice, with a cam.reset in the middle, because, apparently, the camera is autistic
  cam.reset();
  cam.setImageSize(VC0706_160x120);

  uint8_t imgsize = cam.getImageSize();
  Serial.print("Image size: ");
  delay(1000);
  if (imgsize == VC0706_640x480) Serial.println("640x480");
  if (imgsize == VC0706_320x240) Serial.println("320x240");
  if (imgsize == VC0706_160x120) Serial.println("160x120");
  delay(1000);
}

void TakePicture() { //TODO this should be based off a timed photoburst
  Serial.println("Snap in 3 secs...");
  delay(3000);
  if (! cam.takePicture()) {
    Serial.println("Failed to snap!");
  }
  else {
    Serial.println("Picture taken!");
  }

  // Create an image with the name Axxx.JPG
  char filename[9]; //used only inside this function to name the file
  strcpy(filename, "A000.JPG"); // start position

  if (SD.exists(filename)) {
    Serial.println("Apparently, it has decided the previous name exists");
    for (int i = 1; i < 1000; i++) { // int starts at 1 since case 0 is already the starter position // size goes up to 999, only three numbers, useful for specific wipe operation

      String picnumber = String(i); //necessary?

      int ilength = picnumber.length();
      int nzeros = (3 - ilength);
      String zero = "0";

      if (nzeros != 0) {
        for (int z = 1; z < nzeros; z++) {
          zero += "0";
        }
        zero += picnumber;
        picnumber = zero;
      }

      String tempname1 = "A";
      String extension = ".JPG";
      String tempname2 = tempname1 + picnumber;
      String finalname = tempname2 + extension;

      char holder[9];
      finalname.toCharArray(holder, 9);
      strcpy(filename, holder);
      Serial.print("It came up, therefore, with a new name: ");
      Serial.println(filename);

      if (!SD.exists(filename)) { //breaks the loop if name chosen is not taken
        break;
      }
    }
  }
  if (!SD.exists(filename)) {
    Serial.print("File created: ");
    Serial.println(filename);
  }


  // Open File for writing
  File imgFile = SD.open(filename, FILE_WRITE); //Open Image File

  // Get the size of the image (frame) taken
  uint16_t jpglen = cam.frameLength();
  photosize = jpglen;
  Serial.print("Storing ");
  Serial.print(jpglen, DEC);
  Serial.println(" byte image.");
  //  uint8_t a[jpglen]; //has already een created in the header

  int32_t time = millis();
  pinMode(8, OUTPUT); //why 8? why do we need to have an output? Does this work well with Teensy? Testing required
  // Read all the data up to # bytes!
  int innerCount = 0; // For counting # of writes inside array a
  int wCount = 0; // For counting number of writes for normal operation
  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead);
    imgFile.write(buffer, bytesToRead); //which library is this?
    for (int i = innerCount; i < innerCount + bytesToRead; i++) { //do we want to copy automtically and send picture immediately, or have te option to select an image to send?
      a[i] = buffer[i - innerCount];
    }
    /// for byte string comparation ///
    for (int y = 0; y < bytesToRead; y++) {
      Serial.print(buffer[y]); Serial.print(",");

      if (wCount % 128 == 0) {
        Serial.println(".");
      }
      wCount++;
      innerCount++;
    }
    ///  ///
    //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
    jpglen -= bytesToRead;
  }
  imgFile.close();

  time = millis() - time;
  Serial.println("done!");
  Serial.print(time); Serial.println(" ms elapsed");
}

void ImageBuffer() { //this function is used to copy a file in the SD to an array for transmission //must still be integrated with rockblock commands
  //first, we take the umber from popcommand and open a file
  String popc = "02";
  int number = popc.toInt();
  Serial.print("number: ");
  Serial.println(number);
  //go through all files to search for one with the equivelent number:
  for (int i = 0; i < 1000; i++) { // int starts at 1 since case 0 is already the starter position // size goes up to 999, only three numbers, useful for specific wipe operation

    String picnumber = String(i); //necessary?
    String target = String(number);

    int ilength = picnumber.length();
    int nzeros = (3 - ilength);
    String zero = "0";

    if (nzeros != 0) {
      for (int z = 1; z < nzeros; z++) {
        zero += "0";
      }
      zero += picnumber;
      picnumber = zero;
    }

    String tempname1 = "A";
    String extension = ".JPG";
    String tempname2 = tempname1 + picnumber;
    String finalname = tempname2 + extension;

    char holder[9];
    finalname.toCharArray(holder, 9);
    strcpy(classfilename, holder);

    if (SD.exists(classfilename) && i == number) { //breaks the loop if we found the file with the correspondent number
      Serial.println("Target Picture Found for Buffer");
      break;
    }
    else if (!SD.exists(classfilename) && i == 999) {
      Serial.println("SD does not contain file");
      return; //break the function, like this? Something's missing
    }
  }

  File ImgtoCopy = SD.open(classfilename, FILE_READ); // We are reading the file found
  photosize2 = ImgtoCopy.size(); //here we can give the input to the true size of the array, photosize should alreay exist, though empty
  Serial.print("Photosize: ");
  Serial.println(photosize2, DEC);
  Serial.println("Starting Segmentation");
  //    uint8_t a[photosize]; // array should be initiated outside, so we can keep the value

  for (int i = 0; i < photosize2; i++) {
    a[i] = (uint8_t)ImgtoCopy.read();
    Serial.print(a[i]);
    Serial.print(",");
    if (i % 128 == 0 && i != 0) { // a dot every 128 loops
      Serial.println(".");
    }
  }

  Serial.println("");
  Serial.println("Done!");
  Serial.print("Size of a: ");
  Serial.println(sizeof(a));
  Serial.print("Photosize2: ");
  Serial.println(photosize2);
  ImgtoCopy.close();
}

void OutputB64String(uint8_t *a, int i) { //a is an array with the bytes of the picture we want to see, while i will be the size (stored in one of the photosizes) //should we store different photosizes?
  Serial.println(""); Serial.println("Printing Base 64 String of a: ");
  String b64 = Hash_base64(a, i);
  Serial.println(b64);
  Serial.println("String printed!");
  //this function is missing the code to output it, not into the serial, but for the downlink
}

void WipeAll() { // this function wipes our SD of all pictures
  Serial.println("Wiping SD Card");

  // Create the name basket to use
  char filename[9];
  Serial.println("Starting Loop Removal Operation");
  for (int i = 0; i < 999; i++) { // int starts at 0 for 000

    String picnumber = String(i); //necessary?

    int ilength = picnumber.length();
    int nzeros = (3 - ilength);
    String zero = "0";

    if (nzeros != 0) {
      for (int z = 1; z < nzeros; z++) {
        zero += "0";
      }
      zero += picnumber;
      picnumber = zero;
    }

    String tempname1 = "A";
    String extension = ".JPG";
    String tempname2 = tempname1 + picnumber;
    String finalname = tempname2 + extension;

    char holder[9]; //why does it need to be 10?? Well, it does work...
    finalname.toCharArray(holder, 9);
    strcpy(filename, holder);
    if (SD.exists(filename)) {
      SD.remove(filename);
    }
    Serial.print("Removed the following file: ");
    Serial.println(filename);
  }
}

void WipeOne() { //here as a function temporarily, until integrated into popcommands
  Serial.println("Wiping SD Card");

  // Create the name basket to use
  char filename[9];
  //next, a number received from popcommand
  String popc = "000"; //for testing only
  Serial.println("Starting Removal Operation");

  String picnumber = popc; //necessary?
  String tempname1 = "A";
  String extension = ".JPG";
  String tempname2 = tempname1 + picnumber;
  String finalname = tempname2 + extension;

  char holder[9]; //why does it need to be 10?? Well, but it does work...
  finalname.toCharArray(holder, 9);
  strcpy(filename, holder);
  if (SD.exists(filename)) {
    SD.remove(filename);
  }
  Serial.print("Removed the following file: ");
  Serial.println(filename);
  popc = ""; //needed???
}

int getNumPhotos() {
  char filename[9];
  Serial.println("Started Counting");
  int innercounter = 0;
  for (int i = 0; i < 999; i++) { // int starts at 0 for 000

    String picnumber = String(i); //necessary?

    int ilength = picnumber.length();
    int nzeros = (3 - ilength);
    String zero = "0";

    if (nzeros != 0) {
      for (int z = 1; z < nzeros; z++) {
        zero += "0";
      }
      zero += picnumber;
      picnumber = zero;
    }

    String tempname1 = "A";
    String extension = ".JPG";
    String tempname2 = tempname1 + picnumber;
    String finalname = tempname2 + extension;

    char holder[9]; //why does it need to be 10?? Well, it does work...
    finalname.toCharArray(holder, 9);
    strcpy(filename, holder);
    if (SD.exists(filename)) {
      innercounter += 1;
      Serial.print("Found the following file: ");
      Serial.println(filename);
    }
  }
  Serial.print("Total Count: ");
  Serial.println(innercounter);
  return innercounter;
}
