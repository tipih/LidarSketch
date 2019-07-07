/*
 Name:		LidarSketch.ino
 Created:	7/5/2019 10:21:45 AM
 Author:	michael
*/
//HC12 Radio config
//9600
//FU3
//Ch30
//Powe +20dbM

//Header files goes here
#include <SoftwareSerial.h>

//Globale defines goes here
int distance = 0;						//Distance to object
int strength = 0;						//Receiver strength
int calibratedStrength = 0;
int calibratedDistance = 0;
boolean receiveComplete = false;		//Flag to get all data
static bool calibrated = false;
unsigned long int time = 0;

SoftwareSerial HC12(3, 4); // Arduino RX, TX  HC12(TX,RX)
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	HC12.begin(9600);
	pinMode(13, OUTPUT);
	Serial.println("Setting up Serial port to 115200");
	TfminiSetup();
	calibration();
	time = millis();
	
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	if (millis()-time > 1000)
	{
		digitalWrite(13, !digitalRead(13));
		time = millis();
	}

	
	if (receiveComplete) {
		receiveComplete = false;
		
		if (distance > 400) distance = 400;
		if (abs(distance - calibratedDistance) > 20)
		{
			HC12.print("<STOP>");
			HC12.flush();
			Serial.print(distance);
			Serial.print("cm\t");
			Serial.print("strength: ");
			Serial.println(strength);
		}
	}
}

void serialEvent() {
	getTFminiData(&distance, &strength, &receiveComplete);
	
}

int calibration()
{
	bool finished = false;
	static int nbr = 0;

	Serial.println("Enter Calibration");

	while (calibrated == false) {
		
		
		getTFminiData(&distance, &strength, &receiveComplete);
		
		if (receiveComplete) {
			receiveComplete = false;
			calibratedStrength = calibratedStrength + strength;
			calibratedDistance = calibratedDistance + distance;
			nbr++;
			Serial.println(nbr);
			if (nbr == 100) calibrated = true;
		}
	}

	calibratedDistance = calibratedDistance / nbr;
	calibratedStrength = calibratedStrength / nbr;
	if (calibratedDistance > 400)calibratedDistance = 400;
	Serial.print("Calibrated Strength");
	Serial.println(calibratedStrength);
	Serial.print("Calibrated distance");
	Serial.println(calibratedDistance);
	return true;
}