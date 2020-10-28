void printArray(uint8_t arr[], int s) {
  if (s) {
    for (int i = 0; i < s; i++) {
      print_binary(arr[i], 8);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

void print_binary(int v, int num_places) {
  int mask = 0, n;
  for (n = 1; n <= num_places; n++) {
    mask = (mask << 1) | 0x0001;
  }
  v = v & mask;  // truncate v to specified number of places
  while (num_places) {
    if (v & (0x0001 << num_places - 1)) {
      Serial.print("1");
    } else {
      Serial.print("0");
    }
    --num_places;
  }
}

void byteread(int value) {// convert a 1 byte int into 1 uint
  uint8_t f = 0;
  f = value & 0xFF;
  txbuffer[openSpot] = f; openSpot++;
}
void twobyteread (int value) {// convert a 2 byte int into 2 uints
  uint8_t f = 0;
  f = value & 0xFF;
  txbuffer[openSpot + 1] = f;
  f = (value >> 8) & 0xFF;
  txbuffer[openSpot] = f; openSpot = (openSpot + 2);
}
void threebyteread (int value) { // convert a 3 byte int into 3 uints
  uint8_t f1 = 0;
  uint8_t f2 = 0;
  uint8_t f3 = 0;
  f1 = value & 0xFF;
  txbuffer[openSpot + 2] = f1;
  f2 = (value >> 8) & 0xFF;
  txbuffer[openSpot + 1] = f2;
  f3 = (value >> 16) & 0xFF;
  txbuffer[openSpot] = f3;
  openSpot = (openSpot + 3);
}
void printBits(uint8_t myByte) {
  for (uint8_t mask = 0x80; mask; mask >>= 1) {
    if (mask  & myByte)
      Serial.print('1');
    else
      Serial.print('0');
  }
}

String Hash_base64( uint8_t *in, int hashlength) {  //only works on strings up to 5120bytes?
  int i, out;
  char b64[(int)(hashlength * (8 / 6.0) + 1)]; // working byte array for sextets....
  String base64;
  for (i = 0, out = 0 ;; in += 3) { // octets to sextets
    i++;
    b64[out++] = b64chars[in[0] >> 2];

    if (i >= hashlength ) { // single byte, so pad two times
      b64[out++] = b64chars[((in[0] & 0x03) << 4) ];
      b64[out++] =  '=';
      b64[out++] =  '=';
      break;
    }

    b64[out++] = b64chars[((in[0] & 0x03) << 4) | (in[1] >> 4)];
    i++;
    if (i >= hashlength ) { // two bytes, so we need to pad one time;
      b64[out++] =  b64chars[((in[1] & 0x0f) << 2)] ;
      b64[out++] =  '=';
      break;
    }
    b64[out++] = b64chars[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
    b64[out++] =   b64chars[in[2] & 0x3f];

    i++;
    if (i >= hashlength ) { // three bytes, so we need no pad - wrap it;
      break;
    }
    //Serial.println(out);
  } // this should make b64 an array of sextets that is "out" in length
  b64[out] = 0;
  base64 = b64;
  return (base64);
}
