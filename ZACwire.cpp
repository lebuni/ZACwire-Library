/*	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	created by Adrian Immer in 2020
	v2.0.0
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


ZACwire::ZACwire(uint8_t pin, int16_t sensor) : _pin{pin}, _sensor{sensor} {
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


float ZACwire::getTemp(uint8_t maxChangeRate, bool useBackup) {	//return temperature in °C
	if (connectionCheck()) adjustBitThreshold();
	else return errorNotConnected;
	
	if (bitCounter != Bit::finished) useBackup = true;	//when newer reading is incomplete
	uint16_t temp{rawData[backUP^useBackup]};		//get rawData from ISR
	
	if (tempCheck(temp)) {
		temp >>= 1;					//delete second parity bit
		temp = (temp >> 2 & 1792) | (temp & 255);	//delete first    "     "
		if (!prevTemp) prevTemp = temp;
		int16_t grad = (temp - prevTemp) / (heartbeat|1);//grad is [°C/s]
		if (abs(grad) < maxChangeRate) {		//limit change rate to detect misreadings
			prevTemp = temp;
			heartbeat = 0;
			if (_sensor < 400) return ((temp * 250L >> 8) - 499) / 10.0; //use different formula for 206&306 or 506
			else return ((temp * 175L >> 9) - 99) / 10.0;
		}
	}
	return useBackup ? errorMisreading : getTemp(maxChangeRate,!useBackup); //restart with backUP rawData or return error value 222
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
		rawData[backUP] = measuredTimeDiff<<1 | 2;	//send measuredTimeDiff for calculating bitThreshold and add prefix "10" to temp
	}
	else {
		if (bitCounter == Bit::afterStop) microtime += bitThreshold>>2;	//convert timestamp at stop bit to normal 0 bit
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


void ZACwire::adjustBitThreshold() {
	if (bitCounter < Bit::lastZero || bitCounter > Bit::afterStop) return;	//adjust bitThreshold only  before rawData is used for temperature
	
	uint16_t newBitThreshold = (rawData[backUP] >> (bitCounter - 4));//separate newBitThreshold from temperature bits
	newBitThreshold *=  1.25 / 5.25;
	bitThreshold < newBitThreshold ? ++bitThreshold : --bitThreshold;
}


bool ZACwire::tempCheck(uint16_t rawTemp) {					
	if (rawTemp >> 14 != 2 || rawTemp & 512) return false;	//check for prefix "10" and stop bit=0
	
	bool parity{true};
	for (uint8_t i{0}; i<9; ++i) parity ^= rawTemp & 1 << i;
	if (parity) for (uint8_t i{10}; i<14; ++i) parity ^= rawTemp & 1 << i;
	return parity;
}
