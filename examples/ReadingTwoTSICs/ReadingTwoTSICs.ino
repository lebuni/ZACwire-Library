
/*
	Reads two sensors, where TSIC signal pins are connected to pin 2 and 10 on the board
 */


#include <ZACwire.h>


ZACwire TSIC306(2, 306);		// set pin "2" to receive signal from the TSIC "306"

ZACwire TSIC506(30, 506);		// set pin "30" as INPUT to receive signal from the TSIC "506"



void setup() {
  Serial.begin(500000); // set up the serial port
  if (!TSIC306.begin()) Serial.println("No digital pin with signal found");		//.begin() checks for signal and returns false if initializing failed
  if (!TSIC506.begin()) Serial.println("No digital pin with signal found");
  delay(100);
}

void loop() {
  float Input1 = TSIC306.getTemp();
  float Input2 = TSIC506.getTemp();


  Serial.print("Temp1: ");
  Serial.print(Input1);
  Serial.print(" Temp2: ");
  Serial.println(Input2);
  Serial.println("");
  
  delay(500);

}
