/*	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	created by Adrian Immer in 2020
	v2.0.0
*/

#ifndef ZACwire_h
#define ZACwire_h

#include "Arduino.h"

#if defined(ESP32) || defined(ESP8266)
	#define STATIC
#else
	#define IRAM_ATTR
	#define STATIC static
#endif

class ZACwire {
	public:
		explicit ZACwire(uint8_t inputPin, int16_t sensor=306);
		
		bool begin();					//start reading
		
		float getTemp(uint8_t maxChangeRate=10, bool useBackup=false);	//return temperature in °C
		
		void end();
		
	private:
		const uint8_t timeout		{100};		//timeout in ms to give error 221
		const uint8_t errorNotConnected	{221};
		const uint8_t errorMisreading	{222};
		enum Bit {lastZero=5, afterStop=10, finished=19};
		
		static void IRAM_ATTR isrHandler(void* ptr);
		
		STATIC void IRAM_ATTR read();			//ISR to record every rising edge
		
		bool connectionCheck();				//check heartbeat of the ISR
		
		void adjustBitThreshold();			//improve the accuracy of the bitThreshold over time

		bool tempCheck(uint16_t rawTemp);		//validate the received temperature

		uint8_t _pin;
		int16_t _sensor;
		uint32_t timeLastHB{};				//timestamp, when ISR stopped sending heartbeats
		int16_t prevTemp{};				//to calculate the change rate

		STATIC uint16_t measuredTimeDiff;		//measured time between rising edges of signal
		STATIC uint8_t bitThreshold;
		STATIC volatile uint8_t bitCounter;
		STATIC volatile uint8_t heartbeat;
		STATIC volatile bool backUP;
		STATIC volatile uint16_t rawData[2];
};

#undef STATIC

#endif
