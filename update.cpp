#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>

#include "common.hpp"
#include "config.h"

unsigned long lastCheck = 0;

void doUpdate(String expectedMD5) {
  int totalSize;
  HTTPClient http;
  bool success = false;

  http.begin(FIRMWARE_URL);
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("Firmware HTTP GET failed, error: %s\n",
                  http.errorToString(httpCode).c_str());
    goto done;
  }

  totalSize = http.getSize();
  if (totalSize == 0) {
    Serial.println("Firmware GET failed, empty Content-Length");
    goto done;
  }

  Serial.setDebugOutput(true);
  WiFiUDP::stopAll();
  if (!Update.setMD5(expectedMD5.c_str())) {
    Update.printError(Serial);
    goto doneDebug;
  }

  if (!Update.begin(totalSize)) {
    Update.printError(Serial);
    goto doneDebug;
  }

  Update.writeStream(*http.getStreamPtr());
  if (!Update.end(false)) {
    Update.printError(Serial);
    goto doneDebug;
  }
  Serial.printf("\nUpdate Success: %u\nRebooting...\n", totalSize);
  delay(1000);
  ESP.reset();
  return;

doneDebug:
  Serial.setDebugOutput(false);
done:
  http.end();
}

void checkFirmware() {
  unsigned long now = millis();
  if (now - lastCheck < 15000) {
    return;
  }
  lastCheck = now;
  String remoteMD5;
  String currentMD5 = ESP.getSketchMD5();
  HTTPClient http;
  http.begin(FIRMWARE_URL);
  const char *headerKeys[] = {"Content-MD5"};
  size_t headerkeyssize = 1;
  http.collectHeaders(headerKeys, 1);
  int httpCode = http.sendRequest("HEAD");
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("Firmware HTTP HEAD failed, error: %s\n",
                  http.errorToString(httpCode).c_str());
    goto done;
  }
  remoteMD5 = http.header("Content-MD5");
  if (remoteMD5 == "") {
    Serial.println("invalid empty remote md5");
    goto done;
  }
  if (remoteMD5 != currentMD5) {
    Serial.println("Diff firmware MD5, updating...");
    Serial.print("Sketch md5: ");
    Serial.println(currentMD5);
    Serial.print("Remote md5: ");
    Serial.println(remoteMD5);
    doUpdate(remoteMD5);
  }
done:
  http.end();
}
