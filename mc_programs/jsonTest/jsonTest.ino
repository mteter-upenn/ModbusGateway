/*
 Name:		jsonTest.ino
 Created:	1/6/2017 2:24:11 PM
 Author:	mteter
*/

// the setup function runs once when you press reset or power the board
#include <ArduinoJson.h>
#include <SD.h>


void setup() {
	const char fileName[] = "/maplist1.jsn";
	delay(2000);

	Serial.begin(9600);
	Serial.println("start prog");

	// may be necessary to set pin 10 high regardless of arduino type
	pinMode(10, OUTPUT);
	digitalWrite(10, HIGH);
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);

	Serial.println(F("Initializing SD card..."));
	if (!SD.begin(4)) {
		Serial.println(F("ERROR - SD card initialization failed!"));
		return;
	}
	else{
		Serial.println(F("SUCCESS - SD card initialized."));
	}

	Serial.println(fileName);
	File jsonFile = SD.open(fileName);


	if (jsonFile) {
		Serial.println("opened file");
		StaticJsonBuffer<20000> jsonBuffer;  // have no idea what size to use
		Serial.println("made buffer");
		JsonObject& root = jsonBuffer.parseObject(jsonFile);
		Serial.println("parsed file");

		if (!root.success()) {
			Serial.println("Failed!");
			return;
		}
		Serial.println("success!");
		Serial.print("length of json: "); Serial.println(root.measureLength(), DEC);

		uint16_t u16_numMtrs = root["meterlist"].size();

		Serial.print("num meters: "); Serial.println(u16_numMtrs, DEC);

		char ca_mtrName[50] = {0};
		uint16_t u16_numBlks;
		uint16_t u16_numGrps;
		uint16_t u16_blkStrt;
		uint16_t u16_blkEnd;
		char ca_dataType[20] = {0};
		uint16_t u16_grpVals;
		uint16_t u16_grpRegs;
		uint16_t u16_grpStrt;
		int16_t s16_grpOrder[63] = {0};


		for (int ii = 0; ii < u16_numMtrs; ++ii) {
			strcpy(ca_mtrName, root["meterlist"][ii]["name"]);
			u16_numBlks = root["meterlist"][ii]["blocks"].size();
			u16_numGrps = root["meterlist"][ii]["groups"].size();

			Serial.print("meter "); Serial.print(ii + 1); Serial.print(" name: "); Serial.println(ca_mtrName);

			Serial.println("\tBlock\tStart\tEnd\tType");

			for (int jj = 0; jj < u16_numBlks; ++jj) {
				u16_blkStrt = root["meterlist"][ii]["blocks"][jj]["start"];
				u16_blkEnd = root["meterlist"][ii]["blocks"][jj]["end"];
				strcpy(ca_dataType, root["meterlist"][ii]["blocks"][jj]["type"]);

				Serial.print("\t"); Serial.print(jj + 1);
				Serial.print("\t"); Serial.print(u16_blkStrt);
				Serial.print("\t"); Serial.print(u16_blkEnd);
				Serial.print("\t"); Serial.println(ca_dataType);
			}

		}
	}
	else {
		Serial.println("bad file");
	}
	Serial.println("done prog");

}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
