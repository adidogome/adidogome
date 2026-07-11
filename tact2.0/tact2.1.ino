#include "tact2.0.ino"

Touch rows = Touch(); //new touch instance

int touchtype;

void setup(){
  rows.setPins(A3, A2, A11, A7, A9, A10);
  rows.initializeTouch(1);
  Serial.begin(115200);
}

void loop() {
  touchtype = rows.detect_touchFromNoise(0);
  if (touchtype == 1){
    Serial.println("Singletap");
    }
    else if (touchtype == 2) {
      Serial.println("Doubletap");
    }
    else if (touchtype == 3) {
      Serial.println("Shortpress");
    }
    else if (touchtype == 4) {
      Serial.println("longpress");
    }
     else if (touchtype == 5) {
      Serial.println("extralongpress");
    }
     else if (touchtype == -1) {
      Serial.println("detecting");
    }
  }
  




