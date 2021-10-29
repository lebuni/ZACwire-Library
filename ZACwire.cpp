/*	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	created by Adrian Immer in 2020
	v2.0.0b1
*/

#include "Arduino.h"
#include "ZACwire.h"

#if defined(ESP32) || defined(ESP8266)
	#include <FunctionalInterrupt.h>
#endif


ZACwire::ZACwire(uint8_t inputPin, int16_t sensor) {
	_pin = inputPin;
	_sensor = sensor;
}


bool ZACwire::begin(uint8_t defaultBitWindow) {				//start collecting data, needs to be called 100+ms before first getTemp()
	bitWindow = defaultBitWindow + (defaultBitWindow>>2);		//expected BitWindow in µs, depends on sensor & temperature
	pinMode(_pin, INPUT);
	if (!pulseInLong(_pin, LOW)) return false;			//check if there is incoming signal
	uint8_t isrPin = digitalPinToInterrupt(_pin);
	if (isrPin == 255) return false;
	#if defined(ESP32) || defined(ESP8266)
		attachInterrupt(isrPin, [this]{read();}, RISING);
	#else
		attachInterrupt(isrPin, read, RISING);
	#endif
	return true;
}

	
float ZACwire::getTemp() {						//return temperature in °C
	if (connectionCheck()) adjustBitWindow();
	else return errorNotConnected;
	
	if (bitCounter != 19) useBackup = true;				//when newer reading is incomplete
	uint16_t temp = rawData[backUP^useBackup];			//get rawData from ISR
	
	if (tempCheck(temp)) {
		temp >>= 1;						//delete second parity bit
		temp = (temp >> 2 & 1792) | (temp & 255);		//delete first    "     "
		if (!prevTemp) prevTemp = temp;
		int16_t grad ((temp - prevTemp) / (heartbeat|1));	//grad is [°C/s]
		if (abs(grad) < maxChangeRate) {			//limit change rate to detect misreadings
			prevTemp = temp;
			heartbeat = useBackup = 0;
			if (_sensor < 400) return ((temp * 250L >> 8) - 499) / 10.0;	//use different formula for 206&306 or 506
			else return ((temp * 175L >> 9) - 99) / 10.0;
		}
	}
	useBackup = !useBackup;						//reset useBackup after use
	if (millis() < startupTime*60*1000) initDetectBitwindow();	//determine bitwindow in the first minutes of use in case of failure
	return useBackup ? getTemp() : errorMisreading;			//restart with backUP rawData or return error value 222
}


uint8_t ZACwire::initDetectBitwindow() {
	pinMode(_pin, INPUT);
	uint32_t deadline = millis() + timeout;				//abort detection after 200ms
	while (pulseIn(_pin, LOW, 300) != 0) yield();			//detect a transmission break without signal
	while (digitalRead(_pin) == HIGH && deadline > millis()) yield();//wait for next signal
	
	uint32_t duration = micros();					//measure time for 12 bitwindows
	for (uint8_t i=0; i<11; ++i) {
		while (digitalRead(_pin) == LOW && deadline > millis()) yield();
		while (digitalRead(_pin) == HIGH && deadline > millis()) yield();
	}
	duration = micros() - duration;
	
	if (deadline > millis()) {
		bitWindow = (duration + duration/4) / 12;
		return duration/12;
	}
	else return 0;
}


void ZACwire::end() {
	detachInterrupt(digitalPinToInterrupt(_pin));
}


void ZACwire::read() {							//get called with every rising edge
	if (++bitCounter > 4) {						//first 4 bits always =0
		uint16_t microtime = micros();
		deltaTime = microtime - deltaTime;			//measure time to previous rising edge
		if (deltaTime >> 10) {					//start bit
			backUP ^= bitCounter == 20;			//save backup, if it successfully counted 20 bits
			bitCounter = 0;
			++heartbeat;					//give a sign of life to getTemp()
		}
		else if (bitCounter == 5) rawData[backUP] = deltaTime<<1 | 2;	//send deltaTime for calculating bitWindow and add prefix "10" to temp
		else {
			if (bitCounter == 10) microtime += bitWindow>>2;	//convert timestamp at stop bit to normal 0 bit
			if (~rawData[backUP] & 1) deltaTime += bitWindow>>1;	//add 1/2 bitWindow if previous bit was 0 (for normalisation)
			rawData[backUP] <<= 1;
			rawData[backUP] |= bitWindow > deltaTime;
		}
		deltaTime = microtime;
	}
}


bool ZACwire::connectionCheck() {
	if (heartbeat) timeLastHB = 0;
	else {
		if (!bitCounter) {					//use bitCounter to check if begin() was already called
			begin();
			delay(110);
		}
		else if (!timeLastHB) timeLastHB = millis();			//record first missing heartbeat
		else if ((uint16_t)millis() - timeLastHB > timeout) return false;
	}
	return true;
}		


void ZACwire::adjustBitWindow() {
	if (bitCounter > 4 && bitCounter < 11) {			//adjust bitWindow just before rawData is filled with temperature
		uint8_t newBitWindow = rawData[backUP] >> (bitCounter - 2);	//seperate newBitWindow from temperature bits and divide by 4
		newBitWindow = (((newBitWindow>>1) + newBitWindow) >> 1) + 4 + (bitWindow>>2);	//divide by 1.31 (w/ previous line: by 5.25) and add 1/4 bitWindow
		bitWindow < newBitWindow ? ++bitWindow : --bitWindow;
	}
}


bool ZACwire::tempCheck(uint16_t &rawTemp) {					
	if (rawTemp >> 14 == 2 && ~rawTemp & 512) {			//check for prefix "10" and stop bit
		bool parity = true;
		for (uint8_t i=0; i<9; ++i) parity ^= rawTemp & 1 << i;
		if (parity) for (uint8_t i=10; i<14; ++i) parity ^= rawTemp & 1 << i;
		return parity;
	}
	return false;
}


#if !defined(ESP32) && !defined(ESP8266)
uint8_t ZACwire::bitWindow;
uint16_t ZACwire::deltaTime;
volatile uint8_t ZACwire::bitCounter;
volatile bool ZACwire::backUP;
volatile uint16_t ZACwire::rawData[2];
volatile uint8_t ZACwire::heartbeat;
#endif
