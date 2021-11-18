#include <ZACwire.h>

ZACwire Sensor(14,206);		// set pin "14" to receive signal from the TSic "206"

void setup() {
  Serial.begin(500000);
  
  if (Sensor.begin(130) == true) {     //expected bitWindow of 130µs
    Serial.println("Signal found on pin 14");
  }
  else Serial.println("No Signal");
  delay(3);
}

void loop() {
  float Input = Sensor.getTemp();     //get the Temperature in °C
  
  if (Input == 222) {
    Serial.println("Reading failed");
  }
  
  else if (Input == 221) {
    Serial.println("Sensor not connected");
  }
  
  else {
    Serial.print("Temp: ");
    Serial.println(Input);
  }
  delay(100);
}
