/*	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	created by Adrian Immer in 2020
	v1.3.2b2
*/

#ifndef ZACwire_h
#define ZACwire_h

#include "Arduino.h"

template <uint8_t pin>
class ZACwire {

	public:

		ZACwire(int sensor = 306, byte defaultBitWindow = 125, bool core = 1){
		_sensor = sensor;
		bitWindow = defaultBitWindow + (range >> 1);				//expected BitWindow in µs, depends on sensor & temperature
		_core = core;								//only ESP32: choose cpu0 or cpu1
		}

		bool begin() {								//start collecting data, needs to be called 100+ms before first getTemp()
			pinMode(pin, INPUT);
			if (!pulseInLong(pin, LOW)) return false;			//check if there is incoming signal
			isrPin = digitalPinToInterrupt(pin);
			if (isrPin == 255) return false;
			#ifdef ESP32
			xTaskCreatePinnedToCore(attachISR_ESP32,"attachISR_ESP32",800,NULL,1,NULL,_core); //freeRTOS
			#else
			attachInterrupt(isrPin, read, RISING);
			#endif
			return true;
	    }
	  
		float getTemp() {							//give back temperature in °C
			static bool misreading;
			if ((unsigned int)millis() - lastISR > 255) {			//check wire connection for the last 255ms
				if (isrPin != 255) return 221;				//only before call of begin()
				begin();
				delay(110);
			}
			
			if (bitCounter == 19) {
				byte newBitWindow = (((byteTime << 1) + byteTime) >> 5) + (range >> 1);	//divide by around 10.5 and add half range
				bitWindow < newBitWindow ? ++bitWindow : --bitWindow;	//adjust bitWindow
			}
			
			if (bitCounter != 19) misreading = true;			//use misreading-backup when newer reading is incomplete
			uint16_t temp = rawTemp[backUP^misreading];			//get rawTemp from ISR
			misreading = !misreading;					//reset misreading after use
			
			bool parity = true;
			for (byte i=0; i<13; ++i) parity ^= temp & 1 << i;		//check parity
			
			if (parity) {
				temp >>= 1;						//delete last parity bit
				temp = (temp >> 1 & 1792) | (temp & 255);		//delete first  "     "
				misreading = false;
				return (_sensor < 400 ? (temp * 250L >> 8) - 499 : (temp * 175L >> 9) - 99) / 10.0;	//use different formula for 206&306 or 506
			}
			return misreading ? getTemp(): 222;				//restart with backUP raw temperature or return error
		}


		void end() {
			detachInterrupt(isrPin);
		}

	private:  

		#ifdef ESP32
		static void attachISR_ESP32(void *arg){					//attach ISR in freeRTOS because arduino can't attachInterrupt() inside of template class
			gpio_pad_select_gpio((gpio_num_t)isrPin);
			gpio_set_direction((gpio_num_t)isrPin, GPIO_MODE_INPUT);
			gpio_set_intr_type((gpio_num_t)isrPin, GPIO_INTR_POSEDGE);
			gpio_install_isr_service(0);
			gpio_isr_handler_add((gpio_num_t)isrPin, read, NULL);
			vTaskDelete(NULL);
		}
		static void IRAM_ATTR read(void *arg) {
		#elif defined(ESP8266)
		static void ICACHE_RAM_ATTR read() {
		#else
		static void read() {							//get called with every rising edge
		#endif
			if (++bitCounter > 4) {						//first 4 bits always =0
				unsigned int microtime = micros();
				static unsigned int deltaTime;
				deltaTime = microtime - deltaTime;			//measure time to previous rising edge
				if (deltaTime >> 10) {					//start bit
					byteTime = microtime;				//measuring Tstrobe/bitWindow
					backUP = !backUP;
					bitCounter = 0;
					lastISR = millis();				//for checking wire connection
				}
				else if (bitCounter == 10) byteTime = microtime - byteTime;	//stop bit
				else {
					if (bitCounter == 6) rawTemp[backUP] = 0;
					rawTemp[backUP] <<= 1;
					rawTemp[backUP] |= deltaTime < bitWindow - (range >> (bitCounter==11) & -!(rawTemp[backUP] & 2));	//check if sent bit is either 1 or 0
				}
				deltaTime = microtime;
			}
		}

		static byte isrPin = -1;
		int _sensor;
		bool _core;
		static volatile byte bitCounter;
		static volatile unsigned int byteTime;
		static volatile uint16_t rawTemp[2];
		static byte bitWindow;
		static const byte range = 62;
		static volatile bool backUP;
		static volatile unsigned int lastISR;
};

template<uint8_t pin>
volatile byte ZACwire<pin>::bitCounter = 20;
template<uint8_t pin>
volatile unsigned int ZACwire<pin>::byteTime;
template<uint8_t pin>
volatile bool ZACwire<pin>::backUP;
template<uint8_t pin>
volatile uint16_t ZACwire<pin>::rawTemp[2];
template<uint8_t pin>
byte ZACwire<pin>::isrPin;
template<uint8_t pin>
byte ZACwire<pin>::bitWindow;
template<uint8_t pin>
volatile unsigned int ZACwire<pin>::lastISR;

#endif
