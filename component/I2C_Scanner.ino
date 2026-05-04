/*
 * I2C Scanner Sketch
 * Use this to find the I2C addresses of your OLED displays
 * Upload this sketch, open the Serial Monitor at 9600 baud, and it will show found devices
 */

#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  delay(1000);
  Serial.println("I2C Scanner Starting...");
}

void loop() {
  int nDevices = 0;
  
  Serial.println("Scanning I2C devices...");
  
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (i < 16) Serial.print("0");
      Serial.print(i, HEX);
      Serial.println();
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.print("Found ");
    Serial.print(nDevices);
    Serial.println(" device(s)");
  }
  
  delay(5000); // Scan every 5 seconds
}
