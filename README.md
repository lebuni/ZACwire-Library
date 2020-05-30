# ZACwire™ Library to read TSic sensors
Arduino Library to read the ZACwire protocol, wich is used by TSic temperature sensors 206, 306 and 506

`.begin()` returns true if a signal is detected on the specific pin and starts the reading via ISRs. It should be started at least 100ms before the first .getTemp()

`.getTemp()` returns the temperature in °C. 

`.end()` stops the interrupt routine for time critical tasks
