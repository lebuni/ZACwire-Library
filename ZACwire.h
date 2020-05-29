/*
	ZACwire - Library for reading temperature sensors TSIC 206/306/506
	This arduino library uses interrupts and one should use noInterrupts() or ZACwire::end() before time critical tasks. Also this library won't work if ISRs are deactivated.
	
	
	created by Adrian Immer
*/
#ifndef ZACwire_h
#define ZACwire_h

#include "Arduino.h"


class ZACwire {
  public:
	ZACwire(byte pin, int Sensortype = 306);
    bool begin();		//start reading sensor, needs to be called 100ms before the first getTemp()
    float getTemp();	//gives back temperature in °C
	void end();			//stop reading -> for time critical tasks
  private:
	void read();
	int isrPin;
	int _Sensortype;
	byte _pin;
	volatile byte BitCounter;
	volatile unsigned long ByteTime;
	volatile uint16_t tempValue[2];
	unsigned long microtime;
	byte window = 117;
	bool ByteNr;
	static ZACwire* objekt[60];
	static void attacher0();
	static void attacher1();
	static void attacher2();
	static void attacher3();
	static void attacher4();
	static void attacher5();
	static void attacher6();
	static void attacher7();
	static void attacher8();
	static void attacher9();
	static void attacher10();
	static void attacher11();
	static void attacher12();
	static void attacher13();
	static void attacher14();
	static void attacher15();
	static void attacher16();
	static void attacher17();
	static void attacher18();
	static void attacher19();
	static void attacher20();
	static void attacher21();
	static void attacher22();
	static void attacher23();
	static void attacher24();
	static void attacher25();
	static void attacher26();
	static void attacher27();
	static void attacher28();
	static void attacher29();
	static void attacher30();
	static void attacher31();
	static void attacher32();
	static void attacher33();
	static void attacher34();
	static void attacher35();
	static void attacher36();
	static void attacher37();
	static void attacher38();
	static void attacher39();
	static void attacher40();
	static void attacher41();
	static void attacher42();
	static void attacher43();
	static void attacher44();
	static void attacher45();
	static void attacher46();
	static void attacher47();
	static void attacher48();
	static void attacher49();
	static void attacher50();
	static void attacher51();
	static void attacher52();
	static void attacher53();
	static void attacher54();
	static void attacher55();
	static void attacher56();
	static void attacher57();
	static void attacher58();
	static void attacher59();

};


#endif
