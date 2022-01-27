/*	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	created by Adrian Immer in 2020
	14bit_v1.0.0
*/

#include "ZACwire.h"
#include "Arduino.h"

#if !defined(ESP32) && !defined(ESP8266)			//initialize static variables for AVR boards
	uint8_t ZACwire::bitThreshold;
	uint16_t ZACwire::measuredTimeDiff;
	volatile uint8_t ZACwire::bitCounter;
	volatile bool ZACwire::backUP;
	volatile uint16_t ZACwire::rawData[2];
	volatile uint8_t ZACwire::heartbeat;
#endif


ZACwire::ZACwire(uint8_t pin) : _pin{pin} {
	bitCounter = 0;
	bitThreshold = 0;
}


bool ZACwire::begin() {						//start collecting data, needs to be called over 2ms before first getTemp()
	pinMode(_pin, INPUT_PULLUP);
	for (uint8_t i=20; pulseIn(_pin,LOW,500);) {		//wait for time without transmission
		if (!--i) return false;
		yield();					
	}
	uint8_t strobeTime = pulseIn(_pin, LOW);		//check for signal and measure strobeTime
	if (!strobeTime) return false;				//abort if there is no incoming signal
	measuredTimeDiff = micros();				//set timestamp of first rising edge for ISR
	bitThreshold = strobeTime * 2.5;
	
	uint8_t isrPin = digitalPinToInterrupt(_pin);
	if (isrPin == 255) return false;
	#if defined(ESP32) || defined(ESP8266)
		attachInterruptArg(isrPin, isrHandler, this, RISING);
	#else
		attachInterrupt(isrPin, read, RISING);
	#endif
	return true;
}


uint16_t ZACwire::getRawVal(bool useBackup) {	//return temperature in °C
	
	if (bitCounter != Bit::finished) useBackup = true;	//when newer reading is incomplete
	uint16_t temp{rawData[backUP^useBackup]};		//get rawData from ISR
	
	if (parityCheck(temp)) {
		temp >>= 1;					//delete second parity bit
		temp = (temp >> 1 & 16128) | (temp & 255);	//delete first    "     "
		return temp;
	}
	return useBackup ? errorMisreading : getRawVal(!useBackup); //restart with backUP rawData or return error value 222
}


void ZACwire::end() {
	detachInterrupt(digitalPinToInterrupt(_pin));
}


void ZACwire::isrHandler(void* ptr) {
    static_cast<ZACwire*>(ptr)->read();
}


void ZACwire::read() {						//get called with every rising edge
	if (++bitCounter < Bit::lastZero) return;		//first 4 bits always =0
	uint16_t microtime = micros();
	measuredTimeDiff = microtime - measuredTimeDiff;	//measure time to previous rising edge
	if (measuredTimeDiff >> 10) {				//first start bit, so big time difference
		if (bitCounter == 20) {
			backUP = !backUP; 			//save backup, if it successfully counted 20 bits
			++heartbeat;				//give a sign of life to getTemp()
		}
		bitCounter = 0;
	}
	else if (bitCounter == Bit::lastZero)  {
		rawData[backUP] = 0;		//send measuredTimeDiff for calculating bitThreshold and add prefix "10" to temp
	}
	else {
		if (bitCounter == Bit::afterStop) {
			microtime += bitThreshold>>2;		//convert timestamp at stop bit to normal 0 bit
			rawData[backUP] >>= 1;			//delete stop bit to make space for 14bit
		}
		if (~rawData[backUP] & 1) measuredTimeDiff += bitThreshold>>1;	//add 1/2 bitThreshold if previous bit was 0 (for normalisation)
		rawData[backUP] <<= 1;
		if (measuredTimeDiff < bitThreshold) rawData[backUP] |= 1;
	}
	measuredTimeDiff = microtime;
}


bool ZACwire::connectionCheck() {
	if (heartbeat) timeLastHB = 0;
	else if (!bitThreshold) {				//use bitThreshold to check if begin() was already called
		if (begin()) delay(3);
		else return false;
	}
	else if (!timeLastHB) timeLastHB = millis();		//record first missing heartbeat
	else if (millis() - timeLastHB > timeout) return false;
	return true;
}


bool ZACwire::parityCheck(uint16_t rawVal) {					
	bool parity{true};
	for (uint8_t i{0}; i<9; ++i) parity ^= rawVal & 1 << i;
	if (parity) for (uint8_t i{10}; i<14; ++i) parity ^= rawVal & 1 << i;
	return parity;
}
