# ZACwire™ Raw Library
[![Only 32 Kb](https://badge-size.herokuapp.com/lebuni/ZACwire-Library/master/ZACwire.cpp)](https://github.com/lebuni/ZACwire-Library/blob/master/ZACwire.cpp) 
[![GitHub issues](https://img.shields.io/github/issues/lebuni/ZACwire-Library.svg)](https://github.com/lebuni/ZACwire-Library/issues/) 
[![GitHub license](https://img.shields.io/github/license/lebuni/ZACwire-Library.svg)](https://github.com/lebuni/ZACwire-Library/blob/master/LICENSE)


Experimental code to read the raw value of all sensors using the ZACwire protocol, including 14bit data packages.

`ZACwire obj(int pin)` tells the library which input pin of the controller (eg. 2) it should use. Please pay attention that the selected pin supports external interrupts!

`.begin()` returns true if a signal is detected on the specific pin and starts the reading via ISRs. It should be started at least 2ms before the first .getTemp().

`.getRawVal()` returns the temperature in °C and gets usually updated every 100ms. In case of a failed reading, it returns `222`. In case of no incoming signal it returns `221`.

`.end()` stops the reading for time sensititive tasks, which shouldn't be interrupted.


## Example
```c++

#include <ZACwire.h>

ZACwire Sensor(2);		// set pin "2" to receive signal

void setup() {
  Serial.begin(500000);
  
  if (Sensor.begin() == true) {     //check if a sensor is connected to the pin
    Serial.println("Signal found on pin 2");
  }
  delay(2);
}

void loop() {
  float Input = Sensor.getRawVal();     //get the raw value
  
  if (Input == 222) {
    Serial.println("Reading failed");
  }
  
  else if (Input == 221) {
    Serial.println("Sensor not connected");
  }
  
  else {
    Serial.print("raw value: ");
    Serial.println(Input);
  }
  delay(100);
}
```

## Connection check
If .getTemp() gives you **221** as an output, the library detected an unusual long period above 255ms without new signals. Please check your cables or try using the RC filter, that is mentioned in the [application note of the TSic](https://www.ist-ag.com/sites/default/files/attsic_e.pdf).
