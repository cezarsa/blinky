#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "common.hpp"

ESP8266WebServer server(80);

const byte nColors = 3;
char colorLabels[nColors] = {'r', 'g', 'b'};
byte lastColor[nColors] = {0, 0, 0};

const byte nAnimations = 1;
animFunc animate[nAnimations] = {sinAnim::animate};
int currentAnimation = -1;
String animExtra = "";

void setColor(byte n, byte c) {
  lastColor[n] = c;
  byte pin;
  if (n == 0) {
    pin = D1;
  } else if (n == 1) {
    pin = D2;
  } else if (n == 2) {
    pin = D3;
  } else {
    return;
  }
  int duty = (1023 * c) / 255;
  analogWrite(pin, duty);
}

void onRequest() {
  animExtra = server.arg("extra");
  String animStr = server.arg("anim");
  if (animStr != "") {
    currentAnimation = animStr.toInt();
  }
  String rStr = server.arg("r");
  if (rStr != "") {
    setColor(0, rStr.toInt());
  }
  String gStr = server.arg("g");
  if (gStr != "") {
    setColor(1, gStr.toInt());
  }
  String bStr = server.arg("b");
  if (bStr != "") {
    setColor(2, bStr.toInt());
  }
  String rsp("{");
  for (byte i = 0; i < nColors; i++) {
    rsp += "\"";
    rsp += colorLabels[i];
    rsp += "\":";
    rsp += lastColor[i];
    if (i < nColors - 1) {
      rsp += ",";
    }
  }
  rsp += String(",\"anim\":") + currentAnimation;
  rsp += String(",\"extra\":\"") + animExtra + String("\"");
  rsp += "}";
  server.send(200, "application/json", rsp);
}

void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  analogWrite(D1, 0);
  analogWrite(D2, 0);
  analogWrite(D3, 0);
  delay(1000);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_NAME, SSID_PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }

  server.on("/", onRequest);
  server.begin();

  Serial.print(WiFi.localIP());
}

void loop() {
  server.handleClient();
  if (currentAnimation >= 0 && currentAnimation < nAnimations) {
    animate[currentAnimation](animExtra);
  }
}
