#include "Arduino.h"
#include "ZACwire.h"


ZACwire::ZACwire(byte pin, int Sensortype) {
  _pin = pin;
  _Sensortype = Sensortype;
}


void ICACHE_RAM_ATTR ZACwire::attacher0() {objekt[0]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher1() {objekt[1]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher2() {objekt[2]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher3() {objekt[3]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher4() {objekt[4]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher5() {objekt[5]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher6() {objekt[6]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher7() {objekt[7]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher8() {objekt[8]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher9() {objekt[9]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher10() {objekt[10]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher11() {objekt[11]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher12() {objekt[12]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher13() {objekt[13]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher14() {objekt[14]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher15() {objekt[15]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher16() {objekt[16]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher17() {objekt[17]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher18() {objekt[18]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher19() {objekt[19]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher20() {objekt[20]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher21() {objekt[21]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher22() {objekt[22]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher23() {objekt[23]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher24() {objekt[24]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher25() {objekt[25]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher26() {objekt[26]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher27() {objekt[27]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher28() {objekt[28]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher29() {objekt[29]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher30() {objekt[30]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher31() {objekt[31]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher32() {objekt[32]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher33() {objekt[33]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher34() {objekt[34]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher35() {objekt[35]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher36() {objekt[36]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher37() {objekt[37]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher38() {objekt[38]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher39() {objekt[39]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher40() {objekt[40]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher41() {objekt[41]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher42() {objekt[42]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher43() {objekt[43]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher44() {objekt[44]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher45() {objekt[45]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher46() {objekt[46]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher47() {objekt[47]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher48() {objekt[48]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher49() {objekt[49]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher50() {objekt[50]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher51() {objekt[51]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher52() {objekt[52]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher53() {objekt[53]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher54() {objekt[54]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher55() {objekt[55]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher56() {objekt[56]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher57() {objekt[57]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher58() {objekt[58]->read();}
void ICACHE_RAM_ATTR ZACwire::attacher59() {objekt[59]->read();}

bool ZACwire::begin() {
  pinMode(_pin, INPUT);
  if (!pulseInLong(_pin, HIGH)) return false;
  isrPin = digitalPinToInterrupt(_pin);
  switch (isrPin) {
	  case 0: attachInterrupt(isrPin, attacher0, RISING); break;
	  case 1: attachInterrupt(isrPin, attacher1, RISING); break;
	  case 2: attachInterrupt(isrPin, attacher2, RISING); break;
	  case 3: attachInterrupt(isrPin, attacher3, RISING); break;
	  case 4: attachInterrupt(isrPin, attacher4, RISING); break;
	  case 5: attachInterrupt(isrPin, attacher5, RISING); break;
	  case 6: attachInterrupt(isrPin, attacher6, RISING); break;
	  case 7: attachInterrupt(isrPin, attacher7, RISING); break;
	  case 8: attachInterrupt(isrPin, attacher8, RISING); break;
	  case 9: attachInterrupt(isrPin, attacher9, RISING); break;
	  case 10: attachInterrupt(isrPin, attacher10, RISING); break;
	  case 11: attachInterrupt(isrPin, attacher11, RISING); break;
	  case 12: attachInterrupt(isrPin, attacher12, RISING); break;
	  case 13: attachInterrupt(isrPin, attacher13, RISING); break;
	  case 14: attachInterrupt(isrPin, attacher14, RISING); break;
	  case 15: attachInterrupt(isrPin, attacher15, RISING); break;
	  case 16: attachInterrupt(isrPin, attacher16, RISING); break;
	  case 17: attachInterrupt(isrPin, attacher17, RISING); break;
	  case 18: attachInterrupt(isrPin, attacher18, RISING); break;
	  case 19: attachInterrupt(isrPin, attacher19, RISING); break;
	  case 20: attachInterrupt(isrPin, attacher20, RISING); break;
	  case 21: attachInterrupt(isrPin, attacher21, RISING); break;
	  case 22: attachInterrupt(isrPin, attacher22, RISING); break;
	  case 23: attachInterrupt(isrPin, attacher23, RISING); break;
	  case 24: attachInterrupt(isrPin, attacher24, RISING); break;
	  case 25: attachInterrupt(isrPin, attacher25, RISING); break;
	  case 26: attachInterrupt(isrPin, attacher26, RISING); break;
	  case 27: attachInterrupt(isrPin, attacher27, RISING); break;
	  case 28: attachInterrupt(isrPin, attacher28, RISING); break;
	  case 29: attachInterrupt(isrPin, attacher29, RISING); break;
	  case 30: attachInterrupt(isrPin, attacher30, RISING); break;
	  case 31: attachInterrupt(isrPin, attacher31, RISING); break;
	  case 32: attachInterrupt(isrPin, attacher32, RISING); break;
	  case 33: attachInterrupt(isrPin, attacher33, RISING); break;
	  case 34: attachInterrupt(isrPin, attacher34, RISING); break;
	  case 35: attachInterrupt(isrPin, attacher35, RISING); break;
	  case 36: attachInterrupt(isrPin, attacher36, RISING); break;
	  case 37: attachInterrupt(isrPin, attacher37, RISING); break;
	  case 38: attachInterrupt(isrPin, attacher38, RISING); break;
	  case 39: attachInterrupt(isrPin, attacher39, RISING); break;
	  case 40: attachInterrupt(isrPin, attacher40, RISING); break;
	  case 41: attachInterrupt(isrPin, attacher41, RISING); break;
	  case 42: attachInterrupt(isrPin, attacher42, RISING); break;
	  case 43: attachInterrupt(isrPin, attacher43, RISING); break;
	  case 44: attachInterrupt(isrPin, attacher44, RISING); break;
	  case 45: attachInterrupt(isrPin, attacher45, RISING); break;
	  case 46: attachInterrupt(isrPin, attacher46, RISING); break;
	  case 47: attachInterrupt(isrPin, attacher47, RISING); break;
	  case 48: attachInterrupt(isrPin, attacher48, RISING); break;
	  case 49: attachInterrupt(isrPin, attacher49, RISING); break;
	  case 50: attachInterrupt(isrPin, attacher50, RISING); break;
	  case 51: attachInterrupt(isrPin, attacher51, RISING); break;
	  case 52: attachInterrupt(isrPin, attacher52, RISING); break;
	  case 53: attachInterrupt(isrPin, attacher53, RISING); break;
	  case 54: attachInterrupt(isrPin, attacher54, RISING); break;
	  case 55: attachInterrupt(isrPin, attacher55, RISING); break;
	  case 56: attachInterrupt(isrPin, attacher56, RISING); break;
	  case 57: attachInterrupt(isrPin, attacher57, RISING); break;
	  case 58: attachInterrupt(isrPin, attacher58, RISING); break;
	  case 59: attachInterrupt(isrPin, attacher59, RISING); break;
	  default: return false;
  }
  objekt[isrPin] = this;
  return true;
}


void ICACHE_RAM_ATTR ZACwire::read() {
	microtime = micros() - microtime;
	if (microtime > 1000) {		  //begin reading
		ByteTime = micros();
		BitCounter = 1;
		ByteNr = tempValue[0] = tempValue[1] = 0;
	}
	if (BitCounter) {		//gets called with every new bit on rising edge
		if (++BitCounter + (ByteNr * 2) == 12) {
			BitCounter = !ByteNr;
			if (!ByteNr) {			//after stop bit
				microtime += window << 1;
				ByteNr = 1;
				ByteTime = micros() - ByteTime;
			}
		}
		tempValue[ByteNr] <<= 1;
		if (microtime > window + 24);		//Logic 0
		else if (microtime < window - 24 || tempValue[ByteNr] & 2) tempValue[ByteNr] |= 1;	//Logic 1
	}
	microtime = micros();
}

float ZACwire::getTemp() {
	byte parity1 = 0, parity2 = 0, timeout = 10;
	while (BitCounter && --timeout) delay(1);
	noInterrupts();  							//no ISRs because tempValue might change during reading
    uint16_t tempHigh = tempValue[0];		//get high significant bits from ISR
    uint16_t tempLow = tempValue[1];		//get low significant bits from ISR
	byte newWindow = (ByteTime << 5) + (ByteTime << 4) + ByteTime >> 9;
	if (abs(window-newWindow) < 20) window += (newWindow >> 3) - (window >> 3);	//adjust window time, which varies with rising temperature
    interrupts();
    for (byte i = 0; i < 9; ++i) {
      if (tempHigh & (1 << i)) ++parity1;
      if (tempLow & (1 << i)) ++parity2;
    }
    if (timeout && tempHigh | tempLow && ~(parity1 | parity2) & 1) {       // check for errors
		tempHigh >>= 1;               // delete parity bits
		tempLow >>= 1;
		tempLow += tempHigh << 8;	//joints high and low significant figures
		if (_Sensortype < 400) return (float(tempLow * 250L >> 8) - 499) / 10;	//calculates °C
		else return (float(tempLow * 175L >> 9) - 99) / 10;
    }
    else return 222;	//set to 222 if reading failed
}



void ZACwire::end() {
	for (byte timeout = 10; BitCounter && timeout; --timeout) delay(1);
	detachInterrupt(isrPin);
}

ZACwire* ZACwire::objekt[60] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
