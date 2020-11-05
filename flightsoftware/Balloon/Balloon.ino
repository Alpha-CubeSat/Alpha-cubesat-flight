#include "Camera.h"

Camera cam = Camera(&Serial2);

void setup(){
  Serial.begin(115200);

}

void loop(){
  Serial.print(cam.setup());
  
  
  
}
