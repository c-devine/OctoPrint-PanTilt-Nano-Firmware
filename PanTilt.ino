#include "Arduino.h"
#include "EEPROM.h"
#include "Servo.h"
#include "PanTilt.h"

#include "SettingsManager.h"
#include "Led.h"
#include "ArduinoJson-v5.8.4.h"

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
		// skip carriage return
		if (inChar == '\r')
			continue;

		buffer[bufferPos++] = inChar;

		if (inChar == '\n') {
			strncpy(command, buffer, bufferPos);
			memset(buffer, 0, BUFFER_SIZE);
			bufferPos = 0;
			commandReady = true;
		}
	}
}

void printSettings() {

	StaticJsonBuffer<BUFFER_SIZE * 2> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root["panUs"] = settings.panUs;
	root["panMin"] = settings.panMin;
	root["panMax"] = settings.panMax;
	root["tiltUs"] = settings.tiltUs;
	root["tiltMin"] = settings.tiltMin;
	root["tiltMax"] = settings.tiltMax;

	root.printTo(Serial);
}

void printHelp() {

	Serial.println("{command:version|print_settings|reset}");
	Serial.println("{command:set|move, target:panUs|panMin|panMax|tiltUs|tiltMin|tiltMax, value:value}");
	Serial.println("example:");
	Serial.println("{command:'print_settings'}");
	Serial.println("command:'set', target:'panUs', value:1400}");
	Serial.flush();
}

boolean processCommand() {

	StaticJsonBuffer<BUFFER_SIZE * 2> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(command);

	if (!root.success()) {
		Serial.println("ERROR: parseObject() failed");
		return false;
	}

	// return if there are not 1 or 3 arguments
	if (root.size() != 1 && root.size() != 3) {
		Serial.println("ERROR: invalid command");
		return false;
	}

	const char commands[128] = "version print_settings reset set move help";
	const char* jCommand = root["command"];

	if (strstr(commands, jCommand) == NULL) {
		Serial.println("ERROR: invalid command");
		return false;
	}

	// single parameter commands
	if (root.size() == 1) {
		if (strcmp("version", jCommand) == 0) {
			Serial.println(VERSION);
			return false;
		}
		if (strcmp("print_settings", jCommand) == 0) {
			printSettings();
			return false;
		}
		if (strcmp("help", jCommand) == 0) {
			printHelp();
			return false;
		}
		if (strcmp("reset", jCommand) == 0) {
			settingsManager.reset(&settings);
			moveServo(PAN_PIN, settings.panUs);
			moveServo(TILT_PIN, settings.tiltUs);
		}
		return true;
	}

	// multi-parameter commands
	const char targets[128] = "panUs panMin panMax tiltUs tiltMin tiltMax";
	const char* jTarget = root["target"];
	uint16_t jValue = root["value"];

	if (strstr(targets, jTarget) == NULL) {
		Serial.println("ERROR: invalid target");
		return false;
	}

	if (strcmp("set", jCommand) == 0) {
		if (strcmp("panMin", jTarget) == 0) {
			settings.panMin = jValue;
		}
		if (strcmp("panMax", jTarget) == 0) {
			settings.panMax = jValue;
		}
		if (strcmp("panUs", jTarget) == 0) {
			if (jValue < settings.panMin || jValue > settings.panMax) {
				Serial.print("ERROR: invalid value");
				return false;
			}
			settings.panUs = jValue;
			moveServo(PAN_PIN, jValue);
		}
		if (strcmp("tiltMin", jTarget) == 0) {
			settings.tiltMin = jValue;
		}
		if (strcmp("tiltMax", jTarget) == 0) {
			settings.tiltMax = jValue;
		}
		if (strcmp("tiltUs", jTarget) == 0) {
			if (jValue < settings.tiltMin || jValue > settings.tiltMax) {
				Serial.print("ERROR: invalid value");
				return false;
			}
			settings.tiltUs = jValue;
			moveServo(TILT_PIN, jValue);
		}
		// save the new values
		settingsManager.save(&settings);
	}

	else if (strcmp("move", jCommand) == 0) {
		if (strcmp("pan", jCommand) == 0) {
			if (settings.panUs + jValue > settings.panMax
					|| settings.panUs + jValue < settings.panMin) {

				Serial.print("ERROR: invalid value");
				return false;
			}
		}
		settings.panUs += jValue;
		moveServo(PAN_PIN, settings.panUs);

		if (strcmp("tilt", jTarget) == 0) {
			if (settings.tiltUs + jValue > settings.tiltMax
					|| settings.tiltUs + jValue < settings.tiltMin) {

				Serial.print("ERROR: invalid value");
				return false;
			}
			settings.tiltUs += jValue;
			moveServo(TILT_PIN, settings.tiltUs);
		}

		// save the new values
		settingsManager.save(&settings);
	}
	return true;
}

uint32_t millisCounter;
uint16_t testCounter = 0;
uint8_t up = 1;

void loop() {

	if (commandReady) {
		commandReady = false;
		// add OK ack if return is true
		if (processCommand()) {
			Serial.println("OK");
			Serial.flush();
		}
		// reset command bufer
		memset(command, 0, BUFFER_SIZE);
	}

	if (millis() - millisCounter > 1000) {
		//Serial.println(".");
		led.toggle();
		millisCounter = millis();
	}

}

