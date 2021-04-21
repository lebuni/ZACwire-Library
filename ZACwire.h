/*	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	created by Adrian Immer in 2020
	v1.3.2b4
*/

#ifndef ZACwire_h
#define ZACwire_h

#include "Arduino.h"

template <uint8_t pin>
class ZACwire {

	public:

		ZACwire(int sensor = 306, byte defaultBitWindow = 125, bool core = 1){
		_sensor = sensor;
		bitWindow = defaultBitWindow + range/2;					//expected BitWindow in µs, depends on sensor & temperature
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
			if (++heartbeat > 3) {						//check wire connection
				if (byteTime) return 221;				//use byteTime to prove that ISR was already attached
				begin();
				delay(110);
			}
			
			static bool useBackup, pariError;
			if (bitCounter != 19) useBackup = true;				//when newer reading is incomplete
			uint16_t temp = rawTemp[backUP^useBackup^pariError];		//get rawTemp from ISR
			
			bool parity = true;
			for (byte i=0; i<14; ++i) parity ^= temp & 1 << i;		//check parity
			
			if (parity & temp >> 15) {					//check most significant bit
			
				byte newBitWindow = (((byteTime << 1) + byteTime) >> 5) + range/2;	//divide by around 10.5 and add half range
				bitWindow < newBitWindow ? ++bitWindow : --bitWindow;	//adjust bitWindow
				
				temp >>= 1;						//delete last parity bit
				temp = (temp >> 2 & 1792) | (temp & 255);		//delete first  "     "
				useBackup = pariError = false;
				return (_sensor < 400 ? (temp * 250L >> 8) - 499 : (temp * 175L >> 9) - 99) / 10.0;	//use different formula for 206&306 or 506
			}
			useBackup &= pariError = !pariError;				//reset pariError after use
			return pariError ? getTemp(): 222;				//restart with backUP raw temperature or return error
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
					bitCounter = heartbeat = 0;			//give a sign of life to getTemp()
				}
				else if (bitCounter == 5) rawTemp[backUP] = 2;		//set most significant bit 1, other 0
				else {
					if (bitCounter == 10) {				//stop bit
						byteTime = microtime - byteTime;
						microtime += range/2;			//convert timestamp to normal 0 bit
					}
					rawTemp[backUP] <<= 1;
					rawTemp[backUP] |= deltaTime + (range & -!(rawTemp[backUP] & 2)) < bitWindow;	//add range if previous bit was 1
				}
				deltaTime = microtime;
			}
		}

		int _sensor;
		bool _core;
		static byte bitWindow;
		static byte isrPin;
		static const byte range = 62;
		static volatile byte bitCounter;
		static volatile unsigned int byteTime;
		static volatile bool backUP;
		static volatile uint16_t rawTemp[2];
		static volatile byte heartbeat;
};

template<uint8_t pin>
byte ZACwire<pin>::bitWindow;
template<uint8_t pin>
byte ZACwire<pin>::isrPin;
template<uint8_t pin>
volatile byte ZACwire<pin>::bitCounter;
template<uint8_t pin>
volatile unsigned int ZACwire<pin>::byteTime;
template<uint8_t pin>
volatile bool ZACwire<pin>::backUP;
template<uint8_t pin>
volatile uint16_t ZACwire<pin>::rawTemp[2];
template<uint8_t pin>
volatile byte ZACwire<pin>::heartbeat;

#endif
