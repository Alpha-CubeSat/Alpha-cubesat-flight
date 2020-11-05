/*test
  //////////////////////////////////////////////////
  CompleteMinusACS as of 07/19/20:

  TODO:
  question: do we want it to flush the uplink commands? after some time?
  verify camera integration with masterstatus/rockblock variables (fix photosize2 double name)
  have overwrite variables initial state change with flow so that when overwrite is true, the operations doesnt change abruptly
  complete fault report, and list all different downlinks with max
  //make sure fault buf doesnt superseed anything on buf (be it either downlink or uplink)
  values are mapped, make sure system work and max knows of the different bounds. we have to send that over a donwlink/uplink, which is done but not tested
  setup the modes
  op code for fault string for max may require more than one byte
  create updateLowSensors for Low Power mode //discuss with joshua
  save uplink commands in buffer and be capable of accessing them each at a time //make sure the rockblock transmits requested data until a deadline. How many commands we want to be stored?
  See how much the battery is drained. add a clause to break from command if battery gets too low on downlink/uplink while loop
  make sure the while loops inside uplink and downlink can have flexible durations

  TODO ACS:
  decide how to activate permission
  decide how to activate acsmodes
  verify if the acs work has a loop inside or if it performs things with checks every loop, letting the current on for a while
  change acs variables location to adequate position
  verify their values

  Changes:
  added ACS code!
  added count of 5 mins to keep trying to do downlink/uplink.
  txbuffer and rx buffer interated
  rockblock code integrated
  special and routine report structure done
  fault string which is sent has numbers to identify which faults have been activated
  sendFault records if a fault has been detected. after loop inside fault check is complete, it sends a downlink

  //////////////////////////////////////////////////
*/
#include <Wire.h>
#include <IridiumSBD.h>
#define IridiumSerial Serial1
#define DIAGNOSTICS false // Change this to see diagnostics
#include <TimerOne.h>
#include <StarshotACS0.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>
#include <SD.h>
#include <Adafruit_VC0706.h>
//////////////// RockBlock Setup Data ////////////////

IridiumSBD modem(IridiumSerial);

//uint16_t bytesleft; //declared, needs to be integrated into hybrid //added a 2 since it is used by a library //declared at pic downlink
uint16_t counter = 0; //starts as 0, no image has been segmented for downlink yet // needs to be integrated to hybrid
//boolean to know when we are sending the first segment of an image in a downlink -> needs to be created in hybrid
//bool isFirst = true; //declared at pic downlink
//bool imgsent = false; // last msg sent on downlink was an image. Starts as false //declared at pic downlink
int picoperation = 0;

int exampleData = 7471236;
uint32_t downlinkSize = 70; //there may be uplink
//uint8_t buf[340]; //340 is the max for rockblock split between txbuffer and rx buffer
int openSpot = 0;
uint8_t lastReceive = 0; //is this number appropriate for startup?
uint8_t lastSend = 0;
bool DLConf = false; //should you be checking to confirm sucessful downlink. true = yes, false = no
uint8_t lastSR = 0; // how many transmissions have passed since the last special report
//uint32_t downlinkPeriod = 900000; // how many millis to wait between each downlink
//uint32_t uplinkPeriod = 900000; // how many millis to wait before each uplink
uint16_t SRFreq = 10; //how many regular downlinks between special reports
uint32_t SRTimeout = 300000; // how many millis will the rockblock attempt to downlink
uint16_t DLFailCounter = 0; //how many times has downlinking failed
int x;
int err;
bool ULC = false; //used to send data from a popcommand

//for ACS //need to put this in an adequate place (maybe inside the class is not a good idea.
//Keep the variables simple may be better, at your discretion - make sure it doesn't interfere
// with matlab generated library)
int permission = 0;
int count = 0;
int pinset = LOW;
int ACSmode = 0;
int Xtorqorder = 1; //read Armen's code
int Ytorqorder = 1;
int Ztorqorder = 1;
float current1;
float current2;
float current3;
float Res_torquer = 100.0;
int yout1 = 5; //hbridges output
int yout2 = 6; //hbridges output
int zout1 = 9; //hbridges output
int zout2 = 10; //hbridges output
int xout1 = 7; //hbridges output
int xout2 = 8; //hbridges output
int xPWMpin = 2; //hbridges input
int yPWMpin = 3; //hbridges input
int zPWMpin = 4; //hbridges input
//int LEDpin = 13; //testing pin, might need translation to teensy 3.5
int STBYpin = 21; //hbridges input
int STBZpin = 20; //hbridges input

//////////////// RockBlock Setup Data End ////////////////

static StarshotACS0ModelClass rtObj;
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
Adafruit_VC0706 cam = Adafruit_VC0706(&Serial5);

//SD Card
#define chipSelect BUILTIN_SDCARD
#define DLSize 320
#define WireTransferSize 32

long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second =  1000; // 1000 milliseconds in a second

unsigned long manualTimeout = 10 * 1000;
int endT = 0;
unsigned long int z; //number of millis in current cycle
uint32_t nextMode = 0; // mode of the spacecraft next cycle

//downlink msg test!
uint8_t txbuffer[70]; //where u save downlink data to save back to earth
size_t txbufferSize = sizeof(txbuffer);

//for uplink! This time saved!
uint8_t rxbuffer[70]; //data u are receiving, should be empty when used
size_t rxbufferSize = sizeof(rxbuffer);
//Serial.println(sizeof(rxBuffer));
//the above four entities get used in line 2760something - the err line.
char rxOutput = "";
String rxOutputCom = "";
int receivedSize = -1;

//Deployment commands
long int burnTimer;
int deployStage = 0;
bool DLKey1 = false;
bool DLKey2 = false;
//uint8_t faultReport[40];
int ReportOpenSpot = 0;
bool sendFault = false;
bool faultTripped = false;
const int SenseNumb = 10; //change number of data samples in averaging array
const int faultSize = 40; // always make a multiple of 8 //size of faultReport
bool SensUpdate = false; //is there fresh IMU data
bool IMUBufferFull = false; //must fill w/ data before averages can be taken

boolean newData = false;
String receivedChars = "";

//Camera
#define smallImage VC0706_160x120
#define mediumImage VC0706_320x240
#define largeImage VC0706_640x480
long lastCamCheckTime = 0;
int camCheckTime = 4070; // what are you?
bool camStatus = false;
//Camera Functions
char classfilename[9]; //general filename, to be used by camera functions for transmission
uint16_t photosize; //needed to keep track of size of our first image
uint16_t photosize2; //needed to keep track of our true size of the buffer array
uint8_t a[5120]; //array is a general one

//Picture Downlink
uint16_t bytesleft; //declared, needs to be integrated into hybrid //Already declared at RB Init
uint16_t piccounter = 0; //starts as 0, no image has been segmented for downlink yet // needs to be integrated to hybrid
bool isFirst = true; //boolean to know when we are sending the first segment of an image in a downlink -> needs to be created in hybrid
bool imgsent = false; // last msg sent on downlink was an image. Starts as false

static const char b64chars[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; //Remove before Flight //TODO

////////////////////////////////////////////////////////////////////////////////
/////////////////Database of parameters/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class masterStatus {
    //Class to hold entire State of Spacecraft Operation except timers
  public:
    // To others looking at this code:
    //it is important that as few of these variables as possible get arbitrarily assigned a value.
    // these should only be given real data
    int State;
    int LowPowerTimer;
    bool OVERWRITE; //this boolean allows commands from user to overwrite the normal spacecraft behavior. It is used in PowerMan, ModeSwitch, faultCheck
    uint32_t overdown; //this and the next two ints are used by overwrite commands
    uint32_t overup;
    uint32_t overmode;
    //uint32_t State; // what does this do
    uint32_t NextState; // I don't think we need the state stuff
    uint32_t Faults; //number of faults detected over satellite lifetime
    uint8_t faultReport[faultSize]; //What faults are we checking the state of
    //String FaultString; // turn activefaults into a string for easy transmitting stupid idea, strings resrve two bytes per character

    //ROCKBLOCK Message Variables
    bool AttemptingLink; //True if waiting for SBDIX to return
    bool MessageStaged; //True if message waiting in Mobile Originated Buffer
    int RBCheckType; //State of Outgoing Communication with RockBlock. 0=ping, 1=Send SBDIX, 2=Fetch Incomming Command
    int MOStatus; //0 if No Outgoing message, 1 if outgoing message success, 2 if error
    int MOMSN; //Outgoing Message #
    int MTStatus; //0 if No Incoming message, 1 if Incoming message success, 2 if error
    int MTMSN; //Incoming Message #
    int MTLength; //Incoming Message Length in bytes
    uint32_t MTQueued; //# of messages waiting in iridium //setup this to work
    String SBDRT;
    int LastMsgType; //0 = inval, 1 = ok, 2 = ring, 3 = error, 4 = ready //TODO
    int LastSMsgType; //Only Update on NON EMPTY reads from RB: 0 = inval, 1 = ok, 2 = ring, 3 = error, 4 = ready //TODO
    uint32_t SBDIXFails; //setup this to work
    int latestmsgcount; //msg counts after a successful downlink (how many we have in uplink waiting for it to be read
    unsigned long absolute1;
    unsigned long absolute2;

    //Binary array variables
    int binArr[1000];
    int binOpenSpot = 0;

    int XGyroThresh;
    int YGyroThresh;
    int ZGyroThresh;
    int XAccelThresh;
    int YAccelThresh;
    int ZAccelThresh;
    //ACS MAGNETORQUER added, just check if it works.
    //first test in normal conditions, then use joao's rig
    //rig is 5-10x magnetic field strength of earth - one axis should be fine
    //compass360pro - app joao used to measure field strength
    //generate field going from north to south, pretend it's on the equator
    //point IMU up or down to eliminate one axis on the gyroscope
    //whatever spin is doing, the string must follow
    int XMagThresh;
    int YMagThresh;
    int ZMagThresh;

    //Camera Variables
    bool CamActive;
    bool SDActive;
    volatile int ITStatus;
    uint32_t numPhotos;
    bool CameraBurst;
    uint32_t imageSize; //this is our photosize/photosize2
    unsigned long burstStart;
    unsigned long BurstDuration;


    //  get all the min max vals/percision
    //storu IMU values also in nomrla format offered by library, check which is better.
    float ACC;
    float MAG;
    float GYR;

    uint8_t Mag[3];
    uint8_t Gyro[3];
    uint8_t Accel[3];

    uint8_t MagAve[3];
    uint8_t GyroAve[3];
    uint8_t AccelAve[3]; //how do we transmit these three values? with such low cap for byte transmission....

    uint8_t MagLog[3][SenseNumb];
    uint8_t GyroLog[3][SenseNumb];
    uint8_t AccelLog[3][SenseNumb];

    uint8_t ImuTemp;
    uint32_t Temp;
    uint32_t TempAccumulator;
    uint32_t DoorButton; // what is the default reading of a button
    uint32_t lightSense;
    uint32_t SolarCurrent;
    uint32_t CurrentZero;
    // may need to store the zeros aswell.

    //COntants for Current Sensor
    //const int SOLAR_PIN;      // Input pin for measuring Vout
    const float RS = 0.1;           // Shunt resistor value (in ohms)
    const float VOLTAGE_REF = 3.3;  // Reference voltage for analog read
    const float RL = 29.75;         // Load Resistor value NOTE: in kOhms
    //float sensorValue;              // Analog Signal Value
    //float volt;                     // Voltage after conversion from analog signal
    //float amp;

    //Inhibitors
    uint32_t Inhibitor_19a;
    uint32_t Inhibitor_10b;
    uint32_t Inhibitor_2;
    uint32_t FREEHUB;


    //ADCS State Variables: torque rods do we need pwm?
    uint32_t TorqueXDir; //-1 or 1 for Coil Current Direction
    uint32_t TorqueXPWM; // 0 to 255 for Coil Current Level
    uint32_t TorqueYDir; //-1 or 1 for Coil Current Direction
    uint32_t TorqueYPWM; // 0 to 255 for Coil Current Level
    uint32_t TorqueZDir; //-1 or 1 for Coil Current Direction
    uint32_t TorqueZPWM; // 0 to 255 for Coil Current Level
    uint32_t IMUActive;

    //power board variables
    uint8_t Battery; // battery charge

    //ADCS State Variables: torque rods

    //Radio State Variables:
    //todo flesh out once we get our hands on a radio
    bool RBActive;
    bool DownlinkStaged;
    bool AttemptingDownLink;
    bool StageReport;
    uint32_t lastSR; //last special report, keeps track of number of normal reports since last special report
    uint32_t downlinkPeriod; // how many millis to wait between each downlink
    uint32_t uplinkPeriod; // how many millis to wait before each uplink
    uint32_t SRFreq; //how many regular downlinks between special reports
    uint32_t SRTimeout;
    uint32_t VarHolder[13]; //I still dont get this

    masterStatus() {

      //Constructor
      State = 1; //normal ops

      //finalize log calculator and then just initialize it here (basically, get the numbers of zero right)

      //might be able to average more than 3 values if teensy can handle it. 10?
      //averaging might not be a good idea, by modal? ask nathan...
      //      MagLog = {{0,0,0},{0,0,0},{0,0,0}};
      //      GyroLog = {{0,0,0},{0,0,0},{0,0,0}};
      //      AccelLog = {{0,0,0},{0,0,0},{0,0,0}};
      //
      //      GyroAve[3] = {0,0,0}; // sent to us by slave comp
      //      MagAve[3] = {0,0,0}; // sent to us by slave comp
      //      AccelAve[3] = {0,0,0}; // sent to us by slave comp
      //initial conditions:
      ACC = 2; //for 2G
      MAG = 4; //for 4Gauss
      GYR = 245; //for DPS


      TorqueXDir = 0;
      TorqueXPWM = 0;
      TorqueYDir = 0;
      TorqueYPWM = 0;
      TorqueZDir = 0;
      TorqueZPWM = 0;
      IMUActive = false; //used for healthcheck

      ImuTemp = 0; //integrated IMU temp sensor
      Temp = 0; // external thermistor
      DoorButton = 0; //TODO
      lightSense = 0;
      Battery = 3.7; //todo i'd rather not have the default value trip fault detection 50%?
      SolarCurrent = 0;
      CurrentZero = 0;

      CamActive = false; //used for healthcheck
      SDActive = false; //used for healthcheck
      ITStatus = 0; //todo joao
      numPhotos = 0;
      CameraBurst = 0;
      imageSize = 0; //this is our photosize/photosize2
      burstStart = 0;
      BurstDuration = 15000;

      Faults = 0;

      //      RS = 0.1;           // Shunt resistor value (in ohms)    //these three are declared right way since they are const and in C++
      //      VOLTAGE_REF = 3.3;  // Reference voltage for analog read
      //      maybe set voltage_ref to the value that the battery level reader?
      //might not make a difference, but test to be sure.
      //      RL = 29.75;

      //to erase
      Inhibitor_19a = 200;
      Inhibitor_10b = 200;
      Inhibitor_2 = 200; //any value for these three above 0 is taken as true
      FREEHUB = 0; //used for GREENLIGHT function, deterine if cubesat left p-pod //value is zero for false


      OVERWRITE = false; //default values to start spacecraft operations
      overdown = 900000;
      overup = 60000;
      overmode = 0;

      downlinkPeriod = 60000; // how many millis to wait between each downlink
      uplinkPeriod = 1000; // how many millis to wait before each uplink
      SRFreq = 10; //how many regular downlinks between special reports
      SRTimeout = 300000;

      RBActive = false;
      DownlinkStaged = false; // have data to transfer
      AttemptingDownLink = false; // radio transmission in progress
      StageReport = false;
      lastSR = 0;
      latestmsgcount = 0;
      absolute1 = 0;
      absolute2 = 0;

    }

    float roundDecimal(float num, int places) { // is this still needed
      int roundedNum = round(pow(10, places) * num);
      return roundedNum / ((float)(pow(10, places)));
    }

    void PrintStatus() { // all data should be ints (still works w/ floats though..)
      build1Byte(State);
      build3Byte(GyroAve[0]); build3Byte(GyroAve[1]); build3Byte(GyroAve[2]); // change to ints.
      build3Byte(MagAve[0]); build3Byte(MagAve[1]); build3Byte(MagAve[2]);
      build3Byte(AccelAve[0]); build3Byte(AccelAve[1]); build3Byte(AccelAve[2]);
      build1Byte(ImuTemp);
      build1Byte(Temp);
      build1Byte(Battery);
      build1Byte(Faults);
    }

    //    String SpecialReport() { //todo make this work w/ binary //Special Report is now a separate function
    //      // all fixed values
    //      String output = "";
    //      output += "{";
    //      output += "Fl:" + String(Faults) + ",";
    //      output += "FlSt:" + String(FaultString);
    //      output += "SRF:" + String(SRFreq) + ",";
    //      output += "XGT:" + String(XGyroThresh) + ",";
    //      output += "YGT:" + String(YGyroThresh) + ",";
    //      output += "ZGT:" + String(ZGyroThresh) + ",";
    //      output += "XAT:" + String(XAccelThresh) + ",";
    //      output += "YAT:" + String(YAccelThresh) + ",";
    //      output += "ZAT:" + String(ZAccelThresh) + "}";
    //      return (output);
    // all misc information


    /////////////////binary functions//////////////////

    void build3Byte(long int numb) {
      //range +- 8,388,607

      //if we have any values above 8 million we probably have a problem :)

      if (numb >= 0) { //if positive first bit = 1
        binArr[binOpenSpot] = 1;
        binOpenSpot ++;
      } else {
        binArr[binOpenSpot] = 0;
        binOpenSpot ++; //if negative first bit = 0
        numb = abs(numb);
      }

      if (numb > -8388607 && numb < 8388607) {


        if (numb < 2) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 4) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 8) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 16) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 32) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 64) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 128) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 256) {
          binArr[binOpenSpot] = 0; binOpenSpot ++; // 1 byte
        }
        if (numb < 512) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 1024) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 2048) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 4096) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 8192) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 16384) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 32768) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 65536) {
          binArr[binOpenSpot] = 0; binOpenSpot ++; //2byte
        }
        if (numb < 131072) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 262144) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 524288) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 1048576) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 2097152) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 4194304) {
          binArr[binOpenSpot] = 0; binOpenSpot ++; //23 bits of data
        }

      } else {
        numb = 8388607;
      }
      String h = "";
      int a = 0;
      h += String(numb, BIN);
      int msgLen = h.length();

      while (msgLen != 0) {

        String x = h.substring(a, a + 1);
        char d[10];
        x.toCharArray(d, 10);

        if (int(d[0]) == 49) {
          binArr[binOpenSpot] = 1;
        } else {
          binArr[binOpenSpot] = 0;
        }

        binOpenSpot++;
        a++;
        msgLen--;
      }
    }


    void build2Byte(long int numb) {
      //range +- 32,767

      if (numb >= 0) { //if positive first bit = 1
        binArr[binOpenSpot] = 1;
        binOpenSpot ++;
      } else {
        binArr[binOpenSpot] = 0;
        binOpenSpot ++; //if negative first bit = 0
        numb = abs(numb);
      }

      if (numb > -32767 && numb < 32767) {


        if (numb < 2) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 4) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 8) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 16) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 32) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 64) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 128) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 256) {
          binArr[binOpenSpot] = 0; binOpenSpot ++; // 1 byte
        }
        if (numb < 512) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 1024) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 2048) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 4096) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 8192) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 16384) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }

      } else {
        numb = 32767;
      }
      String h = "";
      int a = 0;
      h += String(numb, BIN);
      int msgLen = h.length();

      while (msgLen != 0) {

        String x = h.substring(a, a + 1);
        char d[10];
        x.toCharArray(d, 10);

        if (int(d[0]) == 49) {
          binArr[binOpenSpot] = 1;
        } else {
          binArr[binOpenSpot] = 0;
        }

        binOpenSpot++;
        a++;
        msgLen--;
      }
    }


    void build1Byte(long int numb) {
      //range +- 127

      if (numb >= 0) { //if positive first bit = 1
        binArr[binOpenSpot] = 1;
        binOpenSpot ++;
      } else {
        binArr[binOpenSpot] = 0;
        binOpenSpot ++; //if negative first bit = 0
        numb = abs(numb);
      }

      if (numb > -127 && numb < 127) {


        if (numb < 2) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 4) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 8) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 16) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 32) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }
        if (numb < 64) {
          binArr[binOpenSpot] = 0; binOpenSpot ++;
        }

      } else {
        numb = 127;
      }
      String h = "";
      int a = 0;
      h += String(numb, BIN);
      int msgLen = h.length();

      while (msgLen != 0) {

        String x = h.substring(a, a + 1);
        char d[10];
        x.toCharArray(d, 10);

        if (int(d[0]) == 49) {
          binArr[binOpenSpot] = 1;
        } else {
          binArr[binOpenSpot] = 0;
        }

        binOpenSpot++;
        a++;
        msgLen--;
      }
    }

    /////////////////////////////////////////////////////////////

};

masterStatus MSH; //Declare MSH


class commandBuffer {
  public:
    int commandStack[200][2];
    int openSpot;
    commandBuffer() {
      commandStack[200][2] = { -1};
      openSpot = 0;
    }
    void print() {
      //Serial formatting and Serial output
      int i = 0;
      endT = millis() + manualTimeout;
      while (i < 200 && millis() < endT) {
        if (commandStack[i][0] == -1 && commandStack[i][1] == -1) {
          break;
        }
        i++;
      }
    }
};
commandBuffer cBuf;
