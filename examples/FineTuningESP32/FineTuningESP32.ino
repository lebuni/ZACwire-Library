#include <ZACwire.h>

ZACwire<14> Sensor(206,140,10,1);		// set pin "2" to receive signal from the TSic "206" with an expected bitWindow of "140µs" and an offset of "10µs". ISR executed on CPU1

void setup() {
  Serial.begin(500000);
  
  if (Sensor.begin() == true) {     //check if a sensor is connected to the pin
    Serial.println("Signal found on pin 2");
  }
  else Serial.println("No Signal");
  delay(120);
}

void loop() {
  float Input = Sensor.getTemp();     //get the Temperature in °C
  
  if (Input == 222) {
    Serial.println("Reading failed");
  }
  
  else {
    Serial.print("Temp: ");
    Serial.println(Input);
  }
  delay(100);
}
