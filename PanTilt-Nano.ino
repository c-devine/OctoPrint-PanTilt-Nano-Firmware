#include "Arduino.h"
#include "PanTilt-Nano.h"
#include "EEPROM.h"
#include "Servo.h"
#include "SettingsManager.h"
#include "Led.h"

#define BUFFER_SIZE 128

SettingsManager settingsManager;
settings_t settings;
Servo pan;
Servo tilt;
Led led = Led(LED_PIN);
char buffer[BUFFER_SIZE];
char command[BUFFER_SIZE];
uint8_t bufferPos;

boolean commandReady = false;

void setup() {

	settingsManager.load(&settings);
	pan.attach(PAN_PIN, settings.panMin, settings.panMax);
	tilt.attach(TILT_PIN, settings.tiltMin, settings.tiltMax);
	pan.writeMicroseconds(settings.panUs);
	tilt.writeMicroseconds(settings.tiltUs);
	led.off();

	memset(buffer, 0, BUFFER_SIZE);
	memset(command, 0, BUFFER_SIZE);
	bufferPos = 0;
	Serial.begin(BAUD);

}

void moveServo(uint8_t pin, uint16_t us) {
	if (pin == PAN_PIN) {
		pan.writeMicroseconds(us);
	} else {
		tilt.writeMicroseconds(us);
	}
}

void serialEvent() {

	while (Serial.available()) {

		char inChar = (char) Serial.read();
		// skip CR
		if (inChar == '\r')
			continue;

		buffer[bufferPos++] = inChar;

		if (inChar == '\n') {
			// don't copy the newline
			strncpy(command, buffer, bufferPos - 1);
			memset(buffer, 0, BUFFER_SIZE);
			bufferPos = 0;
			commandReady = true;
		}
	}
}
// set 1500 1500
boolean processCommand() {

	uint8_t commandSize = 3;
	char *cmd[commandSize];
	uint8_t i = 0;

	Serial.println(command);
	cmd[i] = strtok(command, " ");

	while (cmd[i] != NULL) {
		cmd[++i] = strtok(NULL, " ");

		if (i == commandSize)
			break;
	}

	if (strcmp("setUs", cmd[0]) == 0) {
		uint16_t pan = (uint16_t) atoi(cmd[1]);
		uint16_t tilt = (uint16_t) atoi(cmd[2]);

		moveServo(PAN_PIN, pan);
		moveServo(TILT_PIN, tilt);

		settings.panUs = pan;
		settings.tiltUs = tilt;
		settingsManager.save(&settings);
		return true;
	}

	else if (strcmp("reset", cmd[0]) == 0) {
		settingsManager.reset(&settings);
		moveServo(PAN_PIN, settings.panUs);
		moveServo(TILT_PIN, settings.tiltUs);
		return true;

	}

	else if (strcmp("version", cmd[0]) == 0) {
		Serial.println(VERSION);
	}

	return false;
}

uint32_t millisCounter;

void loop() {

	if (commandReady) {
		commandReady = false;
		if (processCommand()) {
			Serial.println("OK");
		}

		// reset command buffer
		memset(command, 0, BUFFER_SIZE);
	}

	if (millis() - millisCounter > 1000) {
		//Serial.println(".");
		led.toggle();
		millisCounter = millis();
	}
}

