# ZACwire™ Library to read TSic sensors
[![Only 32 Kb](https://badge-size.herokuapp.com/lebuni/ZACwire-Library/master/ZACwire.h)](https://github.com/lebuni/ZACwire-Library/blob/master/ZACwire.h) 
[![GitHub issues](https://img.shields.io/github/issues/lebuni/ZACwire-Library.svg)](https://github.com/lebuni/ZACwire-Library/issues/) 
[![GitHub license](https://img.shields.io/github/license/lebuni/ZACwire-Library.svg)](https://github.com/lebuni/ZACwire-Library/blob/master/LICENSE)


Arduino Library to read the ZACwire protocol, wich is used by TSic temperature sensors 206, 306 and 506 on their signal pin.

`ZACwire obj(int pin, int Sensor)` tells the library which input pin of the controller (eg. 2) and type of sensor (eg. 306) it should use. Please pay attention that the selected pin supports external interrupts!

`.begin()` returns true if a signal is detected on the specific pin and starts the reading via ISRs. It should be started at least 2ms before the first .getTemp().

`.getTemp()` returns the temperature in °C and gets usually updated every 100ms. In case of a failed reading, it returns `222`. In case of no incoming signal it returns `221`.

`.end()` stops the reading for time sensititive tasks, which shouldn't be interrupted.


## Benefits compared to former TSic libraries
- saves a lot of controller time, because no delay() is used and calculations are done by bit manipulation
- low memory consumption
- misreading rate lower than 0.001%
- reading an unlimited number of TSic simultaneously (except AVR boards)
- higher accuracy (0.1°C offset corrected)
- simple use






## Example
```c++

#include <ZACwire.h>

ZACwire Sensor(2, 306);		// set pin "2" to receive signal from the TSic "306"

void setup() {
  Serial.begin(500000);
  
  if (Sensor.begin() == true) {     //check if a sensor is connected to the pin
    Serial.println("Signal found on pin 2");
  }
  delay(2);
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
```



## Wiring
Connect V+ to a power supply with 3.0V to 5.5V. For most accurate results connect it to 5V, because that's the voltage the sensor was calibrated with.

The output of the signal pin switches between GND and V+ to send informations, so take care that your µC is capable of reading both V+ and GND.

![TSIC](https://user-images.githubusercontent.com/62163284/116116897-f5ed5900-a6bb-11eb-95b8-ba8f4ef129cc.png)



## Fine-Tuning
Some optional features, that might be interesting for playing around:

```c++
void getTemp(uint8_t maxChangeRate)
```
`uint8_t maxChangeRate` is measured in °C/s and the default value is 10 °C/s. If you have a very stable system, you can lower that value to help the library detecting outliers. If the maxChangeRate is exceeded, a backup value from 100ms before will be used or error 222 will be returned.

When your system changes temperature really quickly and due to the exceeded maxChangeRate the output of .getTemp() is 222, feel free to increase the value.

If .getTemp() gives you **221** as an output, the library detected an unusual long period above 255ms without new signals. Please check your cables or try using the RC filter, that is mentioned in the [application note of the TSic](https://www.ist-ag.com/sites/default/files/attsic_e.pdf).
