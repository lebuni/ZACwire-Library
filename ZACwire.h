/*
	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	This arduino library uses interrupts and one should use noInterrupts() or ZACwire::end() before time critical tasks. Also this library won't work if ISRs are deactivated.
	
	created by Adrian Immer
*/


#ifndef ZACwire_h
#define ZACwire_h

#include "Arduino.h"

template <byte pin>
class ZACwire {
	
  public:
  
  	ZACwire(int Sensortype = 306){
  		_Sensortype = Sensortype;
  	}
	
    bool begin() {		//start reading sensor, needs to be called 100ms before the first getTemp()
	  pinMode(pin, INPUT);
	  window = 117;
	  microtime = micros();
	  if (!pulseInLong(pin, LOW)) return false;
	  isrPin = digitalPinToInterrupt(pin);
	  if (isrPin == -1) return false;
	  attachInterrupt(isrPin, read, RISING);
	  return true;
  	}
  
	float getTemp() {	    //gives back temperature in °C
		byte parity1 = 0, parity2 = 0, timeout = 10;
		while (BitCounter && --timeout) delay(1);
		noInterrupts();  							//no ISRs because tempValue might change during reading
		uint16_t tempHigh = tempValue[0];		//get high significant bits from ISR
		uint16_t tempLow = tempValue[1];		//get low significant bits from ISR
		byte newWindow = (ByteTime << 5) + (ByteTime << 4) + ByteTime >> 9;
		if (abs(window-newWindow) < 20) window += (newWindow >> 3) - (window >> 3);	//adjust window time, which varies with rising temperature
		interrupts();
		for (byte i = 0; i < 9; ++i) {
		  if (tempHigh & (1 << i)) ++parity1;
		  if (tempLow & (1 << i)) ++parity2;
		}
		if (timeout && tempHigh | tempLow && ~(parity1 | parity2) & 1) {       // check for errors
			tempHigh >>= 1;               // delete parity bits
			tempLow >>= 1;
			tempLow += tempHigh << 8;	//joints high and low significant figures
			if (_Sensortype < 400) return (float(tempLow * 250L >> 8) - 499) / 10;	//calculates °C
			else return (float(tempLow * 175L >> 9) - 99) / 10;
		}
		else return 222;	//set to 222 if reading failed
  	}
  
  	void end() {			//stop reading -> for time critical tasks
  		for (byte timeout = 10; BitCounter && timeout; --timeout) delay(1);
  		detachInterrupt(isrPin);
  	}

  private:
  
  	static void ICACHE_RAM_ATTR read() {
  		microtime = micros() - microtime;
  		if (microtime > 1000) {		  //begin reading
  			ByteTime = micros();
  			BitCounter = 1;
  			ByteNr = tempValue[0] = tempValue[1] = 0;
  		}
  		if (BitCounter) {		//gets called with every new bit on rising edge
  			if (++BitCounter + (ByteNr * 2) == 12) {
  				BitCounter = !ByteNr;
  				if (!ByteNr) {			//after stop bit
  					microtime += window << 1;
  					ByteNr = 1;
  					ByteTime = micros() - ByteTime;
  				}
  			}
  			tempValue[ByteNr] <<= 1;
  			if (microtime > window + 24);		//Logic 0
  			else if (microtime < window - 24 || tempValue[ByteNr] & 2) tempValue[ByteNr] |= 1;	//Logic 1
  		}
  		microtime = micros();
  	}
  	static int isrPin;
  	int _Sensortype;
  	static volatile byte BitCounter;
  	static volatile unsigned long ByteTime;
  	static volatile uint16_t tempValue[2];
  	static unsigned long microtime;
  	static byte window;
  	static bool ByteNr;
};

template<byte pin>
int ZACwire<pin>::isrPin;
template<byte pin>
volatile byte ZACwire<pin>::BitCounter;
template<byte pin>
volatile unsigned long ZACwire<pin>::ByteTime;
template<byte pin>
volatile uint16_t ZACwire<pin>::tempValue[2];
template<byte pin>
unsigned long ZACwire<pin>::microtime;
template<byte pin>
byte ZACwire<pin>::window;
template<byte pin>
bool ZACwire<pin>::ByteNr;

#endif
