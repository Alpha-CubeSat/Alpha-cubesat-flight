void powerMan() { //how much power we use on uplink? //balances RB usage during normal operations
  //there sould be two values for battery. the one in bits and one in volts, since volts is less accurate, we should use the bits
  //Scale goes from 0 volts to max (4.1 for now, please check with flight battery how far it can go with normal charge) This means it 3.7 = 230; 3.8 = 236; 3.9 = 242
  uint8_t i = MSH.Battery; //deleteme
  if ((MSH.OVERWRITE)) { //only works when OVERWRITE = true
    //OVERWRITE IS TRUE //U BETTA KNOW WHATCHU DUIN
    //int overdown = ; //downlink period from popcomands
    //int overup = ; //uplink period from popcommands

    //an alpha cycle has size equal to period of downlink and uplink (5 min each)
    //so cycle length with delays below = period + 10minutes
    //maybe increase these numbers

    //maybe have a timer on how long the overwrite mode can last
    //or just check battery voltage, if too low, then overwrite cannot actually overwrite
    //battery cutoff for this should be 3.3V - but test with everything on and log data
    //keep flatsat on for 1hr, substitute power supply with lower voltages until you notice something is wrong
    //compare data as you go down to the original data at 4.0V battery
    switch (MSH.overdown) { //takes value given by popcommands for downlink Period
      case 5:
        MSH.downlinkPeriod = 1800000;//30 min
        Serial.println("Downlink 30 mins!");
        break;

      case 4:
        MSH.downlinkPeriod = 600000;//10 min
        Serial.println("Downlink 10 mins!");
        break;

      case 3:
        MSH.downlinkPeriod = 300000;//5 min
        Serial.println("Downlink 5 mins!");
        break;

      case 2:
        MSH.downlinkPeriod = 60000;//1 min
        Serial.println("Downlink 1 min!");
        break;

      case 1: //max set to 110 incase the range i set up is a little off
        MSH.downlinkPeriod = 5000;// 5 sec
        Serial.println("Downlink 5 secs!");
        break;

    }

    switch (MSH.overup) { //takes value given by popcommands for uplink Period
      case 5: //3.8V-3.85V
        MSH.uplinkPeriod = 1800000;//30 min
        Serial.println("Uplink 30 mins!");
        break;

      case 4:
        MSH.uplinkPeriod = 600000;//10 min
        Serial.println("Uplink 10 mins!");
        break;

      case 3:
        MSH.uplinkPeriod = 300000;//5 min
        Serial.println("Uplink 5 mins!");
        break;

      case 2:
        MSH.uplinkPeriod = 60000;//1 min
        Serial.println("Uplink 1 min!");
        break;

      case 1: //max set to 110 incase the range i set up is a little off
        MSH.uplinkPeriod = 5000;// 5 sec
        Serial.println("Uplink 5 secs!");
        break;

    }

  } else {  //else for overwrite state //normal operations

    //these numbers are the direct readings
    if (i <= 178) { // i comes from voltage read //redo this conflicting functions
      nextMode = 4; // enter low power if under 3.8V
      //dlfreq minimum maybe an hour?

    } else if (nextMode != 4) { //else in case we have enough power and we are not in lowPower Mode!

      switch (i) { //scales down with voltage that we have
        case 178 ... 192: //3.8V-3.85V
          MSH.downlinkPeriod = 1800000;//30 min
          Serial.println("Downlink 30 mins!");
          break;

        case 193 ... 204: //3.86V-3.9V
          MSH.downlinkPeriod = 5000;//10 min //for testing
          Serial.println("Downlink 5seconds! change me after done testing");
          break;

        case 205 ... 255: //3.9V-4.1V
          MSH.downlinkPeriod = 5000;//1 min
          Serial.println("Downlink 5 seconds!"); //if battery is charged, we are trying as much as we can. we can still choose different modes by manual commands
          break;

          //      case 81 ... 90:
          //        MSH.downlinkPeriod = 60000;//1 min
          //        Serial.println("Downlink 1 min!");
          //        break;
          //
          //      case 91 ... 110: //max set to 110 incase the range i set up is a little off
          //        MSH.downlinkPeriod = 5000;// 5 sec
          //        Serial.println("Downlink 5 secs!");
          //        break;

      }
      switch (i) { //same as above, but determines the uplink Period instead of Downlink
        case 178 ... 192: //3.8V-3.85V
          MSH.uplinkPeriod = 1800000;//30 min
          Serial.println("Uplink 30 mins!");
          break;

        case 193 ... 204: //3.86V-3.9V
          MSH.uplinkPeriod = 5000;//10 min
          Serial.println("Uplink 5 seconds! change me after done testing");
          break;

        case 205 ... 255: //3.9V-4.1V
          MSH.uplinkPeriod = 5000;//5 min
          Serial.println("Uplink 5 seconds! change me after done testing");
          break;

          //      case 81 ... 90:
          //        MSH.downlinkPeriod = 60000;//1 min
          //        Serial.println("Downlink 1 min!");
          //        break;
          //
          //      case 91 ... 110: //max set to 110 incase the range i set up is a little off
          //        MSH.downlinkPeriod = 5000;// 5 sec
          //        Serial.println("Downlink 5 secs!");
          //        break;
      }
    }
  }
}
