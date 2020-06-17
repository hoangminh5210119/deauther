
#include "DeviceSleep.h"

DeviceSleep::DeviceSleep(/* args */) {}

DeviceSleep::~DeviceSleep() {}

void DeviceSleep::setup() {
  gpio_init();
  wifi_set_sleep_type(LIGHT_SLEEP_T);
}
void DeviceSleep::sleep() {
  ESP.deepSleep(10000000);
}
