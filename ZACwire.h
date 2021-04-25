/*	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	created by Adrian Immer in 2020
	v1.3.2
*/

#ifndef ZACwire_h
#define ZACwire_h

#include "Arduino.h"

template <uint8_t pin>
class ZACwire {

	public:

		ZACwire(int sensor = 306, byte defaultBitWindow = 125, bool core = 1){
			_sensor = sensor;
			bitWindow = defaultBitWindow + (defaultBitWindow>>2);		//expected BitWindow in µs, depends on sensor & temperature
			_core = core;							//only ESP32: choose cpu0 or cpu1
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
	  
		float getTemp() {							//return temperature in °C
			static unsigned int lastHB;
			if (!heartbeat) {						//check wire connection
				if (isrPin == 255) {					//use isrPin to prove that ISR was already attached
					begin();
					delay(110);
				}
				else if (!lastHB) lastHB = millis();			//record first missing heartbeat
				else if ((unsigned int)millis() - lastHB > 255) return 221;	//return 221 after timeout of 255ms
			}
			else {
				lastHB = 0;
				if (bitCounter > 4 && bitCounter < 11) {		//adjust bitWindow
					byte newBitWindow = rawData[backUP] >> (bitCounter - 2);	//seperate newBitWindow from temperature bits and divide by 4
					newBitWindow = (((newBitWindow>>1) + newBitWindow) >> 1) + 4 + (bitWindow>>2);	//divide by 1.31 (w/ previous line: by 5.25) and add 1/4 bitWindow
					bitWindow < newBitWindow ? ++bitWindow : --bitWindow;
				}
			}
			
			static bool useBackup;
			if (bitCounter != 19) useBackup = true;				//when newer reading is incomplete
			uint16_t temp = rawData[backUP^useBackup];			//get rawData from ISR
			
			bool parity = true;
			for (byte i=0; i<9; ++i) parity ^= temp & 1 << i;		//check parity
			if (parity) for (byte i=10; i<14; ++i) parity ^= temp & 1 << i;
			
			
			if (parity && temp >> 14 == 2 && ~temp & 512) {			//three factor check: parity, prefix "10", stop bit
				temp >>= 1;						//delete second parity bit
				temp = (temp >> 2 & 1792) | (temp & 255);		//delete first    "     "
				static int prevTemp = temp;
				int grad ((temp - prevTemp) / (heartbeat|1));		//grad is [°C/s]
				if (abs(grad) < 20) {					//limit change rate to 20°C/s
					prevTemp = temp;
					heartbeat = useBackup = 0;
					return (_sensor < 400 ? (temp * 250L >> 8) - 499 : (temp * 175L >> 9) - 99) / 10.0;	//use different formula for 206&306 or 506
				}
			}
			useBackup = !useBackup;						//reset useBackup after use
			return useBackup ? getTemp(): 222;				//restart with backUP rawData or return error
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
					backUP ^= bitCounter == 20;
					bitCounter = 0;
					++heartbeat;					//give a sign of life to getTemp()
				}
				else if (bitCounter == 5) rawData[backUP] = deltaTime<<1 | 2;	//send deltaTime for calculating bitWindow and add prefix "10" to temp
				else {
					if (bitCounter == 10) microtime += bitWindow>>2;	//convert timestamp at stop bit to normal 0 bit
					rawData[backUP] <<= 1;
					rawData[backUP] |= deltaTime + (bitWindow>>1 & -!(rawData[backUP] & 2)) < bitWindow;	//add 1/2 bitWindow if previous bit was 1
				}
				deltaTime = microtime;
			}
		}

		int _sensor;
		bool _core;
		static byte bitWindow;
		static byte isrPin;
		static volatile byte bitCounter;
		static volatile bool backUP;
		static volatile uint16_t rawData[2];
		static volatile byte heartbeat;
};

template<uint8_t pin>
byte ZACwire<pin>::bitWindow;
template<uint8_t pin>
byte ZACwire<pin>::isrPin = 255;
template<uint8_t pin>
volatile byte ZACwire<pin>::bitCounter;
template<uint8_t pin>
volatile bool ZACwire<pin>::backUP;
template<uint8_t pin>
volatile uint16_t ZACwire<pin>::rawData[2];
template<uint8_t pin>
volatile byte ZACwire<pin>::heartbeat;

#endif
