/*	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	created by Adrian Immer in 2020
	v1.3.3 bleeding-edge b2 
	Changes:
	- Added directMode, which disable a few internal mechanism to detect outliers, disables on most part useBackup functionality and takes care that the same temp value is not returned twice. 
	  - This is useful when the calling program needs to have access to "raw" data to build their own "outlier" detection (recommended on esp32).
	- Added auto-tuning of initial bitwindow to quicker tune-in to best values. (This helps when using esp32).
	- Added getBitWindow() to help debug stuff.
	- Minor one-bit-off fix of timeDelta saved in rawData (?).
*/

#ifndef ZACwire_h
#define ZACwire_h

#define ZACWIRE_VERSION 134L

#include "Arduino.h"

#ifdef ARDUINO_ESP8266_RELEASE_
	#include <gpio.h>
	#warning "Arduino ESP8266 3.0.0 has issues with IRAM. Please downgrade to 2.7.4 for more efficient memory usage"
	void IRAM_ATTR tsicIsrHandler(uint8_t gpio);
#endif


template <uint8_t pin>
class ZACwire {
	
	friend void tsicIsrHandler(uint8_t);
	
	public:

		ZACwire(int sensor = 306, byte defaultBitWindow=125, bool core=1, bool directMode=false) {
			_sensor = sensor;
			bitWindow = defaultBitWindow;		//expected BitWindow in µs, depends on sensor & temperature
			_core = core;							//only ESP32: choose cpu0 or cpu1
			_directMode = directMode;
		}

		byte getBitWindow() {  //just used for debugging purposes
			return bitWindow;
		}

		bool begin() {								//start collecting data, needs to be called 100+ms before first getTemp()
			pinMode(pin, INPUT);
			if (!pulseInLong(pin, LOW)) return false;			//check if there is incoming signal
			byte isrPin = digitalPinToInterrupt(pin);
			if (isrPin == 255) return false;
			initialize = true;
			#ifdef ESP32
			xTaskCreatePinnedToCore(attachISR_ESP32,"attachISR",2000,NULL,1,NULL,_core); //freeRTOS
			#elif defined(ARDUINO_ESP8266_RELEASE_)				//In ARDUINO_ESP8266_RELEASE_3.0.0 a new version of gcc with bug 70435
			ETS_GPIO_INTR_ATTACH(tsicIsrHandler,(intptr_t)isrPin);		//...is included, which has issues with IRAM and template classes.
			gpio_pin_intr_state_set(isrPin,GPIO_PIN_INTR_POSEDGE);		//...That's the reason to use nonOS here
			ETS_GPIO_INTR_ENABLE();
			#else
			attachInterrupt(isrPin, read, RISING);
			#endif
			delay(110);
			getTemp();  //call to initially set correct bitwindow
			return true;
		}
	  
		float getTemp() {							//return temperature in °C
			static unsigned int lastHB;
			if (!heartbeat) {						//check wire connection
				if (!bitCounter) {					//use bitCounter to prove that ISR was already attached
					begin();
					delay(110);
				}
				else if (!lastHB) lastHB = millis();			//record first missing heartbeat
				else if ((unsigned int)millis() - lastHB > 255) return 221;	//return error 221 after timeout of 255ms
			}
			else {
				lastHB = 0;
				if (bitCounter > 4 && bitCounter < 11) {		//adjust bitWindow
					byte newBitWindow = rawData[backUP] >> (bitCounter - 1);	//seperate newBitWindow from temperature bits and divide by 4
					newBitWindow = (((newBitWindow>>1) + newBitWindow) >> 1) + 4 + (bitWindow>>2);	//divide by 1.31 (w/ previous line: by 5.25) and add 1/4 bitWindow

					if (initialize) {
						if (abs(bitWindow - newBitWindow) <24) {      // suppress outlier to be used in initialization
							//DEBUG_print("Temp sensor bitwindow set to %d (orig=%d)\n", newBitWindow, bitWindow);
							bitWindow = newBitWindow;
							initialize = false;
							delay(200);
							getTemp();
						} else {
							//ERROR_print("Temp sensor bitwindow set to %d (orig=%d) failed. Retrying.\n", newBitWindow, bitWindow);
							bitCounter=6;  //fetch another deltaTime
							delay(200);
							getTemp();
						}
					} else if (bitWindow != newBitWindow) {
						bitWindow < newBitWindow ? ++bitWindow : --bitWindow;
					}
					
				}
			}
			
			static bool useBackup;
			if (bitCounter != 19) useBackup = true;			    //when newer reading is incomplete
			uint16_t temp = rawData[backUP^useBackup];			//get rawData from ISR
			if (_directMode) rawData[backUP^useBackup] = 0;     //dont ever return the same temp twice
			
			if (tempCheck(temp)) {
				temp >>= 1;						//delete second parity bit
				temp = (temp >> 2 & 1792) | (temp & 255);		//delete first    "     "
				static int prevTemp = temp;
				int grad ((temp - prevTemp) / (heartbeat|1));		//grad is [°C/s]
				if (abs(grad) < 20 || _directMode) {					//limit change rate to 20°C/s to detect misreadings
					prevTemp = temp;
					heartbeat = useBackup = 0;
					return (_sensor < 400 ? (temp * 250L >> 8) - 499 : (temp * 175L >> 9) - 99) / 10.0;	//use different formula for 206&306 or 506
				}
				return 1000;
			}
			useBackup = !useBackup;						//reset useBackup after use
			return (useBackup && !_directMode) ? getTemp(): 222;				//restart with backUP rawData or return error value 222
		}

		void end() {
			detachInterrupt(digitalPinToInterrupt(pin));
		}
	
	private:

		#ifdef ESP32
		static void attachISR_ESP32(void*){					//attach ISR in freeRTOS because gcc 5.3+ can't put ISR in IRAM inside of template class
			gpio_num_t isrPin = (gpio_num_t)pin;
			gpio_pad_select_gpio(isrPin);
			gpio_set_direction((gpio_num_t)isrPin, GPIO_MODE_INPUT);
			gpio_set_intr_type(isrPin, GPIO_INTR_POSEDGE);
			gpio_install_isr_service(0);
			gpio_isr_handler_add(isrPin, read, NULL);
			vTaskDelete(NULL);
		}
		static void IRAM_ATTR read(void*) {
		#elif defined(ARDUINO_ESP8266_RELEASE_)
		static inline void read() __attribute__((always_inline)) {
			uint16 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);	//get GPIO that caused the interrupt
			if (gpio_status != BIT(pin)) return;				//check if the right GPIO triggered
			GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS,gpio_status);		//clear interrupt flag
		#elif defined (ESP8266)
		static void ICACHE_RAM_ATTR read() {
		#else
		static void read() {							//get called with every rising edge
		#endif
			if (++bitCounter > 4) {						//first 4 bits always =0
				unsigned int microtime = micros();
				static unsigned int deltaTime;
				deltaTime = microtime - deltaTime;			//measure time to previous rising edge
				if (deltaTime >> 10) {					//start bit
					backUP ^= bitCounter >= 20;     //sometimes additional interrupts are thrown afterwards
					bitCounter = 0;
					++heartbeat;					//give a sign of life to getTemp()
				}
				else if (bitCounter == 5) rawData[backUP] = deltaTime<<2 | 2;	//send deltaTime for calculating bitWindow and add prefix "10" to temp
				else if (initialize && bitCounter == 6) { --bitCounter; } //during initialization wait for getTemp() to set bitWindow
				else {
					if (bitCounter == 10) microtime += bitWindow>>2;	//convert timestamp at stop bit to normal 0 bit
					rawData[backUP] <<= 1;
					rawData[backUP] |= bitWindow > deltaTime + ((rawData[backUP] & 2)?0:(bitWindow>>1));	//add 1/2 bitWindow if previous bit was 1 (for normalisation)
				}
				deltaTime = microtime;
			}
		}
		
		bool tempCheck(uint16_t &rawTemp) {					
			if (rawTemp >> 14 == 2 && ~rawTemp & 512) {				//check for prefix "10" and stop bit
				bool parity = true;
				for (byte i=0; i<9; ++i) parity ^= rawTemp & 1 << i;
				if (parity) for (byte i=10; i<14; ++i) parity ^= rawTemp & 1 << i;
				return parity;
			}
			return false;
		}
		
		int _sensor;
		int _directMode;
		bool _core;
		static bool initialize;
		static byte bitWindow;
		static volatile byte bitCounter;
		static volatile bool backUP;
		static volatile uint16_t rawData[2];
		static volatile byte heartbeat;
};

template<uint8_t pin>
bool ZACwire<pin>::initialize;
template<uint8_t pin>
byte ZACwire<pin>::bitWindow;
template<uint8_t pin>
volatile byte ZACwire<pin>::bitCounter;
template<uint8_t pin>
volatile bool ZACwire<pin>::backUP;
template<uint8_t pin>
volatile uint16_t ZACwire<pin>::rawData[2];
template<uint8_t pin>
volatile byte ZACwire<pin>::heartbeat;

#ifdef ARDUINO_ESP8266_RELEASE_
void tsicIsrHandler(uint8_t gpio) {
	switch (gpio) {
		case 2:
			ZACwire<2>::read();
			break;
		case 4:
			ZACwire<4>::read();
			break;
		case 5:
			ZACwire<5>::read();
			break;
		case 12:
			ZACwire<12>::read();
			break;
		case 13:
			ZACwire<13>::read();
			break;
		case 14:
			ZACwire<14>::read();
			break;
	}
}
#endif

#endif
