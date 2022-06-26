#include "Telnet.h"

WiFiServer telnet_server(TELNET_PORT);
WiFiClient serverClients[MAX_TELNET_CLIENTS];
WiFiClient currentClient;

Telnet::Telnet(/* args */) {}

Telnet::~Telnet() {}

void Telnet::setup() {
  telnet_server.begin();
  telnet_server.setNoDelay(true);
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(did, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3],
          mac[4], mac[5]);
}

String Telnet::update() {
  if (telnet_server.hasClient()) {
    int i;
    for (i = 0; i < MAX_TELNET_CLIENTS; i++)
      if (!serverClients[i]) {
        serverClients[i] = telnet_server.available();
        serverClients[i].print(serverClients[i].remoteIP());
        serverClients[i].println(" connected");
        currentClient = serverClients[i];
        print_tnet_new_line();
        break;
      }
    if (i == MAX_TELNET_CLIENTS) {
      telnet_server.available().println("busy");
    }
  }

  for (int i = 0; i < MAX_TELNET_CLIENTS; i++) {
    data = String();
    while (serverClients[i].available()) {
      char text = serverClients[i].read();
      currentClient = serverClients[i];
      data = data + text;
    }

    if (data.length() > 0) {
      if (data == "\r\n") {
        print_tnet_new_line();
        data = String();
        return "";
      }
      Serial.print("data: ");
      Serial.println(data);
      return data;
    }
  }

  size_t maxToTcp = 0;
  for (int i = 0; i < MAX_TELNET_CLIENTS; i++) {
    if (serverClients[i]) {
      size_t afw = serverClients[i].availableForWrite();
      if (afw) {
        if (!maxToTcp) {
          maxToTcp = afw;
        } else {
          maxToTcp = std::min(maxToTcp, afw);
        }
      } else {
        Serial.println(F("one client is congested"));
      }
    }
  }

  return "";
}

// String Telnet::value() {
//   String _data = data;
//   data = String();
//   return _data;
// }

void Telnet::println(String text) {
  if (currentClient) {
    currentClient.println(text);
  }
}

void Telnet::print(String text) {
  if (currentClient) {
    currentClient.print(text);
  }
}

void Telnet::println(const char *ptr) { println(FPSTR(ptr)); }
void Telnet::print(const char *ptr) { print(FPSTR(ptr)); }

void Telnet::println(char c) { println(String(c)); }
void Telnet::print(char c) { print(String(c)); }

void Telnet::println(int i) { println((String)i); }
void Telnet::print(int i) { print((String)i); }

void Telnet::print_tnet_new_line() {
  if (currentClient) {
    currentClient.print("\033[1;36m" + String(did) + "\033[0m:/ $ ");
  }
}