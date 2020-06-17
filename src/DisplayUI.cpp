/*
CAUTION: if want to display image into oled
-> convert image to xbm image extension
-> use any text editor to open xbm image
-> copy text inside image paste to your image define
*/

#include "DisplayUI.h"
// #include "images.h"

// ===== adjustable ===== //
void DisplayUI::configInit() {
  display.init();
  display.setFont(DejaVu_Sans_Mono_12);

  // brightness = settings.getBrightness();
  // int _brightness = map(brightness, 0, 100, 0, 255);
  display.setBrightness(255);
  display.setContrast(255);
#ifndef FLIP_DIPLAY
  display.flipScreenVertically();
#endif // ifndef FLIP_DIPLAY

  display.clear();
  display.display();
  setTime(hour(), minute(), second(), day(), month(), year());
}

void DisplayUI::configOn() {
  display.displayOn();
  for (int j = 0; j < screenWidth / 2; j++) {
    display.clear();
    display.drawRect(screenWidth / 2 - j, 32, (j * 2), 1);
    display.display();
  }

  for (int i = 0; i < sreenHeight / 2; i = i + 4) {
    display.clear();
    display.drawRect(-1, sreenHeight / 2 - i, screenWidth + 2, i * 2);
    display.display();
  }
  display.clear();
  display.display();
}

void DisplayUI::configOff() {

  for (int i = 0; i < sreenHeight / 2; i = i + 4) {
    display.clear();
    display.drawRect(-1, i, screenWidth + 2, sreenHeight - (i * 2));
    display.display();
  }
  for (int j = 0; j < screenWidth / 2; j++) {
    display.clear();
    display.drawRect(j, 32, screenWidth - (j * 2), 1);
    display.display();
  }
  display.clear();
  // drawString(2, center("DeepSleep", maxLen));
  display.display();
  display.displayOff();
}

void DisplayUI::updatePrefix() { display.clear(); }

void DisplayUI::updateSuffix() { display.display(); }

void DisplayUI::drawString(int x, int y, String str) {
  display.drawString(x, y, replaceUtf8(str, String(QUESTIONMARK)));
}

void DisplayUI::drawString(int row, String str) {
  drawString(0, row * lineHeight, str);
}

void DisplayUI::drawLine(int x1, int y1, int x2, int y2) {
  display.drawLine(x1, y1, x2, y2);
}

void DisplayUI::drawImage(int16_t xMove, int16_t yMove, int16_t width,
                          int16_t height, const uint8_t *xbm) {
  display.drawXbm(xMove, yMove, width, height, xbm);
}

void DisplayUI::drawImageCenter(int16_t width, int16_t height,
                                const uint8_t *xbm) {
  drawImage(screenWidth / 2 - width / 2, sreenHeight / 2 - height / 2, width,
            height, xbm);
}

void DisplayUI::drawImageCenterHorizontal(int16_t x, int16_t width,
                                          int16_t height, const uint8_t *xbm) {
  drawImage(x, sreenHeight / 2 - height / 2, width, height, xbm);
}
void DisplayUI::drawImageCenterVertical(int16_t y, int16_t width,
                                        int16_t height, const uint8_t *xbm) {
  drawImage(screenWidth / 2 - width / 2, y, width, height, xbm);
}

// ====================== //

DisplayUI::DisplayUI() {}

DisplayUI::~DisplayUI() {}

void DisplayUI::setup() {

  configInit();
  setupButtons();
  buttonTime = currentTime;

  // ===== MENUS ===== //
  //
  // MAIN MENU apple_bits
  createMenu(&mainMenu, NULL, DISPLAY_GUI, [this]() {
    addMenuNode(&mainMenu, scan_bits, D_SCAN, &scanMenu);       /// SCAN
    addMenuNode(&mainMenu, show_bits, D_SHOW, &showMenu);       // SHOW
    addMenuNode(&mainMenu, attack_bits, D_ATTACK, &attackMenu); // ATTACK
    // addMenuNode(&mainMenu, monitor_bits, D_PACKET_MONITOR,
    //             [this]() { // PACKET MONITOR
    //               scan.start(SCAN_MODE_SNIFFER, 0, SCAN_MODE_OFF, 0, false,
    //                          wifi_channel);
    //               mode = DISPLAY_MODE::PACKETMONITOR;
    //             });
    addMenuNode(&mainMenu, captive_portal_bits, D_HACK_AND_CAPTIVE_PORTAL,
                &captiveMenu);

    addMenuNode(&mainMenu, credentail_bits, D_CREDENTIAL, &credentialMenu);

    // addMenuNode(&mainMenu, menu_game_bits, D_GAME, &gameMenu);

    addMenuNode(&mainMenu, wifi_bits, D_WIFI, &wifiMenu);

    addMenuNode(&mainMenu, time_on_screen_bits, D_TIME_ON_SCREEN,
                &timeOnScreenMenu);

    // addMenuNode(&mainMenu, clock_bits, D_SET_CLOCK, [this]() { // PACKET
    // MONITOR
    //   mode = DISPLAY_MODE::SETCLOCK;
    // });

    // addMenuNode(&mainMenu, menu_brightness3_bits, D_BRIGHTNESS,
    //             [this]() { // PACKET MONITOR
    //               mode = DISPLAY_MODE::BRIGHTNESS;
    //             });

    // addMenuNode(&mainMenu, wifi_hacker_bits, D_HACK_WIFI, &wifiHackMenu);

    addMenuNode(&mainMenu, reset_bits, D_RESET, [this]() { // RESET
      if (alert.alertOptions(str(D_RESET), str(D_MESSAGE_RESET_DEVICE),
                             str(D_RESET), str(D_CANCEL_BUTTON))) {
        mode = DISPLAY_MODE::RESET;
        settings.reset();
        settings.save(true);
        alert.showSuccess(str(D_SUCCESS));
        ESP.restart();
      } else {
        changeMenu(&mainMenu);
      }
      configInit();

    });
    // addMenuNode(&mainMenu, reboot_bits, D_REBOOT, [this]() { // REBOOT
    //   if (alert.alertOptions(str(D_TITLE_REBOOT_DEVICE),
    //                          str(D_MESSAGE_REBOOT_DEVICE),
    //                          str(D_REBOOT_BUTTON), str(D_CANCEL_BUTTON))) {
    //     ESP.restart();
    //   } else {
    //     changeMenu(&mainMenu);
    //   }
    //   configInit();
    // });

    addMenuNode(&mainMenu, menu_exit_bits, D_EXIT, [this]() { // REBOOT
      mode = DISPLAY_MODE::HOME;
      display.setFont(DejaVu_Sans_Mono_12);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
    });

#ifdef HIGHLIGHT_LED
    addMenuNode(&mainMenu, D_LED, [this]() { // LED
      highlightLED = !highlightLED;
      digitalWrite(HIGHLIGHT_LED, highlightLED);
    });
#endif // ifdef HIGHLIGHT_LED
  });

  // SCAN MENU
  createMenu(&scanMenu, &mainMenu, DISPLAY_LIST, [this]() {
    addMenuNode(&scanMenu, D_SCAN_APST, [this]() {
      scan.start(SCAN_MODE_ALL, 15000, SCAN_MODE_OFF, 0, true, wifi_channel);
      mode = DISPLAY_MODE::LOADSCAN;
    });
    addMenuNode(&scanMenu, D_SCAN_AP, [this]() {
      scan.start(SCAN_MODE_APS, 0, SCAN_MODE_OFF, 0, true, wifi_channel);
      mode = DISPLAY_MODE::LOADSCAN;
    });
    addMenuNode(&scanMenu, D_SCAN_ST, [this]() {
      scan.start(SCAN_MODE_STATIONS, 30000, SCAN_MODE_OFF, 0, true,
                 wifi_channel);
      mode = DISPLAY_MODE::LOADSCAN;
    });
  });

  // SHOW MENU
  createMenu(&showMenu, &mainMenu, DISPLAY_LIST, [this]() {
    addMenuNode(
        &showMenu,
        [this]() {
          return leftRight(str(D_ACCESSPOINTS), (String)accesspoints.count(),
                           maxLen - 1);
        },
        &apListMenu);
    addMenuNode(
        &showMenu,
        [this]() {
          return leftRight(str(D_STATIONS), (String)stations.count(),
                           maxLen - 1);
        },
        &stationListMenu);
    addMenuNode(
        &showMenu,
        [this]() {
          return leftRight(str(D_NAMES), (String)names.count(), maxLen - 1);
        },
        &nameListMenu);
    addMenuNode(
        &showMenu,
        [this]() {
          return leftRight(str(D_SSIDS), (String)ssids.count(), maxLen - 1);
        },
        &ssidListMenu);
  });

  createMenu(&captiveMenu, &mainMenu, DISPLAY_LIST, [this]() {
    addMenuNode(
        &captiveMenu,
        [this]() {
          return leftRight(str(D_GET_WIFI_SCAN_HACK_WIFI), "", maxLen);
        },
        &wifiListCaptiveScanHackMenu);

    addMenuNode(
        &captiveMenu,
        [this]() { // SSIDs 0
          return leftRight(str(D_CREATE_CAPTIVE_PORTAL), "", maxLen);
        },
        &wifiCaptiveMenu);
  });

  createMenu(&timeOnScreenMenu, &mainMenu, DISPLAY_LIST, [this]() {
    addMenuNode(&timeOnScreenMenu, D_TIME_1M, [this]() {
      settings.setDisplayTimeout(60);
      settings.save(true);
      alert.showSuccess(str(D_SUCCESS_ALERT));
      configInit();
    });
    addMenuNode(&timeOnScreenMenu, D_TIME_2M, [this]() {
      settings.setDisplayTimeout(120);
      settings.save(true);
      alert.showSuccess(str(D_SUCCESS_ALERT));
      configInit();
    });
    addMenuNode(&timeOnScreenMenu, D_TIME_3M, [this]() {
      settings.setDisplayTimeout(180);
      settings.save(true);
      alert.showSuccess(str(D_SUCCESS_ALERT));
      configInit();
    });
    addMenuNode(&timeOnScreenMenu, D_TIME_4M, [this]() {
      settings.setDisplayTimeout(240);
      settings.save(true);
      alert.showSuccess(str(D_SUCCESS_ALERT));
      configInit();
    });
    addMenuNode(&timeOnScreenMenu, D_TIME_5M, [this]() {
      settings.setDisplayTimeout(300);
      settings.save(true);
      alert.showSuccess(str(D_SUCCESS_ALERT));
      configInit();
    });
    addMenuNode(&timeOnScreenMenu, D_TIME_10M, [this]() {
      settings.setDisplayTimeout(600);
      settings.save(true);
      alert.showSuccess(str(D_SUCCESS_ALERT));
      configInit();
    });
  });

  createMenu(&captiveSelectMenu, &wifiCaptiveMenu, DISPLAY_LIST, [this]() {
    addMenuNode(&captiveSelectMenu, D_CAPTIVE_PORTAL_ON_WITH_PHONE, [this]() {
      if (alert.alertOptions(str(D_TITLE_TURN_ON_CAPTIVE),
                             str(D_CAPTIVE_PORTAL_ON_WITH_PHONE),
                             str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
        WiFi.mode(WIFI_AP);
        settings.setCaptivePortal(true);
        settings.save(true);
        settings.setCaptiveType(CAPTIVE_TYPE::PHONE);
        alert.showSuccess(str(D_SUCCESS_ALERT));
      }
      configInit();
    });

    addMenuNode(
        &captiveSelectMenu, D_CAPTIVE_PORTAL_ON_WITH_FACEBOOK, [this]() {
          if (alert.alertOptions(str(D_TITLE_TURN_ON_CAPTIVE),
                                 str(D_CAPTIVE_PORTAL_ON_WITH_FACEBOOK),
                                 str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
            WiFi.mode(WIFI_AP);
            settings.setCaptivePortal(true);
            settings.save(true);
            settings.setCaptiveType(CAPTIVE_TYPE::FACEBOOK);
            alert.showSuccess(str(D_SUCCESS_ALERT));
          }
          configInit();
        });

    addMenuNode(&captiveSelectMenu, D_CAPTIVE_PORTAL_ON_WITH_ZALO, [this]() {
      if (alert.alertOptions(str(D_TITLE_TURN_ON_CAPTIVE),
                             str(D_CAPTIVE_PORTAL_ON_WITH_ZALO),
                             str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
        WiFi.mode(WIFI_AP);
        settings.setCaptivePortal(true);
        settings.save(true);
        settings.setCaptiveType(CAPTIVE_TYPE::ZALO);
        alert.showSuccess(str(D_SUCCESS_ALERT));
      }
      configInit();
    });

    addMenuNode(&captiveSelectMenu, D_CAPTIVE_PORTAL_ON_WITH_TWITTER, [this]() {
      if (alert.alertOptions(str(D_TITLE_TURN_ON_CAPTIVE),
                             str(D_CAPTIVE_PORTAL_ON_WITH_TWITTER),
                             str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
        WiFi.mode(WIFI_AP);
        settings.setCaptivePortal(true);
        settings.save(true);
        settings.setCaptiveType(CAPTIVE_TYPE::TWITTER);
        alert.showSuccess(str(D_SUCCESS_ALERT));
      }
      configInit();
    });

    addMenuNode(&captiveSelectMenu, D_TITLE_TURN_OFF_CAPTIVE, [this]() {
      if (alert.alertOptions(str(D_TITLE_TURN_OFF_CAPTIVE),
                             str(D_CAPTIVE_PORTAL_OFF_MESSAGE),
                             str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
        WiFi.mode(WIFI_AP);
        settings.setCaptivePortal(false);
        settings.save(true);
        alert.showSuccess(str(D_SUCCESS_ALERT));
      }
      configInit();
    });
  });

  createMenu(&credentialMenu, &mainMenu, DISPLAY_GUI, [this]() {
    addMenuNode(&credentialMenu, phone_bits, CLI_PHONE_CREDENTIAL,
                &listPhoneCredential);
    addMenuNode(&credentialMenu, facebook_bits, CLI_FACEBOOK_CREDENTIAL,
                &listFacebookCredential);
    addMenuNode(&credentialMenu, zalo_bits, CLI_ZALO_CREDENTIAL,
                &listZaloCredential);
    // TODO: listTwitterCredential
    addMenuNode(&credentialMenu, twitter_bits, CLI_TWITTER_CREDENTIAL,
                &listTwitterCredential);
    addMenuNode(&credentialMenu, wifi_bits, D_WIFI, &listWifiCredential);
    addMenuNode(&credentialMenu, delete_bits, CLI_DELETE, [this]() {
      if (alert.alertOptions(str(D_TITLE_DELETE_CREDENTIAL), "",
                             str(D_AGREE_ERASE_CREDENTIAL),
                             str(D_CANCEL_BUTTON))) {
        credential.deleteAll();
        alert.showSuccess(str(D_SUCCESS));
      }
      configInit();
      // changeMenu(&credentialMenu);
    });
  });

  // listPhoneCredential
  createMenu(&listPhoneCredential, &credentialMenu, DISPLAY_LIST, [this]() {
    // add APs to list
    int c = credential.count(str(CLI_PHONE_CREDENTIAL));
    for (int i = 0; i < c; i++) {
      addMenuNode(
          &listPhoneCredential,
          [i]() {
            return "name: " + credential.getName(i) +
                   " - phone: " + credential.getPhone(i) + " ";
          },
          [this, i]() {
            if (alert.alertOptions(str(CLI_DELETE),
                                   str(D_TITLE_DELETE_CREDENTIAL),
                                   str(CLI_DELETE), str(D_CANCEL_BUTTON))) {
              credential.deleteIndex(str(CLI_PHONE_CREDENTIAL), i);
              alert.showSuccess(str(D_SUCCESS_ALERT));
            }
            configInit();
            changeMenu(&listPhoneCredential);
          });
    }
    if (c == 0) {
      addMenuNode(&listPhoneCredential, D_EMPTY, [this]() { // SELECT ALL
        changeMenu(&credentialMenu);
      });
    }
  });

  createMenu(&listFacebookCredential, &credentialMenu, DISPLAY_LIST, [this]() {
    // add APs to list
    int c = credential.count(str(CLI_FACEBOOK_CREDENTIAL));
    for (int i = 0; i < c; i++) {
      addMenuNode(
          &listFacebookCredential,
          [i]() {
            return "usr: " +
                   credential.getSocialUser(str(CLI_FACEBOOK_CREDENTIAL), i) +
                   " - pass: " +
                   credential.getSocialPass(str(CLI_FACEBOOK_CREDENTIAL), i) +
                   " ";
          },
          [this, i]() {
            if (alert.alertOptions(str(CLI_DELETE),
                                   str(D_TITLE_DELETE_CREDENTIAL),
                                   str(CLI_DELETE), str(D_CANCEL_BUTTON))) {
              credential.deleteIndex(str(CLI_FACEBOOK_CREDENTIAL), i);
              alert.showSuccess(str(D_SUCCESS_ALERT));
            }
            configInit();
            changeMenu(&listFacebookCredential);
          });
    }
    if (c == 0) {
      addMenuNode(&listFacebookCredential, D_EMPTY, [this]() { // SELECT ALL
        changeMenu(&credentialMenu);
      });
    }
  });

  createMenu(&listZaloCredential, &credentialMenu, DISPLAY_LIST, [this]() {
    // add APs to list
    int c = credential.count(str(CLI_ZALO_CREDENTIAL));
    for (int i = 0; i < c; i++) {
      addMenuNode(
          &listZaloCredential,
          [i]() {
            return "usr: " +
                   credential.getSocialUser(str(CLI_ZALO_CREDENTIAL), i) +
                   " - pass: " +
                   credential.getSocialPass(str(CLI_ZALO_CREDENTIAL), i) + " ";
          },
          [this, i]() {
            if (alert.alertOptions(str(CLI_DELETE),
                                   str(D_TITLE_DELETE_CREDENTIAL),
                                   str(CLI_DELETE), str(D_CANCEL_BUTTON))) {
              credential.deleteIndex(str(CLI_ZALO_CREDENTIAL), i);
              alert.showSuccess(str(D_SUCCESS_ALERT));
            }
            configInit();
            changeMenu(&listZaloCredential);
          });
    }
    if (c == 0) {
      addMenuNode(&listZaloCredential, D_EMPTY, [this]() { // SELECT ALL
        changeMenu(&credentialMenu);
      });
    }
  });

  createMenu(&listTwitterCredential, &credentialMenu, DISPLAY_LIST, [this]() {
    // add APs to list
    int c = credential.count(str(CLI_TWITTER_CREDENTIAL));
    for (int i = 0; i < c; i++) {
      addMenuNode(
          &listTwitterCredential,
          [i]() {
            return "usr: " +
                   credential.getSocialUser(str(CLI_TWITTER_CREDENTIAL), i) +
                   " - pass: " +
                   credential.getSocialPass(str(CLI_TWITTER_CREDENTIAL), i) +
                   " ";
          },
          [this, i]() {
            if (alert.alertOptions(str(CLI_DELETE),
                                   str(D_TITLE_DELETE_CREDENTIAL),
                                   str(CLI_DELETE), str(D_CANCEL_BUTTON))) {
              credential.deleteIndex(str(CLI_TWITTER_CREDENTIAL), i);
              alert.showSuccess(str(D_SUCCESS_ALERT));
            }
            configInit();
            changeMenu(&listTwitterCredential);
          });
    }
    if (c == 0) {
      addMenuNode(&listTwitterCredential, D_EMPTY, [this]() { // SELECT ALL
        changeMenu(&credentialMenu);
      });
    }
  });

  createMenu(&listWifiCredential, &credentialMenu, DISPLAY_LIST, [this]() {
    // add APs to list
    int c = credential.count(str(CLI_WIFI_CREDENTIAL));
    for (int i = 0; i < c; i++) {
      addMenuNode(
          &listWifiCredential,
          [i]() {
            return "ssid: " +
                   credential.getSocialUser(str(CLI_WIFI_CREDENTIAL), i) +
                   " - pass: " +
                   credential.getSocialPass(str(CLI_WIFI_CREDENTIAL), i) + " ";
          },
          [this, i]() {
            if (alert.alertOptions(str(CLI_DELETE),
                                   str(D_TITLE_DELETE_CREDENTIAL),
                                   str(CLI_DELETE), str(D_CANCEL_BUTTON))) {
              credential.deleteIndex(str(CLI_WIFI_CREDENTIAL), i);
              alert.showSuccess(str(D_SUCCESS_ALERT));
            }
            configInit();
            changeMenu(&listWifiCredential);
          });
    }
    if (c == 0) {
      addMenuNode(&listWifiCredential, D_EMPTY, [this]() { // SELECT ALL
        changeMenu(&credentialMenu);
      });
    }
  });
  // TODO:
  // off wifi
  // add new wifi
  // create ap wifi
  // disconnect wifi
  createMenu(&wifiMenu, &mainMenu, DISPLAY_LIST, [this]() {
    if (WiFi.getMode() == WIFI_OFF) {
      addMenuNode(&wifiMenu, D_ON_WIFI, [this]() {
        if (alert.alertOptions(str(D_WIFI), str(D_TITLE_TURN_ON_WIFI),
                               str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
          WiFi.mode(WIFI_AP);
          alert.showSuccess(str(D_SUCCESS_ALERT));
        }
        configInit();
        changeMenu(&wifiMenu);
      });
    } else {
      addMenuNode(&wifiMenu, D_OFF_WIFI, [this]() {
        if (alert.alertOptions(str(D_WIFI), str(D_TITLE_TURN_OFF_WIFI),
                               str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
          WiFi.mode(WIFI_OFF);
          alert.showSuccess(str(D_SUCCESS_ALERT));
        }
        configInit();
        changeMenu(&wifiMenu);
      });
    }

    addMenuNode(&wifiMenu, D_ADD_NEW_WIFI,
                [this]() { mode = DISPLAY_MODE::SMARTCONFIG; });
  });

  createMenu(&wifiListCaptiveScanHackMenu, &captiveMenu, DISPLAY_LIST,
             [this]() {
               int c = accesspoints.count();
               for (int i = 0; i < c; i++) {
                 addMenuNode(
                     &wifiListCaptiveScanHackMenu,
                     [i]() {
                       return b2a(accesspoints.getSelected(i)) +
                              accesspoints.getSSID(i);
                     },
                     [this, i]() {
                       if (attack.isRunning()) {
                         if (alert.alertOptions(
                                 str(D_TITLE_SET_CAPTIVE_STOP_WIFI_HACKING), "",
                                 str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
                           attack.stop();
                         }
                         accesspoints.deselectAll();
                         alert.showSuccess(str(D_SUCCESS_ALERT));
                         configInit();
                       } else {
                         if (alert.alertOptions(str(S_SSID), str(D_HACK_SSID),
                                                str(D_AGREE_BUTTON),
                                                str(D_CANCEL_BUTTON))) {
                           accesspoints.deselectAll();
                           accesspoints.select(i);
                           String ssid = accesspoints.getSSID(i);
                           if (alert.alertOptions(
                                   str(S_SSID), str(D_ADD_EXTENSION_NAME_SSID),
                                   str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
                             ssid = keyboard.show(accesspoints.getSSID(i));
                           }
                           settings.setCaptiveType(CAPTIVE_TYPE::WIFI);
                           settings.setNonePassword(true);
                           settings.setSSID(ssid);
                           credential.setNameWifi(accesspoints.getSSID(i));
                           settings.setChangeSSID();
                           settings.save(true);
                           attack.start(false, true, false, false, true,
                                        settings.getAttackTimeout() * 1000);
                           alert.showSuccess(str(D_SUCCESS_ALERT));
                         }
                         configInit();
                       }
                     },
                     [this, i]() {});
               }
             });

  // add select wifi ssid attack
  // create name ssid
  // select type captive portal
  // attack

  // MARK: TODO
  createMenu(&wifiCaptiveMenu, &captiveMenu, DISPLAY_LIST, [this]() {
    addMenuNode(
        &wifiCaptiveMenu,
        [this]() { return leftRight(str(D_SELECT_VICTIM), "", maxLen); },
        &wifiListCaptiveScanMenu);

    addMenuNode(&wifiCaptiveMenu, D_CREATE_NEW_SSID, [this]() {
      if (alert.alertOptions(str(D_SET_NEW_SSID), settings.getSSID(),
                             str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {

        String ssid = keyboard.show(settings.getSSID());
        if (ssid.length() > 0) {
          if (alert.alertOptions(str(D_SET_PASSWORD), str(D_SET_PASS_FOR_SSID),
                                 str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
            String password = keyboard.show();
            if (password.length() >= 8) {
              settings.setNonePassword(false);
              settings.setPassword(password);
              alert.showSuccess(str(D_SUCCESS_ALERT));
            } else {
              settings.setNonePassword(true);
              alert.showSuccess(str(D_SET_NONE_PASS_FOR_SSID));
            }
          } else {
            settings.setSSID(ssid);
            settings.save(true);
            settings.setNonePassword(true);
            settings.setChangeSSID();
            alert.showSuccess(str(D_SET_NONE_PASS_FOR_SSID));
          }
        }
      } else {
        settings.setNonePassword(true);
        settings.save(true);
        settings.setChangeSSID();
        alert.showSuccess(str(D_SET_NONE_PASS_FOR_SSID));
      }

      changeMenu(&wifiCaptiveMenu);
      configInit();
    });

    addMenuNode(
        &wifiCaptiveMenu,
        [this]() {
          return leftRight(str(D_CAPTIVE_PORTAL_ON_WITH), "", maxLen);
        },
        &captiveSelectMenu);

    addMenuNode(
        &wifiCaptiveMenu,
        (attack.isRunning()) ? (D_STOP_ATTACK) : (D_START_ATTACK),
        [this]() { // SELECT ALL
          if (attack.isRunning()) {
            if (alert.alertOptions(str(D_TITLE_SET_CAPTIVE_STOP_WIFI_HACKING),
                                   "", str(D_AGREE_BUTTON),
                                   str(D_CANCEL_BUTTON))) {
              attack.stop();
            }
            accesspoints.deselectAll();
            alert.showSuccess(str(D_SUCCESS_ALERT));
            configInit();
          } else {
            if (alert.alertOptions(str(S_SSID), str(D_HACK_SSID),
                                   str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
              attack.start(false, true, false, false, true,
                           settings.getAttackTimeout() * 1000);
              alert.showSuccess(str(D_SUCCESS_ALERT));
            }
          }
          changeMenu(&wifiCaptiveMenu);
          configInit();
        });
  });

  createMenu(&wifiListCaptiveScanMenu, &wifiCaptiveMenu, DISPLAY_LIST,
             [this]() {
               int c = accesspoints.count();

               for (int i = 0; i < c; i++) {
                 addMenuNode(
                     &wifiListCaptiveScanMenu,
                     [i]() {
                       return b2a(accesspoints.getSelected(i)) +
                              accesspoints.getSSID(i);
                     },
                     [this, i]() {
                       if (accesspoints.getSelected(i)) {
                         accesspoints.deselectAll();
                         //  settings.
                       } else {
                         accesspoints.deselectAll();
                         accesspoints.select(i);
                         settings.setSSID(accesspoints.getSSID(i));
                       }

                       configInit();
                     },
                     [this, i]() {
                       // selectedID = i;
                       // changeMenu(&apMenu);
                     });
               }
             });

  createMenu(&apListMenu, &showMenu, DISPLAY_LIST, [this]() {
    // add APs to list
    int c = accesspoints.count();

    for (int i = 0; i < c; i++) {
      addMenuNode(
          &apListMenu,
          [i]() {
            return b2a(accesspoints.getSelected(i)) + accesspoints.getSSID(i);
          },
          [this, i]() {
            accesspoints.getSelected(i) ? accesspoints.deselect(i)
                                        : accesspoints.select(i);
          },
          [this, i]() {
            selectedID = i;
            changeMenu(&apMenu);
          });
    }
    addMenuNode(&apListMenu, D_SELECT_ALL, [this]() { // SELECT ALL
      accesspoints.selectAll();
      changeMenu(&apListMenu);
    });
    addMenuNode(&apListMenu, D_DESELECT_ALL, [this]() { // DESELECT ALL
      accesspoints.deselectAll();
      changeMenu(&apListMenu);
    });
    addMenuNode(&apListMenu, D_REMOVE_ALL, [this]() { // REMOVE ALL
      accesspoints.removeAll();
      goBack();
    });
  });

  // STATION LIST MENU
  createMenu(&stationListMenu, &showMenu, DISPLAY_LIST, [this]() {
    // add stations to list
    int c = stations.count();

    for (int i = 0; i < c; i++) {
      addMenuNode(
          &stationListMenu,
          [i]() {
            return b2a(stations.getSelected(i)) +
                   (stations.hasName(i) ? stations.getNameStr(i)
                                        : stations.getMacVendorStr(i));
          },
          [this, i]() {
            stations.getSelected(i) ? stations.deselect(i) : stations.select(i);
          },
          [this, i]() {
            selectedID = i;
            changeMenu(&stationMenu);
          });
    }

    addMenuNode(&stationListMenu, D_SELECT_ALL, [this]() { // SELECT ALL
      stations.selectAll();
      changeMenu(&stationListMenu);
    });
    addMenuNode(&stationListMenu, D_DESELECT_ALL, [this]() { // DESELECT ALL
      stations.deselectAll();
      changeMenu(&stationListMenu);
    });
    addMenuNode(&stationListMenu, D_REMOVE_ALL, [this]() { // REMOVE ALL
      stations.removeAll();
      goBack();
    });
  });

  // NAME LIST MENU
  createMenu(&nameListMenu, &showMenu, DISPLAY_LIST, [this]() {
    // add device names to list
    int c = names.count();

    for (int i = 0; i < c; i++) {
      addMenuNode(
          &nameListMenu,
          [i]() { return names.getSelectedStr(i) + names.getName(i); },
          [this, i]() {
            names.getSelected(i) ? names.deselect(i) : names.select(i);
          },
          [this, i]() {
            selectedID = i;
            changeMenu(&nameMenu);
          });
    }
    addMenuNode(&nameListMenu, D_SELECT_ALL, [this]() { // SELECT ALL
      names.selectAll();
      changeMenu(&nameListMenu);
    });
    addMenuNode(&nameListMenu, D_DESELECT_ALL, [this]() { // DESELECT ALL
      names.deselectAll();
      changeMenu(&nameListMenu);
    });
    addMenuNode(&nameListMenu, D_REMOVE_ALL, [this]() { // REMOVE ALL
      names.removeAll();
      goBack();
    });
  });

  // SSID LIST MENU
  createMenu(&ssidListMenu, &showMenu, DISPLAY_LIST, [this]() {
    addMenuNode(&ssidListMenu, D_CLONE_APS, [this]() { // CLONE APs
      ssids.cloneSelected(true);
      changeMenu(&ssidListMenu);
      ssids.save(false);
    });
    addMenuNode(
        &ssidListMenu,
        [this]() {
          return b2a(ssids.getRandom()) + str(D_RANDOM_MODE); // *RANDOM MODE
        },
        [this]() {
          if (ssids.getRandom())
            ssids.disableRandom();
          else
            ssids.enableRandom(10);
          changeMenu(&ssidListMenu);
        });

    // add ssids to list
    int c = ssids.count();

    for (int i = 0; i < c; i++) {
      addMenuNode(
          &ssidListMenu,
          [i]() { return ssids.getName(i).substring(0, ssids.getLen(i)); },
          [this, i]() {
            selectedID = i;
            changeMenu(&ssidMenu);
          },
          [this, i]() {
            ssids.remove(i);
            changeMenu(&ssidListMenu);
            ssidListMenu.selected = i;
          });
    }

    addMenuNode(&ssidListMenu, D_REMOVE_ALL, [this]() { // REMOVE ALL
      ssids.removeAll();
      goBack();
    });
  });

  // AP MENU
  createMenu(&apMenu, &apListMenu, DISPLAY_LIST, [this]() {
    addMenuNode(
        &apMenu,
        [this]() {
          return accesspoints.getSelectedStr(selectedID) +
                 accesspoints.getSSID(selectedID); // *<ssid>
        },
        [this]() {
          accesspoints.getSelected(selectedID)
              ? accesspoints.deselect(selectedID)
              : accesspoints.select(selectedID);
        });
    addMenuNode(
        &apMenu,
        [this]() {
          return str(D_ENCRYPTION) + accesspoints.getEncStr(selectedID);
        },
        NULL); // Encryption: -/WPA2
    addMenuNode(
        &apMenu,
        [this]() {
          return str(D_RSSI) + (String)accesspoints.getRSSI(selectedID);
        },
        NULL); // RSSI: -90
    addMenuNode(
        &apMenu,
        [this]() {
          return str(D_CHANNEL) + (String)accesspoints.getCh(selectedID);
        },
        NULL); // Channel: 11
    addMenuNode(
        &apMenu, [this]() { return accesspoints.getMacStr(selectedID); },
        NULL); // 00:11:22:00:11:22
    addMenuNode(
        &apMenu,
        [this]() {
          return str(D_VENDOR) + accesspoints.getVendorStr(selectedID);
        },
        NULL); // Vendor: INTEL
    addMenuNode(
        &apMenu,
        [this]() {
          return accesspoints.getSelected(selectedID)
                     ? str(D_DESELECT)
                     : str(D_SELECT); // SELECT/DESELECT
        },
        [this]() {
          accesspoints.getSelected(selectedID)
              ? accesspoints.deselect(selectedID)
              : accesspoints.select(selectedID);
        });
    addMenuNode(&apMenu, D_CLONE, [this]() { // CLONE
      ssids.add(accesspoints.getSSID(selectedID),
                accesspoints.getEnc(selectedID) != ENC_TYPE_NONE, 60, true);
      changeMenu(&showMenu);
      ssids.save(false);
    });
    addMenuNode(&apMenu, D_REMOVE, [this]() { // REMOVE
      accesspoints.remove(selectedID);
      apListMenu.list->remove(apListMenu.selected);
      goBack();
    });
  });

  // STATION MENU
  createMenu(&stationMenu, &stationListMenu, DISPLAY_LIST, [this]() {
    addMenuNode(
        &stationMenu,
        [this]() {
          return stations.getSelectedStr(selectedID) +
                 (stations.hasName(selectedID)
                      ? stations.getNameStr(selectedID)
                      : stations.getMacVendorStr(selectedID)); // <station
                                                               // name>
        },
        [this]() {
          stations.getSelected(selectedID) ? stations.deselect(selectedID)
                                           : stations.select(selectedID);
        });
    addMenuNode(
        &stationMenu, [this]() { return stations.getMacStr(selectedID); },
        NULL); // 00:11:22:00:11:22
    addMenuNode(
        &stationMenu,
        [this]() { return str(D_VENDOR) + stations.getVendorStr(selectedID); },
        NULL); // Vendor: INTEL
    addMenuNode(
        &stationMenu,
        [this]() {
          return str(D_AP) + stations.getAPStr(selectedID); // AP: someAP
        },
        [this]() {
          int apID = accesspoints.find(stations.getAP(selectedID));

          if (apID >= 0) {
            selectedID = apID;
            changeMenu(&apMenu);
          }
        });
    addMenuNode(
        &stationMenu,
        [this]() {
          return str(D_PKTS) + String(*stations.getPkts(selectedID));
        },
        NULL); // Pkts: 12
    addMenuNode(
        &stationMenu,
        [this]() {
          return str(D_CHANNEL) + String(stations.getCh(selectedID));
        },
        NULL); // Channel: 11
    addMenuNode(
        &stationMenu,
        [this]() { return str(D_SEEN) + stations.getTimeStr(selectedID); },
        NULL); // Seen: <1min

    addMenuNode(
        &stationMenu,
        [this]() {
          return stations.getSelected(selectedID)
                     ? str(D_DESELECT)
                     : str(D_SELECT); // SELECT/DESELECT
        },
        [this]() {
          stations.getSelected(selectedID) ? stations.deselect(selectedID)
                                           : stations.select(selectedID);
        });
    addMenuNode(&stationMenu, D_REMOVE, [this]() { // REMOVE
      stations.remove(selectedID);
      stationListMenu.list->remove(stationListMenu.selected);
      goBack();
    });
  });

  // NAME MENU
  createMenu(&nameMenu, &nameListMenu, DISPLAY_LIST, [this]() {
    addMenuNode(
        &nameMenu,
        [this]() {
          return names.getSelectedStr(selectedID) +
                 names.getName(selectedID); // <station name>
        },
        [this]() {
          names.getSelected(selectedID) ? names.deselect(selectedID)
                                        : names.select(selectedID);
        });
    addMenuNode(
        &nameMenu, [this]() { return names.getMacStr(selectedID); },
        NULL); // 00:11:22:00:11:22
    addMenuNode(
        &nameMenu,
        [this]() { return str(D_VENDOR) + names.getVendorStr(selectedID); },
        NULL); // Vendor: INTEL
    addMenuNode(
        &nameMenu,
        [this]() { return str(D_AP) + names.getBssidStr(selectedID); },
        NULL); // AP: 00:11:22:00:11:22
    addMenuNode(
        &nameMenu,
        [this]() { return str(D_CHANNEL) + (String)names.getCh(selectedID); },
        NULL); // Channel: 11

    addMenuNode(
        &nameMenu,
        [this]() {
          return names.getSelected(selectedID)
                     ? str(D_DESELECT)
                     : str(D_SELECT); // SELECT/DESELECT
        },
        [this]() {
          names.getSelected(selectedID) ? names.deselect(selectedID)
                                        : names.select(selectedID);
        });
    addMenuNode(&nameMenu, D_REMOVE, [this]() { // REMOVE
      names.remove(selectedID);
      nameListMenu.list->remove(nameListMenu.selected);
      goBack();
    });
  });

  // SSID MENU
  createMenu(&ssidMenu, &ssidListMenu, DISPLAY_LIST, [this]() {
    addMenuNode(
        &ssidMenu,
        [this]() {
          return ssids.getName(selectedID)
              .substring(0, ssids.getLen(selectedID));
        },
        NULL); // SSID
    addMenuNode(
        &ssidMenu,
        [this]() {
          return str(D_ENCRYPTION) + ssids.getEncStr(selectedID); // WPA2
        },
        [this]() { ssids.setWPA2(selectedID, !ssids.getWPA2(selectedID)); });
    addMenuNode(&ssidMenu, D_REMOVE, [this]() { // REMOVE
      ssids.remove(selectedID);
      ssidListMenu.list->remove(ssidListMenu.selected);
      goBack();
    });
  });

  // Game MENU
  // createMenu(&gameMenu, &mainMenu, DISPLAY_GUI, [this]() {
  //   addMenuNode(&gameMenu, dinosauria_bits, D_GAME_T_REX, [this]() {
  //     mode = DISPLAY_MODE::GAME;
  //     game_type = GAME_TYPE::DINOSAURIA;
  //     trexGame.setup();
  //   });

  // addMenuNode(&gameMenu, flappy_bird_bits, D_GAME_FLAPPY_BIRD, [this]() {
  //   mode = DISPLAY_MODE::GAME;
  //   game_type = GAME_TYPE::FLAPPY_BIRD;
  //   flappyBird.setup();
  // });

  // addMenuNode(&gameMenu, PongGame_bits, D_GAME_PONG, [this]() {
  //   mode = DISPLAY_MODE::GAME;
  //   game_type = GAME_TYPE::PONG;
  //   pongGame.setup();
  // });

  //   addMenuNode(&gameMenu, raceCarGame_bits, D_GAME_CAR_RACE, [this]() {
  //     mode = DISPLAY_MODE::GAME;
  //     game_type = GAME_TYPE::CAR_RACE;
  //     carRaceGame.setup();
  //   });
  // });

  // ATTACK MENU
  createMenu(&attackMenu, &mainMenu, DISPLAY_LIST, [this]() {
    addMenuNode(
        &attackMenu,
        [this]() { // *DEAUTH 0/0
          if (attack.isRunning())
            return leftRight(b2a(deauthSelected) + str(D_DEAUTH),
                             (String)attack.getDeauthPkts() + SLASH +
                                 (String)attack.getDeauthMaxPkts(),
                             maxLen - 1);
          else
            return leftRight(b2a(deauthSelected) + str(D_DEAUTH),
                             (String)scan.countSelected(), maxLen - 1);
        },
        [this]() { // deauth
          deauthSelected = !deauthSelected;

          if (attack.isRunning()) {
            attack.start(beaconSelected, deauthSelected, false, probeSelected,
                         true, settings.getAttackTimeout() * 1000);
          }
        });
    addMenuNode(
        &attackMenu,
        [this]() { // *BEACON 0/0
          if (attack.isRunning())
            return leftRight(b2a(beaconSelected) + str(D_BEACON),
                             (String)attack.getBeaconPkts() + SLASH +
                                 (String)attack.getBeaconMaxPkts(),
                             maxLen - 1);
          else
            return leftRight(b2a(beaconSelected) + str(D_BEACON),
                             (String)ssids.count(), maxLen - 1);
        },
        [this]() { // beacon
          beaconSelected = !beaconSelected;

          if (attack.isRunning()) {
            attack.start(beaconSelected, deauthSelected, false, probeSelected,
                         true, settings.getAttackTimeout() * 1000);
          }
        });
    addMenuNode(
        &attackMenu,
        [this]() { // *PROBE 0/0
          if (attack.isRunning())
            return leftRight(b2a(probeSelected) + str(D_PROBE),
                             (String)attack.getProbePkts() + SLASH +
                                 (String)attack.getProbeMaxPkts(),
                             maxLen - 1);
          else
            return leftRight(b2a(probeSelected) + str(D_PROBE),
                             (String)ssids.count(), maxLen - 1);
        },
        [this]() { // probe
          probeSelected = !probeSelected;

          if (attack.isRunning()) {
            attack.start(beaconSelected, deauthSelected, false, probeSelected,
                         true, settings.getAttackTimeout() * 1000);
          }
        });

    addMenuNode(
        &attackMenu,
        [this]() { // START
          return leftRight(str(D_SET_ATTACK_TIMEOUT),
                           String(settings.getAttackTimeout()) + "s",
                           maxLen - 1);
        },
        [this]() {
          if (alert.alertOptions(str(D_ATTACK), str(D_SET_ATTACK_TIMEOUT),
                                 str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
            String timeout = keyboard.show();

            if (timeout.length() > 0) {
              if (keyboard.isNumber(timeout)) {
                if (timeout.toInt() >= 60) {
                  settings.setAttackTimeout(timeout.toInt());
                  alert.showSuccess(str(D_SUCCESS_ALERT));
                } else {
                  alert.showFailure(str(D_SET_ATTACK_TIMEOUT_MIN_ERROR));
                }
              } else {
                alert.showFailure(str(D_INPUT_NUMBER_WRONG_FORMAT));
              }
            } else {
              alert.showFailure(str(D_INPUT_EMPTY));
            }
          }
          configInit();
        });

    addMenuNode(
        &attackMenu,
        [this]() { // START
          return leftRight(
              str(attack.isRunning() ? D_STOP_ATTACK : D_START_ATTACK),
              attack.getPacketRate() > 0 ? (String)attack.getPacketRate()
                                         : String(),
              maxLen - 1);
        },
        [this]() {
          if (attack.isRunning())
            attack.stop();
          else
            attack.start(beaconSelected, deauthSelected, false, probeSelected,
                         true, settings.getAttackTimeout() * 1000);
        });
  });

  // ===================== //

  // set current menu to main menu
  changeMenu(&mainMenu);
  enabled = true;
  startTime = currentTime;
}

#ifdef HIGHLIGHT_LED
void DisplayUI::setupLED() {
  pinMode(HIGHLIGHT_LED, OUTPUT);
  digitalWrite(HIGHLIGHT_LED, HIGH);
  highlightLED = true;
}

#endif // ifdef HIGHLIGHT_LED

void DisplayUI::update() {
  if (!enabled)
    return;

  up->update();
  down->update();
  a->update();
  b->update();

  draw();

  uint32_t timeout = settings.getDisplayTimeout() * 1000;
  // uint32_t timeout = 5000;

  if (currentTime > timeout) {
    if (!tempOff) {
      if (buttonTime < currentTime - timeout)
        off();
    } else {
      if (buttonTime > currentTime - timeout)
        on();
    }
  }
}

void DisplayUI::on() {
  if (enabled) {
    WiFi.mode(WIFI_AP);
    configOn();
    tempOff = false;
    buttonTime = currentTime; // update a button time to keep display on
    prntln(D_MSG_DISPLAY_ON);
  } else {
    prntln(D_ERROR_NOT_ENABLED);
  }
}

void DisplayUI::off() {
  if (enabled) {
    configOff();
    tempOff = true;
    prntln(D_MSG_DISPLAY_OFF);
    deviceSleep.sleep();
    // configOn();
    // on();
  } else {
    prntln(D_ERROR_NOT_ENABLED);
  }
}

void DisplayUI::setupButtons() {
  up = new ButtonPullup(BUTTON_UP);
  down = new ButtonPullup(BUTTON_DOWN);
  a = new ButtonPullup(BUTTON_A);
  b = new ButtonPullup(BUTTON_B);

  // game.up
  // === BUTTON UP === //
  up->setOnClicked([this]() {
    scrollCounter = 0;
    scrollTime = currentTime;
    buttonTime = currentTime;

    if (!tempOff) {
      // when in menu, go up or down with cursor
      if (mode == DISPLAY_MODE::MENU) {
        if (currentMenu->selected > 0)
          currentMenu->selected--;
        else
          currentMenu->selected = currentMenu->list->size() - 1;
        // when in packet monitor,
      } else if (mode == DISPLAY_MODE::PACKETMONITOR) {
        // change channel
        scan.setChannel(wifi_channel + 1);
        // when in packet monitor, change channel
      } else if (mode == DISPLAY_MODE::HOME) {
        switch (home_mode) {
        case HOME_MODE::HOME_1:
          home_mode = HOME_MODE::HOME_2;
          break;
        case HOME_MODE::HOME_2:
          home_mode = HOME_MODE::HOME_3;
          break;
        case HOME_MODE::HOME_3:
          home_mode = HOME_MODE::HOME_4;
          break;
        default:
          home_mode = HOME_MODE::HOME_1;
          break;
        }
      }
      //  else if (mode == DISPLAY_MODE::SETCLOCK) {
      //   if (clock_set == SET_TIME::SET_HOUR) {
      //     setTime(hour() + 1, minute(), second(), day(), month(), year());
      //   } else if (clock_set == SET_TIME::SET_MINUTE) {
      //     setTime(hour(), minute() + 1, second(), day(), month(), year());
      //   } else if (clock_set == SET_TIME::SET_DAY) {
      //     setTime(hour(), minute(), second(), day() + 1, month(), year());
      //   } else if (clock_set == SET_TIME::SET_MONTH) {
      //     setTime(hour(), minute(), second(), day(), month() + 1, year());
      //   } else if (clock_set == SET_TIME::SET_YEAR) {
      //     setTime(hour(), minute(), second(), day(), month(), year() + 1);
      //   }
      // }
      // else if (mode == DISPLAY_MODE::BRIGHTNESS) {
      //   brightness = brightness - 1;
      //   drawSetBrightness();
      // } else if (mode == DISPLAY_MODE::GAME) {
      // }
    }
  });

  up->setOnHolding(
      [this]() {
        scrollCounter = 0;
        scrollTime = currentTime;
        buttonTime = currentTime;
        if (!tempOff) {
          if (mode == DISPLAY_MODE::MENU) {
            if (currentMenu->selected > 0)
              currentMenu->selected--;
            else
              currentMenu->selected = currentMenu->list->size() - 1;
          } else if (mode == DISPLAY_MODE::PACKETMONITOR) {
            scan.setChannel(wifi_channel + 1);
          }
          // else if (mode == DISPLAY_MODE::SETCLOCK) {
          //   if (clock_set == SET_TIME::SET_HOUR) {
          //     setTime(hour() + 1, minute(), second(), day(), month(),
          //     year());
          //   } else if (clock_set == SET_TIME::SET_MINUTE) {
          //     setTime(hour(), minute() + 1, second(), day(), month(),
          //     year());
          //   } else if (clock_set == SET_TIME::SET_DAY) {
          //     setTime(hour(), minute(), second(), day() + 1, month(),
          //     year());
          //   } else if (clock_set == SET_TIME::SET_MONTH) {
          //     setTime(hour(), minute(), second(), day(), month() + 1,
          //     year());
          //   } else if (clock_set == SET_TIME::SET_YEAR) {
          //     setTime(hour(), minute(), second(), day(), month(), year() +
          //     1);
          //   }
          // }
          // else if (mode == DISPLAY_MODE::BRIGHTNESS) {
          //   brightness = brightness - 1;
          //   drawSetBrightness();
          // } else if (mode == DISPLAY_MODE::GAME) {
          // }
        }
      },
      buttonDelay);

  // === BUTTON DOWN === //
  down->setOnClicked([this]() {
    scrollCounter = 0;
    scrollTime = currentTime;
    buttonTime = currentTime;
    if (!tempOff) {
      if (mode == DISPLAY_MODE::MENU) {
        if (currentMenu->selected < currentMenu->list->size() - 1)
          currentMenu->selected++;
        else
          currentMenu->selected = 0;
      } else if (mode == DISPLAY_MODE::PACKETMONITOR) {
        scan.setChannel(wifi_channel - 1);
      } else if (mode == DISPLAY_MODE::HOME) {
        switch (home_mode) {
        case HOME_MODE::HOME_1:
          home_mode = HOME_MODE::HOME_4;
          break;
        case HOME_MODE::HOME_2:
          home_mode = HOME_MODE::HOME_1;
          break;
        case HOME_MODE::HOME_3:
          home_mode = HOME_MODE::HOME_2;
          break;
        default:
          home_mode = HOME_MODE::HOME_3;
          break;
        }
      }
      // else if (mode == DISPLAY_MODE::SETCLOCK) {
      //   if (clock_set == SET_TIME::SET_HOUR) {
      //     setTime(hour() - 1, minute(), second(), day(), month(), year());
      //   } else if (clock_set == SET_TIME::SET_MINUTE) {
      //     setTime(hour(), minute() - 1, second(), day(), month(), year());
      //   } else if (clock_set == SET_TIME::SET_DAY) {
      //     setTime(hour(), minute(), second(), day() - 1, month(), year());
      //   } else if (clock_set == SET_TIME::SET_MONTH) {
      //     setTime(hour(), minute(), second(), day(), month() - 1, year());
      //   } else if (clock_set == SET_TIME::SET_YEAR) {
      //     setTime(hour(), minute(), second(), day(), month(), year() - 1);
      //   }
      // }
      // else if (mode == DISPLAY_MODE::BRIGHTNESS) {
      //   brightness = brightness + 1;
      //   drawSetBrightness();
      // } else if (mode == DISPLAY_MODE::GAME) {
      //   if (game_type == GAME_TYPE::DINOSAURIA) {
      //     game.update();
      //   } else if (game_type == GAME_TYPE::FLAPPY_BIRD) {
      //     flappyBird.update(FLAPPY_BIRD_KEY_ACTION::ANY_PRESSED);
      //     prntln("FLAPPY_BIRD_KEY_ACTION::ANY_PRESSED");
      //   }
      // }
    }
  });

  down->setOnHolding(
      [this]() {
        scrollCounter = 0;
        scrollTime = currentTime;
        buttonTime = currentTime;
        if (!tempOff) {
          if (mode ==
              DISPLAY_MODE::MENU) { // when in menu, go up or down with cursor
            if (currentMenu->selected < currentMenu->list->size() - 1)
              currentMenu->selected++;
            else
              currentMenu->selected = 0;
          } else if (mode ==
                     DISPLAY_MODE::PACKETMONITOR) { // when in packet monitor,
                                                    // change channel
            scan.setChannel(wifi_channel - 1);
          }

          // else if (mode == DISPLAY_MODE::SETCLOCK) {
          //   if (clock_set == SET_TIME::SET_HOUR) {
          //     setTime(hour() - 1, minute(), second(), day(), month(),
          //     year());
          //   } else if (clock_set == SET_TIME::SET_MINUTE) {
          //     setTime(hour(), minute() - 1, second(), day(), month(),
          //     year());
          //   } else if (clock_set == SET_TIME::SET_DAY) {
          //     setTime(hour(), minute(), second(), day() - 1, month(),
          //     year());
          //   } else if (clock_set == SET_TIME::SET_MONTH) {
          //     setTime(hour(), minute(), second(), day(), month() - 1,
          //     year());
          //   } else if (clock_set == SET_TIME::SET_YEAR) {
          //     setTime(hour(), minute(), second(), day(), month(), year() -
          //     1);
          //   }
          // }
          // else if (mode == DISPLAY_MODE::BRIGHTNESS) {
          //   brightness = brightness + 1;
          //   drawSetBrightness();
          // } else if (mode == DISPLAY_MODE::GAME) {
          //   if (game_type == GAME_TYPE::DINOSAURIA) {
          //     game.update(KEY_ACTION::LEFT_DOWN);
          //     prntln("KEY_ACTION::LEFT_DOWN");
          //   } else if (game_type == GAME_TYPE::FLAPPY_BIRD) {
          //     flappyBird.update(FLAPPY_BIRD_KEY_ACTION::ANY_PRESSED);
          //     prntln("FLAPPY_BIRD_KEY_ACTION::ANY_PRESSED");
          //   }
          // }
        }
      },
      buttonDelay);

  // === BUTTON A === //
  a->setOnClicked([this]() {
    scrollCounter = 0;
    scrollTime = currentTime;
    buttonTime = currentTime;
    if (!tempOff) {
      switch (mode) {
      case DISPLAY_MODE::MENU:
        if (currentMenu->list->get(currentMenu->selected).click) {
          currentMenu->list->get(currentMenu->selected).click();
        }
        break;

      case DISPLAY_MODE::PACKETMONITOR:
      case DISPLAY_MODE::LOADSCAN:
        scan.stop();
        mode = DISPLAY_MODE::MENU;
        break;

        // case DISPLAY_MODE::BRIGHTNESS:
        //   mode = DISPLAY_MODE::MENU;
        //   break;
        // case DISPLAY_MODE::SETCLOCK:
        //   if (clock_set == SET_TIME::SET_HOUR) {
        //     clock_set = SET_TIME::SET_MINUTE;
        //   } else if (clock_set == SET_TIME::SET_MINUTE) {
        //     clock_set = SET_TIME::SET_DAY;
        //   } else if (clock_set == SET_TIME::SET_DAY) {
        //     clock_set = SET_TIME::SET_MONTH;
        //   } else if (clock_set == SET_TIME::SET_MONTH) {
        //     clock_set = SET_TIME::SET_YEAR;
        //   } else if (clock_set == SET_TIME::SET_YEAR) {
        //     clock_set = SET_TIME::SET_HOUR;
        //   }
        //   break;

      case DISPLAY_MODE::HOME:
        mode = DISPLAY_MODE::MENU;
        // display.setFont(DejaVu_Sans_Mono_12);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        break;
      case DISPLAY_MODE::GAME:
        mode = DISPLAY_MODE::MENU;
        configInit();
        break;
      }
    }
  });

  a->setOnHolding(
      [this]() {
        scrollCounter = 0;
        scrollTime = currentTime;
        buttonTime = currentTime;
        if (!tempOff) {
          if (mode == DISPLAY_MODE::MENU) {
            if (currentMenu->list->get(currentMenu->selected).hold) {
              currentMenu->list->get(currentMenu->selected).hold();
            }
          }
          // else if (mode == DISPLAY_MODE::SETCLOCK) {
          //   mode = DISPLAY_MODE::MENU;
          // }
        }
      },
      800);

  // === BUTTON B === //
  b->setOnClicked([this]() {
    scrollCounter = 0;
    scrollTime = currentTime;
    buttonTime = currentTime;
    if (!tempOff) {
      switch (mode) {
      case DISPLAY_MODE::MENU:
        goBack();
        break;

      case DISPLAY_MODE::PACKETMONITOR:
      case DISPLAY_MODE::LOADSCAN:
        scan.stop();
        mode = DISPLAY_MODE::MENU;
        break;

      case DISPLAY_MODE::HOME:
        mode = DISPLAY_MODE::MENU;
        break;
      }
    }
  });
}

String DisplayUI::getChannel() {
  String ch = String(wifi_channel);

  if (ch.length() < 2)
    ch = ' ' + ch;
  return ch;
}

void DisplayUI::draw() {
  if ((currentTime - drawTime > drawInterval) && currentMenu) {
    drawTime = currentTime;

    updatePrefix();

    switch (mode) {
    case DISPLAY_MODE::BUTTON_TEST:
      drawButtonTest();
      break;

    case DISPLAY_MODE::MENU:
      drawMenu();
      break;

    case DISPLAY_MODE::LOADSCAN:
      drawLoadingScan();
      break;

      // case DISPLAY_MODE::BRIGHTNESS:
      //   drawSetBrightness();
      //   break;

    case DISPLAY_MODE::SMARTCONFIG:
      drawSmartconfig();
      break;

    case DISPLAY_MODE::PACKETMONITOR:
      drawPacketMonitor();
      break;

    case DISPLAY_MODE::INTRO:
      if (currentTime - startTime >= screenIntroTime) {
        mode = DISPLAY_MODE::HOME;
      }
      drawIntro();
      break;
    case DISPLAY_MODE::HOME:
      drawHome();
      break;
      // case DISPLAY_MODE::SETCLOCK:
      //   setClock();
      //   break;
      // case DISPLAY_MODE::GAME:
      //   if (game_type == GAME_TYPE::DINOSAURIA) {
      //     if (trexGame.update()) {
      //       buttonTime = currentTime;
      //       mode = DISPLAY_MODE::MENU;
      //       game_type = GAME_TYPE::NONE_GAME;
      //       changeMenu(&gameMenu);
      //       configInit();
      //       // delete trexGame;
      //     }
      //   }
      // else if (game_type == GAME_TYPE::FLAPPY_BIRD) {
      //   if (flappyBird.update()) {
      //     buttonTime = currentTime;
      //     mode = DISPLAY_MODE::MENU;
      //     game_type = GAME_TYPE::NONE_GAME;
      //     changeMenu(&gameMenu);
      //     configInit();
      //   }
      // }
      // else if (game_type == GAME_TYPE::PONG) {
      //   if (pongGame.update()) {
      //     buttonTime = currentTime;
      //     mode = DISPLAY_MODE::MENU;
      //     game_type = GAME_TYPE::NONE_GAME;
      //     changeMenu(&gameMenu);
      //     configInit();
      //   }
      // }
      // else if (game_type == GAME_TYPE::CAR_RACE) {
      //   if (carRaceGame.update()) {
      //     buttonTime = currentTime;
      //     mode = DISPLAY_MODE::MENU;
      //     game_type = GAME_TYPE::NONE_GAME;
      //     changeMenu(&gameMenu);
      //     configInit();
      //   }
      // }
      // break;
    }
    updateSuffix();
  }
}

void DisplayUI::drawButtonTest() {
  drawString(0, str(D_UP) + b2s(up->read()));
  drawString(1, str(D_DOWN) + b2s(down->read()));
  drawString(2, str(D_A) + b2s(a->read()));
  drawString(3, str(D_B) + b2s(b->read()));
}

void DisplayUI::drawMenu() {
  if (currentMenu->isGUI) {
    display.setFont(DejaVu_Sans_Mono_10);
    drawGUI();
  } else {
    display.setFont(DejaVu_Sans_Mono_12);
    drawList();
  }
}

void DisplayUI::drawList() {
  String tmp;
  int tmpLen;
  int row = (currentMenu->selected / 5) * 5;
  // correct selected if it's off
  if (currentMenu->selected < 0)
    currentMenu->selected = 0;
  else if (currentMenu->selected >= currentMenu->list->size())
    currentMenu->selected = currentMenu->list->size() - 1;

  // draw menu entries
  for (int i = row; i < currentMenu->list->size() && i < row + 5; i++) {
    tmp = currentMenu->list->get(i).getStr();
    tmpLen = tmp.length();

    // horizontal scrolling
    if ((currentMenu->selected == i) && (tmpLen >= maxLen)) {
      tmp = tmp + tmp;
      tmp = tmp.substring(scrollCounter, scrollCounter + maxLen - 1);

      if ((scrollCounter > 0 && scrollTime < currentTime - scrollSpeed) ||
          (scrollCounter == 0 && scrollTime < currentTime - scrollSpeed * 4)) {
        scrollTime = currentTime;
        scrollCounter++;
      }

      if (scrollCounter > tmpLen)
        scrollCounter = 0;
    }

    tmp = (currentMenu->selected == i ? CURSOR : SPACE) + tmp;
    drawString(0, (i - row) * 12, tmp);
  }
}

void DisplayUI::drawGUI() {
  if (currentMenu->selected < 0) {
    currentMenu->selected = 0;
    // if (currentMenu == &credentialMenu) {
    //   mode = DISPLAY_MODE::HOME;
    // }

  } else if (currentMenu->selected >= currentMenu->list->size()) {
    currentMenu->selected = currentMenu->list->size() - 1;
    // if (currentMenu == &credentialMenu) {
    //   mode = DISPLAY_MODE::HOME;
    // }
  }

  int size = currentMenu->list->size();
  int row = (currentMenu->selected);

  if (currentMenu->list->size() > 0) {
    if (row > 0) {
      drawImage(6, 16, menu_image_width, menu_image_height,
                currentMenu->list->get(row - 1).image);
    }

    if (row < size - 1) {
      drawImage(90, 16, menu_image_width, menu_image_height,
                currentMenu->list->get(row + 1).image);
    }
    drawImageCenter(menu_image_width, menu_image_height,
                    currentMenu->list->get(row).image);

    drawString(0, 54, currentMenu->list->get(row).getStr());
  }
  drawImage(46, 14, selectbar_top_width, selectbar_top_height,
            selectbar_top_bits);
  drawImage(46, 42, selectbar_bottom_width, selectbar_bottom_height,
            selectbar_bottom_bits);
}

// void DisplayUI::drawSetBrightness() {
//   if (brightness >= 100) {
//     brightness = 100;
//   }

//   if (brightness <= 0) {
//     brightness = 0;
//   }

//   display.setFont(DejaVu_Sans_Mono_12);
//   display.drawProgressBar(4, 32, 120, 8, brightness);
//   int _brightness = map(brightness, 0, 100, 0, 255);

//   if (oldBrighness != brightness) {
//     oldBrighness = brightness;
//     // display.setContrast(_brightness);
//     display.setBrightness(_brightness);
//     settings.setBrightness(brightness);
//     settings.save(true);
//   }

//   drawString(4, center(str(D_BRIGHTNESS), maxLen));
//   drawString(1, leftRight("0", String(brightness), maxLen));
//   display.display();
// }

String DisplayUI::getStringWeek() {
  switch (weekday()) {
  case 1:
    return str(CLOCK_SUNDAY);
  case 2:
    return str(CLOCK_MONDAY);
  case 3:
    return str(CLOCK_TUESDAY);
  case 4:
    return str(CLOCK_WEDNESDAY);
  case 5:
    return str(CLOCK_THURSDAY);
  case 6:
    return str(CLOCK_FRIDAY);
  default:
    return str(CLOCK_SATURDAY);
  }
}

// void DisplayUI::setClock() {
//   display.setFont(Montez_Regular_32);
//   String clockTime = String(hour());

//   if (hour() < 10) {
//     clockTime = '0' + clockTime;
//   }
//   clockTime += ':';
//   if (minute() < 10)
//     clockTime += '0';
//   clockTime += String(minute());
//   display.drawString(32, 10, clockTime);

//   display.setFont(DejaVu_Sans_Mono_10);
//   drawString(0, 50, getStringWeek());
//   String dateTime = String(day());

//   if (day() < 10) { // 0 + 1 = 01
//     dateTime = "0" + dateTime;
//   }

//   dateTime = dateTime + DASH;  // 01+- = 01-
//   if (month() < 10) {          //
//     dateTime = dateTime + "0"; // 01-+0 = 01-0
//   }
//   dateTime = dateTime + String(month()) + DASH + String(year()); //
//   01-01-2020

//   drawString(64, 50, dateTime);

//   if (clock_set == SET_TIME::SET_HOUR) {
//     display.fillRect(32, 42, 30, 2);
//   } else if (clock_set == SET_TIME::SET_MINUTE) {
//     display.fillRect(70, 42, 30, 2);
//   } else if (clock_set == SET_TIME::SET_WEEK) {
//     display.fillRect(0, 62, 35, 2);
//   } else if (clock_set == SET_TIME::SET_DAY) {
//     display.fillRect(64, 62, 13, 2);
//   } else if (clock_set == SET_TIME::SET_MONTH) {
//     display.fillRect(83, 62, 13, 2);
//   } else if (clock_set == SET_TIME::SET_YEAR) {
//     display.fillRect(100, 62, 25, 2);
//   }
// }

void DisplayUI::drawLoadingScan() {
  String percentage;

  if (scan.isScanning()) {
    percentage = String(scan.getPercentage()) + '%';
  } else {
    percentage = str(DSP_SCAN_DONE);
  }

  drawString(0, leftRight(str(DSP_SCAN_FOR), scan.getMode(), maxLen));
  drawString(1, leftRight(str(DSP_APS), String(accesspoints.count()), maxLen));
  drawString(2, leftRight(str(DSP_STS), String(stations.count()), maxLen));
  drawString(3, leftRight(str(DSP_PKTS),
                          String(scan.getPacketRate()) + str(DSP_S), maxLen));
  drawString(4, center(percentage, maxLen));
}

void DisplayUI::drawPacketMonitor() {
  // double scale = scan.getScaleFactor(sreenHeight - lineHeight - 2);

  // String headline =
  //     leftRight(str(D_CH) + getChannel() + String(' ') + String('[') +
  //                   String(scan.deauths) + String(']'),
  //               String(scan.getPacketRate()) + str(D_PKTS), maxLen);

  // drawString(0, 0, headline);

  // if (scan.getMaxPacket() > 0) {
  //   int i = 0;
  //   int x = 0;
  //   int y = 0;
  //   while (i < SCAN_PACKET_LIST_SIZE && x < screenWidth) {
  //     y = (sreenHeight - 1) - (scan.getPackets(i) * scale);
  //     i++;

  //     // Serial.printf("%d,%d -> %d,%d\n", x, (sreenHeight-1), x, y);
  //     drawLine(x, (sreenHeight - 1), x, y);
  //     x++;

  //     // Serial.printf("%d,%d -> %d,%d\n", x, (sreenHeight-1), x, y);
  //     drawLine(x, (sreenHeight - 1), x, y);
  //     x++;
  //   }
  //   // Serial.println("---------");
  // }
}

void DisplayUI::drawIntro() {
  // if (isFistOnScreen) {
  //   isFistOnScreen = false;
  //   configOn();
  //   drawString(0, center(F("esp8266 deauther"), maxLen));
  //   drawString(1, center(F("please use for education"), maxLen));
  //   drawString(2, center(F("and"), maxLen));
  //   drawString(3, center(F("research"), maxLen));
  //   drawString(4, center(F("thanks"), maxLen));
  //   display.display();
  // drawImageCenterVertical(10, apple_width, apple_height, apple_bits);
  // for (int i = 0; i < 100; i++) {
  //   display.drawProgressBar(0, 50, screenWidth - 1, 8, i);
  //   delay(20);
  //   display.display();
  // }
  // }
  display.setFont(DejaVu_Sans_Mono_10);
  drawString(0, center(F("esp8266 deauther"), 20));
  drawString(1, center(F("please just use for"), 20));
  drawString(2, center(F("education and"), 20));
  drawString(3, center(F("research"), 20));
  drawString(4, center(F("thanks"), 20));
}

void DisplayUI::drawSmartconfig() {
  if (alert.alertOptions(str(D_WIFI), str(D_WIFI_SMARTCONFIG_CONNECT),
                         str(D_AGREE_BUTTON), str(D_CANCEL_BUTTON))) {
    if (wifiConfig.smartconfig()) {
      alert.alertNotify(str(D_WIFI), str(D_WIFI_CONNECTED),
                        str(D_AGREE_BUTTON));
    } else {
      alert.alertNotify(str(D_WIFI), str(D_WIFI_CONNECT_FALSE),
                        str(D_AGREE_BUTTON));
    }
  }
  configInit();
  mode = DISPLAY_MODE::MENU;
  changeMenu(&wifiMenu);
}

void DisplayUI::drawHome() {
  switch (home_mode) {
  case HOME_MODE::HOME_1:
    drawSystemInfo();
    break;
  case HOME_MODE::HOME_2:
    drawHome2();
    break;
  case HOME_MODE::HOME_3:
    drawHome3();
    break;
  default:
    drawHome4();
    break;
  }
}

void DisplayUI::drawHome2() { drawCntAccount(); }
void DisplayUI::drawHome3() { drawDeviceInfo(); }
void DisplayUI::drawHome4() {
  display.setFont(Montez_Regular_32);
  String clockTime = String(hour());

  clockTime += ':';
  if (minute() < 10)
    clockTime += '0';
  clockTime += String(minute());
  clockTime += ':';
  if (second() < 10)
    clockTime += '0';
  clockTime += String(second());
  display.drawString(32, 13, clockTime);

  int cnt_facebook = credential.count(str(CLI_FACEBOOK_CREDENTIAL));
  int cnt_phone = credential.count(str(CLI_PHONE_CREDENTIAL));
  int cnt_zalo = credential.count(str(CLI_ZALO_CREDENTIAL));
  int cnt_twitter = credential.count(str(CLI_TWITTER_CREDENTIAL));
  int cnt_wifi = credential.count(str(CLI_WIFI_CREDENTIAL));

  // drawString(1, left(clockTime, maxLen));
  display.setFont(DejaVu_Sans_Mono_10);
  String info = "F:" + String(cnt_facebook) + " " + "Z:" + String(cnt_zalo) +
                " " + "P:" + String(cnt_phone) + " " +
                "T:" + String(cnt_twitter) + " " + "W:" + String(cnt_wifi);
  display.drawString(0, 44, info);

  display.setFont(DejaVu_Sans_Mono_10);
  drawString(0, 54, getStringWeek());
  String dateTime = String(day());

  if (day() < 10) { // 0 + 1 = 01
    dateTime = "0" + dateTime;
  }

  dateTime = dateTime + DASH;  // 01+- = 01-
  if (month() < 10) {          //
    dateTime = dateTime + "0"; // 01-+0 = 01-0
  }
  dateTime = dateTime + String(month()) + DASH + String(year()); // 01-01-2020
  drawString(64, 54, dateTime);

  display.setFont(DejaVu_Sans_Mono_10);
  String infoWifi = "W:";
  if (WiFi.getMode() == WIFI_AP) {
    infoWifi = infoWifi + "ap";
  } else if (WiFi.getMode() == WIFI_AP_STA) {
    infoWifi = infoWifi + "as";
  } else if (WiFi.getMode() == WIFI_STA) {
    infoWifi = infoWifi + "sta";
  } else {
    infoWifi = infoWifi + "off";
  }

  if (settings.captivePortal) {
    display.drawString(0, 0, settings.getSSID());
  } else {
    drawImage(screenWidth - battIconFull_width, 0, battIconFull_width,
              battIconFull_height, battIconFull_bits);
    drawImage(screenWidth - battIconFull_width - usbIcon_width, 0,
              usbIcon_width, usbIcon_height, usbIcon_bits);
  }

  display.drawString(0, 16, infoWifi);
  String captive_portal_info =
      "C:" + String((settings.captivePortal) ? "on" : "off");
  display.drawString(0, 26, captive_portal_info);
}

// void DisplayUI::drawClock() {
//   display.setFont(Montez_Regular_32);
//   String clockTime = String(hour());

//   clockTime += ':';
//   if (minute() < 10)
//     clockTime += '0';
//   clockTime += String(minute());
//   clockTime += ':';
//   if (second() < 10)
//     clockTime += '0';
//   clockTime += String(second());
//   display.drawString(28, 13, clockTime);
// }

void DisplayUI::drawCntAccount() {
  int cnt_facebook = credential.count(str(CLI_FACEBOOK_CREDENTIAL));
  int cnt_phone = credential.count(str(CLI_PHONE_CREDENTIAL));
  int cnt_zalo = credential.count(str(CLI_ZALO_CREDENTIAL));
  int cnt_twitter = credential.count(str(CLI_TWITTER_CREDENTIAL));
  int cnt_wifi = credential.count(str(CLI_WIFI_CREDENTIAL));

  // drawString(1, left(clockTime, maxLen));
  display.setFont(DejaVu_Sans_Mono_10);

  drawString(
      0, leftRight(str(CLI_FACEBOOK_CREDENTIAL), String(cnt_facebook), maxLen));
  drawString(1, leftRight(str(CLI_ZALO_CREDENTIAL), String(cnt_zalo), maxLen));
  drawString(2,
             leftRight(str(CLI_PHONE_CREDENTIAL), String(cnt_phone), maxLen));
  drawString(
      3, leftRight(str(CLI_TWITTER_CREDENTIAL), String(cnt_twitter), maxLen));
  drawString(4, leftRight(str(CLI_WIFI_CREDENTIAL), String(cnt_wifi), maxLen));
}

// void DisplayUI::drawDateTime() {
//   display.setFont(DejaVu_Sans_Mono_10);
//   drawString(0, 0, getStringWeek());
//   // drawString(0, center(week, maxLen));
//   String dateTime = String(day());

//   if (day() < 10) { // 0 + 1 = 01
//     dateTime = "0" + dateTime;
//   }

//   dateTime = dateTime + DASH;  // 01+- = 01-
//   if (month() < 10) {          //
//     dateTime = dateTime + "0"; // 01-+0 = 01-0
//   }
//   dateTime = dateTime + String(month()) + DASH + String(year()); //
//   01-01-2020 drawString(64, 54, dateTime);
//   // drawString(4, center(dateTime, maxLen));
// }

void DisplayUI::drawSystemInfo() {
  // char s[150];
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  display.setFont(DejaVu_Sans_Mono_10);
  drawString(0, center("system info", 20));
  drawString(1, center("RAM", 20));
  drawString(1, leftRight("type", "FS", 20));

  drawString(2, center(String(81920 / 1024) + "Kb", 20));
  drawString(2,
             leftRight("total", String(fs_info.totalBytes / 1024) + "Kb", 20));

  drawString(
      3, center(String(100 - system_get_free_heap_size() / (81920 / 100)) + "%",
                20));
  drawString(
      3, leftRight("usage",
                   String(fs_info.usedBytes / (fs_info.totalBytes / 100)) + "%",
                   20));

  drawString(
      4, center(String(system_get_free_heap_size() / (81920 / 100)) + "%", 20));
  drawString(4, leftRight("free",
                          String((fs_info.totalBytes - fs_info.usedBytes) /
                                 (fs_info.totalBytes / 100)) +
                              "%",
                          20));

  // drawString(4, center("RAM", 20));
  // drawString(4, leftRight("system", "FS", 20));

  // sprintf(s, str(RAM_TOTAL).c_str(), 81920);
  // drawString(1, center(s, maxLen));
  // // prntln(String(s));
  // sprintf(s, str(RAM_USE_FREE).c_str(), 81920 - system_get_free_heap_size(),
  //         100 - system_get_free_heap_size() / (81920 / 100),
  //         system_get_free_heap_size(),
  //         system_get_free_heap_size() / (81920 / 100));
  // drawString(2, center(s, maxLen));

  // sprintf(s, str(SPIFFS_TOTAL).c_str(), fs_info.totalBytes);
  // drawString(3, center(s, maxLen));
  // sprintf(s, str(SPIFFS_USE_FREE).c_str(), fs_info.usedBytes,
  //         fs_info.usedBytes / (fs_info.totalBytes / 100),
  //         fs_info.totalBytes - fs_info.usedBytes,
  //         (fs_info.totalBytes - fs_info.usedBytes) /
  //             (fs_info.totalBytes / 100));
  // drawString(4, center(s, maxLen));
  // delay(200);
}

void DisplayUI::drawDeviceInfo() {
  display.setFont(DejaVu_Sans_Mono_10);
  String mode = String();
  String ip;
  if (WiFi.getMode() == WIFI_AP) {
    mode = str(D_WIFI_MODE_AP);
    ip = WiFi.softAPIP().toString();
  } else if (WiFi.getMode() == WIFI_AP_STA) {
    mode = str(D_WIFI_MODE_AP_STA);
    ip = WiFi.softAPIP().toString();
  } else if (WiFi.getMode() == WIFI_STA) {
    mode = str(D_WIFI_MODE_STA);
    ip = WiFi.localIP().toString();
  } else {
    mode = str(D_WIFI_MODE_OFF);
    ip = "";
  }
  String captive_portal_state =
      String((settings.captivePortal) ? str(CLI_ON) : str(CLI_OFF));
  String deauther_state =
      String(attack.isRunning() ? str(CLI_ON) : str(CLI_OFF));
  String captive_mode_state;
  int captive_type = settings.getCaptiveType();
  switch (captive_type) {
  case CAPTIVE_TYPE::FACEBOOK:
    captive_mode_state = str(CLI_FACEBOOK_CREDENTIAL);
    break;
  case CAPTIVE_TYPE::ZALO:
    captive_mode_state = str(CLI_ZALO_CREDENTIAL);
    break;
  case CAPTIVE_TYPE::TWITTER:
    captive_mode_state = str(CLI_TWITTER_CREDENTIAL);
    break;
  case CAPTIVE_TYPE::PHONE:
    captive_mode_state = str(CLI_PHONE_CREDENTIAL);
    break;
  default:
    captive_mode_state = str(CLI_WIFI_CREDENTIAL);
    break;
  }
  // drawString(0, center(ip, maxLen));
  drawString(0, leftRight(str(D_IP_NAME), ip, maxLen));
  drawString(1, leftRight(str(CLI_WIFI_CREDENTIAL), mode, maxLen));
  drawString(2, leftRight(str(D_CAPTIVE_NAME), captive_portal_state, maxLen));
  drawString(3, leftRight(str(D_CREDENTIAL_WITH), captive_mode_state, maxLen));
  drawString(4, leftRight(str(D_DEAUTHER_STATUS), deauther_state, maxLen));
}

void DisplayUI::clearMenu(Menu *menu) {
  while (menu->list->size() > 0) {
    menu->list->remove(0);
  }
}

void DisplayUI::changeMenu(Menu *menu) {
  if (menu) {
    // only open list menu if it has nodes
    if (((menu == &apListMenu) && (accesspoints.count() == 0)) ||
        ((menu == &stationListMenu) && (stations.count() == 0)) ||
        ((menu == &nameListMenu) && (names.count() == 0))) {
      return;
    }
    if (currentMenu)
      clearMenu(currentMenu);
    currentMenu = menu;
    // currentMenu->selected = 0;

    buttonTime = currentTime;

    if (selectedID < 0)
      selectedID = 0;

    if (currentMenu->parentMenu) {
      addMenuNode(currentMenu, D_BACK, currentMenu->parentMenu); // add [BACK]
      currentMenu->selected = 1;
    }

    if (currentMenu->build)
      currentMenu->build();

    // drawImage(46, 14, selectbar_top_width, selectbar_top_height,
    //           selectbar_top_bits);
    // drawImage(46, 42, selectbar_bottom_width, selectbar_bottom_height,
    //           selectbar_bottom_bits);
  }
}

void DisplayUI::goBack() {
  if (currentMenu->parentMenu)
    changeMenu(currentMenu->parentMenu);
}

void DisplayUI::createMenu(Menu *menu, Menu *parent, bool isGUI,
                           std::function<void()> build) {
  menu->list = new SimpleList<MenuNode>;
  menu->parentMenu = parent;
  menu->selected = 0;
  menu->isGUI = isGUI;
  menu->build = build;
}

void DisplayUI::addMenuNode(Menu *menu, const uint8_t image[], const char *ptr,
                            std::function<void()> click,
                            std::function<void()> hold) {
  menu->list->add(MenuNode{image, [ptr]() { return str(ptr); }, click, hold});
}

void DisplayUI::addMenuNode(Menu *menu, const uint8_t image[], const char *ptr,
                            std::function<void()> click) {
  addMenuNode(menu, image, ptr, click, NULL);
}

void DisplayUI::addMenuNode(Menu *menu, const uint8_t image[], const char *ptr,
                            Menu *next) {
  addMenuNode(
      menu, image, ptr, [this, next]() { changeMenu(next); }, NULL);
}

void DisplayUI::addMenuNode(Menu *menu, std::function<String()> getStr,
                            std::function<void()> click,
                            std::function<void()> hold) {
  menu->list->add(MenuNode{menu_exit_bits, getStr, click, hold});
}

void DisplayUI::addMenuNode(Menu *menu, std::function<String()> getStr,
                            std::function<void()> click) {
  addMenuNode(menu, getStr, click, NULL);
}

void DisplayUI::addMenuNode(Menu *menu, std::function<String()> getStr,
                            Menu *next) {
  addMenuNode(menu, getStr, [this, next]() { changeMenu(next); });
}

void DisplayUI::addMenuNode(Menu *menu, const char *ptr,
                            std::function<void()> click) {
  addMenuNode(
      menu, [ptr]() { return str(ptr); }, click);
}

void DisplayUI::addMenuNode(Menu *menu, const char *ptr, Menu *next) {
  addMenuNode(
      menu, [ptr]() { return str(ptr); }, next);
}
