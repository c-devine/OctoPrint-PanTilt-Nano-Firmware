#include "EEPROM.h"
#include "SettingsManager.h"

SettingsManager::SettingsManager() {
	// TODO Auto-generated constructor stub

}

SettingsManager::~SettingsManager() {
	// TODO Auto-generated destructor stub
}

void SettingsManager::load(settings_t* settings) {

	byte* byteArray = (byte*) settings;
	for (uint16_t i = 0; i < SETTINGS_SIZE; i++)
		byteArray[i] = EEPROM.read(i);

	// look for uninitialized settings
	if (settings->panMin == settings->panMax) {
		this->reset(settings);
	}

}

void SettingsManager::save(settings_t* settings) {

	byte* byteArray = (byte*) settings;
	for (uint16_t i = 0; i < SETTINGS_SIZE; i++)
		EEPROM.write(i, byteArray[i]);
}

void SettingsManager::reset(settings_t* settings) {
	settings->panMin = 1000;
	settings->panMax = 2000;
	settings->panUs = 1500;
	settings->tiltMin = 1000;
	settings->tiltMax = 2000;
	settings->tiltUs = 1500;

	// save the settings
	this->save(settings);
}
