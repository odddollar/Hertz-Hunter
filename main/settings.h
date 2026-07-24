#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>
#include "esp_system.h"
#include "variable.h"

#define DEFAULT_INDEX 0
#define DEFAULT_SCAN_INTERVAL 2.5
#define DEFAULT_BUZZER true
#define DEFAULT_BATTERY_ALARM 36
#define DEFAULT_LOW_CALIBRATED_RSSI 0
#define DEFAULT_HIGH_CALIBRATED_RSSI 4095

// Holds state for settings and handles updates to options
class Settings {
public:
  Settings();
  void saveSettingsStorage(const char *key, int value);
  void loadSettingsStorage();
  void clearReset();
  void setScanIntervalIndex(int index);
  void setBuzzerIndex(int index);
  void setBatteryAlarmIndex(int index);
  void setLowCalibratedRssi(int value);
  void setHighCalibratedRssi(int value);

  VariableCallback<int, Settings> scanIntervalIndex;
  Variable<float, Settings> scanInterval;
  VariableCallback<int, Settings> buzzerIndex;
  Variable<bool, Settings> buzzer;
  VariableCallback<int, Settings> batteryAlarmIndex;
  Variable<int, Settings> batteryAlarm;
  VariableCallback<int, Settings> lowCalibratedRssi;
  VariableCallback<int, Settings> highCalibratedRssi;

private:
  bool initialReadDone;

  Preferences preferences;
};

#endif
