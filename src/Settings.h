#ifndef Settings_h
#define Settings_h

#include "Arduino.h"
#include <FS.h>
extern "C" {
#include "user_interface.h"
}
#include "A_config.h"
#include "ArduinoJson.h"
#include "Credential.h"
#include "WifiConfigData.h"
#include "language.h"

#define VERSION "v1.1"

extern void checkFile(String path, String data);
extern JsonVariant parseJSONFile(String path, DynamicJsonBuffer &jsonBuffer);
extern bool writeFile(String path, String &buf);
extern void saveJSONFile(String path, JsonObject &root);
extern String macToStr(uint8_t *mac);
extern void getRandomMac(uint8_t *mac);
extern bool strToMac(String macStr, uint8_t *mac);
extern void setWifiChannel(uint8_t ch);
extern String fixUtf8(String str);
extern void copyWebFiles(bool force);
extern bool macValid(uint8_t *mac);
extern WifiConfigData wifiConfig;
extern Credential credential;

enum CAPTIVE_TYPE {FACEBOOK, ZALO, PHONE, TWITTER, WIFI };

class Settings {
public:
  Settings();
  bool captivePortal = true;

  void load();
  void load(String filepath);
  void save(bool force);
  void save(bool force, String filepath);
  void reset();
  void print();

  void set(const char *str, String value);
  String get(const char *str);

  String getVersion();
  uint16_t getDeauthsPerTarget();
  uint8_t getDeauthReason();
  bool getBeaconChannel();
  uint8_t getForcePackets();
  bool getAutosave();
  uint32_t getAutosaveTime();
  bool getBeaconInterval();
  uint8_t getChannel();
  String getSSID();
  String getPassword();
  bool getNonePassword();
  bool getCLI();
  bool getDisplayInterface();
  bool getWebInterface();
  uint16_t getChTime();
  uint8_t *getMacSt();
  uint8_t *getMacAP();
  bool getRandomTX();
  uint32_t getAttackTimeout();
  bool getLedEnabled();
  uint8_t getProbesPerSSID();
  bool getHidden();
  bool getCaptivePortal();
  uint16_t getMinDeauths();
  uint32_t getDisplayTimeout();
  String getLang();
  bool getSerialEcho();
  bool getWebSpiffs();

  void setDeauthsPerTarget(uint16_t deauthsPerTarget);
  void setDeauthReason(uint8_t deauthReason);
  void setBeaconChannel(bool beaconChannel);
  void setForcePackets(uint8_t forcePackets);
  void setAutosave(bool autosave);
  void setAutosaveTime(uint32_t autosaveTime);
  void setBeaconInterval(bool beaconInterval);
  void setChannel(uint8_t channel);
  void setSSID(String ssid);
  void setPassword(String password);
  void setNonePassword(bool isNonePassword);
  void setCLI(bool cli);
  void setDisplayInterface(bool displayInterface);
  void setWebInterface(bool webInterface);
  void setChTime(uint16_t chTime);
  void setMacSt(String macStr);
  bool setMacSt(uint8_t *macSt);
  void setMacAP(String macStr);
  bool setMacAP(uint8_t *macAP);
  void setRandomTX(bool randomTX);
  void setAttackTimeout(uint32_t attackTimeout);
  void setLedEnabled(bool ledEnabled);
  void setProbesPerSSID(uint8_t probesPerSSID);
  void setHidden(bool hidden);
  void setCaptivePortal(bool captivePortal);
  void setMinDeauths(uint16_t minDeauths);
  void setDisplayTimeout(uint32_t displayTimeout);
  void setLang(String lang);
  void setSerialEcho(bool serialEcho);
  void setWebSpiffs(bool webSpiffs);

  void setCaptiveType(int type);
  int getCaptiveType();
  void setChangeSSID();
  // void setBrightness(int brightness);
  // int getBrightness();
  // String getSSID();

private:
  bool changed = false;
  int captive_type = CAPTIVE_TYPE::PHONE;

  String version = VERSION;

  bool beaconChannel = false;
  bool autosave = true;
  bool beaconInterval = false;
  bool cli = true;
  bool displayInterface = USE_DISPLAY;
  bool webInterface = true;
  bool webSpiffs = false;
  bool randomTX = false;
  bool ledEnabled = true;
  bool serialEcho = true;

  uint32_t attackTimeout = 600;
  uint32_t autosaveTime = 10000;
  uint32_t displayTimeout = 600;
  uint16_t deauthsPerTarget = 20;
  uint16_t chTime = 384;
  uint16_t minDeauths = 3;
  uint8_t forcePackets = 1;
  uint8_t channel = 9;
  uint8_t deauthReason = 1;
  uint8_t *macSt;
  uint8_t *macAP;
  uint8_t probesPerSSID = 1;

  String ssid = "FREE HIGHSPEED WIFI";
  String password = "12345678";
  bool nonepassword = true;
  bool hidden = false;
  String lang = "vi";

  String FILE_PATH = "/settings.json";

  String getJsonStr();
};

#endif // ifndef Settings_h
