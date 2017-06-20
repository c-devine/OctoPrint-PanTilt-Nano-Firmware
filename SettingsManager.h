#ifndef SETTINGSMANAGER_H_
#define SETTINGSMANAGER_H_

#include "Arduino.h"

#define SETTINGS_SIZE sizeof(settings_t)

typedef struct
{
   uint16_t panUs;
   uint16_t panMin;
   uint16_t panMax;
   uint16_t tiltUs;
   uint16_t tiltMin;
   uint16_t tiltMax;
} __attribute__((packed)) settings_t;


class SettingsManager {
public:
	SettingsManager();
	virtual ~SettingsManager();

	void load(settings_t *settings);
	void save(settings_t *settings);
	void reset(settings_t *settings);

};

#endif /* SETTINGSMANAGER_H_ */
