/*	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	created by Adrian Immer in 2020
	v2.0.0b1
*/

#ifndef ZACwire_h
#define ZACwire_h

#include "Arduino.h"

#if defined(ESP32) || defined(ESP8266)
	#define STATIC
	#define INITNULL = 0
#else
	#define IRAM_ATTR
	#define STATIC static
	#define INITNULL
#endif

class ZACwire {
	public:
		ZACwire(uint8_t inputPin, int16_t sensor=306);
		
		bool begin(uint8_t defaultBitWindow=125);
		
		float getTemp();
		
		uint8_t initDetectBitwindow();
		
		void end();
		
	private:
		const uint8_t maxChangeRate = 20;	//change in °C/s to detect failure
		const uint8_t startupTime = 5;		//time in min after start, in where the code is allowed to re-measure the bitwindow
		const uint8_t timeout = 220;		//timeout in ms to give error 221
		const uint8_t errorNotConnected = 221;
		const uint8_t errorMisreading = 222;
		
		STATIC void IRAM_ATTR read();
		
		bool connectionCheck();
		
		void adjustBitWindow();
		
		bool tempCheck(uint16_t &rawTemp);
		
		uint8_t _pin;
		int16_t _sensor;
		uint16_t timeLastHB;
		bool useBackup;
		int16_t prevTemp = 0;
		STATIC uint16_t deltaTime;
		STATIC uint8_t bitWindow INITNULL;
		STATIC volatile uint8_t bitCounter INITNULL;
		STATIC volatile uint8_t heartbeat INITNULL;
		STATIC volatile bool backUP;
		STATIC volatile uint16_t rawData[2];
};

#undef STATIC
#undef INITNULL

#endif
