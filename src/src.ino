/*
   ===========================================
      Copyright (c) 2018 Stefan Kremser
             github.com/spacehuhn
   ===========================================
 */

extern "C" {
// Please follow this tutorial:
// https://github.com/spacehuhn/esp8266_deauther/wiki/Installation#compiling-using-arduino-ide
// And be sure to have the right board selected
#include "user_interface.h"
}

#include <EEPROM.h>

#include <ArduinoJson.h>
#if ARDUINOJSON_VERSION_MAJOR != 5
// The software was build using ArduinoJson v5.x
// version 6 is still in beta at the time of writing
// go to tools -> manage libraries, search for ArduinoJSON and install the
// latest version 5
//#error Please upgrade/downgrade ArduinoJSON library to version 5!
#endif

#include "A_config.h"
#include "Attack.h"
#include "CLI.h"
#include "Credential.h"

#include "Names.h"

#include "SSIDs.h"
#include "Scan.h"
#include "Settings.h"
#include "Telnet.h"

#include "WifiConfigData.h"
#include "deautherDetector.h"
#include "functions.h"
#include "imagefile.h"
#include "language.h"
#include "oui.h"
#include "webfiles.h"

Telnet telnet;

Settings settings;
Names names;
SSIDs ssids;
Accesspoints accesspoints;
Stations stations;
Scan scan;
Attack attack;
CLI cli;

Credential credential;
WifiConfigData wifiConfig;

#include "wifi.h"

void writeString(char add, String data);
String readString(char add);

uint32_t autosaveTime = 0;
uint32_t currentTime = 0;

bool booted = false;

void setup() {
  // for random generator
  randomSeed(os_random());

  // start serial
  Serial.begin(115200);
  Serial.println();
  // Start EEPROM
  EEPROM.begin(4096);
  // delay(2000);

  prnt(SETUP_MOUNT_SPIFFS);
  prntln(SPIFFS.begin() ? SETUP_OK : SETUP_ERROR);

  // auto repair when in boot-loop
  uint8_t bootCounter = EEPROM.read(0);

  if (bootCounter >= 5) {
    prnt(SETUP_FORMAT_SPIFFS);
    SPIFFS.format();
    prntln(SETUP_OK);
  } else {
    EEPROM.write(0, bootCounter + 1); // add 1 to the boot counter
    EEPROM.commit();
  }

  // get time
  currentTime = millis();

  // load settings
  settings.load();

  // set mac for access point
  wifi_set_macaddr(SOFTAP_IF, settings.getMacAP());

  // start WiFi //help //WIFI_AP
  WiFi.mode(WIFI_AP);
  wifi_set_opmode(SOFTAP_MODE);
  wifi_set_promiscuous_rx_cb(
      [](uint8_t *buf, uint16_t len) { scan.sniffer(buf, len); });

  // set mac for station
  wifi_set_macaddr(STATION_IF, settings.getMacSt());

  names.load();
  ssids.load();
  cli.load();

  wifiConfig.init();
  credential.init();

  telnet.setup();

  credential.toSerial();
  wifiConfig.toSerial();

  scan.setup();

  // set channel
  setWifiChannel(settings.getChannel());

// load Wifi settings: SSID, password,...
#ifdef DEFAULT_SSID
  if (settings.getSSID() == "pwned")
    settings.setSSID(DEFAULT_SSID);
#endif // ifdef DEFAULT_SSID
  loadWifiConfigDefaults();
  // dis/enable serial command interface
  if (settings.getCLI()) {
    cli.enable();
  } else {
    prntln(SETUP_SERIAL_WARNING);
    Serial.flush();
    Serial.end();
  }
  // start access point/web interface
  if (settings.getWebInterface()) {
    startAP();
  }
  // STARTED
  prntln(SETUP_STARTED);
  setupMdns();
  prntln(settings.getVersion());
}

bool isGetUpdate = false;
bool isResetupMdns = false;
bool isSetWiFiMode = false;
int cnt = 0;

void loop() {
  if (!isDetectDeauther) {
    if (isSetWiFiMode) {
      wifi_set_opmode(STATIONAP_MODE);
      wifi_promiscuous_enable(false);
      if (cnt > 10) {
        isSetWiFiMode = false;
      }
      cnt = cnt + 1;
    }
    if (WiFi.status() == WL_CONNECTED) {
      if (!isResetupMdns) {
        isResetupMdns = MDNS.close();
        setupMdns();
      }
    }

    wifiUpdate(); // manage access point
    attack.update();
    scan.update();  // run scan
    ssids.update(); // run random mode, if enabled

    if (settings.getAutosave() &&
        (currentTime - autosaveTime > settings.getAutosaveTime())) {
      autosaveTime = currentTime;
      names.save(false);
      ssids.save(false);
      settings.save(false);
    }
  } else {
    isSetWiFiMode = true;
    cnt = 0;
  }

  currentTime = millis();
  cli.update(); // read and run serial input

  if (!booted) {
    EEPROM.write(0, 0);
    EEPROM.commit();
    booted = true;
  }
}
