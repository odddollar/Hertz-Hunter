#include "settings.h"

Settings::Settings()
  // Initialise to defaults
  : scanIntervalIndex(DEFAULT_INDEX), scanInterval(DEFAULT_SCAN_INTERVAL),
    buzzerIndex(DEFAULT_INDEX), buzzer(DEFAULT_BUZZER),
    batteryAlarmIndex(DEFAULT_INDEX), batteryAlarm(DEFAULT_BATTERY_ALARM),
    lowCalibratedRssi(DEFAULT_LOW_CALIBRATED_RSSI), highCalibratedRssi(DEFAULT_HIGH_CALIBRATED_RSSI),
    initialReadDone(false) {

  // When interval index changes, update actual interval
  scanIntervalIndex.onChange([this](int val) {
    scanInterval.set(2.5 * pow(2, val));
    if (initialReadDone) saveSettingsStorage("s_i_index", val);
  });

  // When buzzer index changes, update buzzer state
  buzzerIndex.onChange([this](int val) {
    buzzer.set(val == 0 ? true : false);
    if (initialReadDone) saveSettingsStorage("b_index", val);
  });

  // When battery index changes, update alarm threshold
  batteryAlarmIndex.onChange([this](int val) {
    batteryAlarm.set(-3 * val + 36);
    if (initialReadDone) saveSettingsStorage("b_a_index", val);
  });

  // Write calibration to storage on change
  lowCalibratedRssi.onChange([this](int val) {
    if (initialReadDone) saveSettingsStorage("l_c_rssi", val);
  });

  // Write calibration to storage on change
  highCalibratedRssi.onChange([this](int val) {
    if (initialReadDone) saveSettingsStorage("h_c_rssi", val);
  });
}

// Save given value to given key
void Settings::saveSettingsStorage(const char *key, int value) {
  preferences.begin("settings", false);
  preferences.putInt(key, value);
  preferences.end();
}

// Load all settings from memory
void Settings::loadSettingsStorage() {
  preferences.begin("settings", true);
  setScanIntervalIndex(preferences.getInt("s_i_index", DEFAULT_INDEX));
  setBuzzerIndex(preferences.getInt("b_index", DEFAULT_INDEX));
  setBatteryAlarmIndex(preferences.getInt("b_a_index", DEFAULT_INDEX));
  setLowCalibratedRssi(preferences.getInt("l_c_rssi", DEFAULT_LOW_CALIBRATED_RSSI));
  setHighCalibratedRssi(preferences.getInt("h_c_rssi", DEFAULT_HIGH_CALIBRATED_RSSI));
  preferences.end();

  // Used to prevent reading from non-volatile memory, updating variables, then immediately writing same value
  // Prevents unnecessary flash wear
  initialReadDone = true;
}

// Clear everything and reset
void Settings::clearReset() {
  preferences.begin("settings", false);
  preferences.clear();
  preferences.end();
  esp_restart();
}

// Update scan interval index
void Settings::setScanIntervalIndex(int index) {
  if (index < 0) index = 0;
  if (index > 2) index = 2;
  scanIntervalIndex.set(index);
}

// Update buzzer index
void Settings::setBuzzerIndex(int index) {
  if (index < 0) index = 0;
  if (index > 1) index = 1;
  buzzerIndex.set(index);
}

// Update battery alarm index
void Settings::setBatteryAlarmIndex(int index) {
  if (index < 0) index = 0;
  if (index > 2) index = 2;
  batteryAlarmIndex.set(index);
}

// Update low calibrated rssi
void Settings::setLowCalibratedRssi(int value) {
  if (value < 0) value = 0;
  if (value > 4095) value = 4095;
  lowCalibratedRssi.set(value);
}

// Update high calibrated rssi
void Settings::setHighCalibratedRssi(int value) {
  if (value < 0) value = 0;
  if (value > 4095) value = 4095;
  highCalibratedRssi.set(value);
}
