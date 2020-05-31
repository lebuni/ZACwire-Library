# ZACwire™ Library to read TSic sensors
Arduino Library to read the ZACwire protocol, wich is used by TSic temperature sensors 206, 306 and 506

`ZACwire<uint8_t SignalPin> obj(int Sensor)` tells the library which input pin (eg. 2) and type of sensor (eg. 306) it should use

`.begin()` returns true if a signal is detected on the specific pin and starts the reading via ISRs. It should be started at least 120ms before the first .getTemp().

`.getTemp()` returns the temperature in °C and gets usually updated every 100ms. In case of a noisy signal, it returns 222

`.end()` stops the interrupt routine for time critical tasks

## Example
```c++
#include <ZACwire.h>

ZACwire<2> Sensor(306);		// set pin "2" to receive signal from the TSic "306"

void setup() {
  Serial.begin(500000);
  
  if (Sensor.begin() == TRUE) {     //check if a sensor is connected to the pin
    Serial.println("Signal found on pin 2");
  }
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
}
```
