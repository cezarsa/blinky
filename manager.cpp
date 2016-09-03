#include <DNSServer.h>
#include <WiFiManager.h>

#include "common.hpp"

String configSSID() { return String("esp-") + ESP.getChipId(); }

bool runManager() {
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);
  if (!wifiManager.autoConnect(configSSID().c_str())) {
    Serial.println("failed to connect and hit timeout");
    delay(1000);
    ESP.restart();
    return false;
  }
  Serial.print("Connected with IP:");
  Serial.print(WiFi.localIP());
  return true;
}

void forceManager() {
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);
  if (!wifiManager.startConfigPortal(configSSID().c_str())) {
    Serial.println("failed to connect in reconfig and hit timeout");
  }
  Serial.println("reconfig done, restarting");
  delay(1000);
  ESP.restart();
}
