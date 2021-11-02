/*	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	created by Adrian Immer in 2020
	v2.0.0b2
*/

#ifndef ZACwire_h
#define ZACwire_h

#include "Arduino.h"

#if defined(ESP32) || defined(ESP8266)
	#define STATIC
	#define INITNULL {0}
#else
	#define IRAM_ATTR
	#define STATIC static
	#define INITNULL
#endif

class ZACwire {
	public:
		ZACwire(uint8_t inputPin, int16_t sensor=306);
		
		bool begin(uint8_t customBitWindow=0);		//start reading
		
		float getTemp();				//return temperature in °C
				
		void end();
		
	private:
		const uint8_t maxChangeRate	{20};		//change in °C/s to detect failure
		const uint8_t startupTime	{5};		//time in min after start, in where the code is allowed to re-measure the bitwindow
		const uint8_t timeout		{220};		//timeout in ms to give error 221
		const uint8_t errorNotConnected	{221};
		const uint8_t errorMisreading	{222};
		const uint8_t defaultBitWindow	{125};		//125µs according to datasheet
		enum Bit {lastZero=5, afterStop=10, finished=19};
		
		STATIC void IRAM_ATTR read();			//ISR to record every rising edge
		
		bool connectionCheck();				//check heartbeat of the ISR
				
		bool tempCheck(uint16_t rawTemp);		//validate the received temperature

		uint8_t initDetectBitThreshold();		//detect the bitThreshold
		
		uint8_t adjustBitThreshold();			//improve the accuracy of the bitThreshold over time

		uint8_t _pin;
		int16_t _sensor;
		uint16_t timeLastHB;				//timestamp, when ISR stopped sending heartbeats
		bool useBackup;					//to use backup rawData
		int16_t prevTemp{0};				//to calculate the change rate

		STATIC uint16_t measuredTimeDiff;		//measured time between rising edges of signal
		STATIC uint8_t bitThreshold;
		STATIC volatile uint8_t bitCounter INITNULL;
		STATIC volatile uint8_t heartbeat INITNULL;
		STATIC volatile bool backUP;
		STATIC volatile uint16_t rawData[2];
};

#undef STATIC
#undef INITNULL

#endif
