#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include "common.hpp"

ESP8266WebServer *server;

const byte nColors = 3;
char colorLabels[nColors] = {'r', 'g', 'b'};
byte lastColor[nColors] = {0, 0, 0};
byte lastColorGamma[nColors] = {0, 0, 0};
double gammas[nColors] = {1.8, 1.8, 1.8};
byte max_outs[nColors] = {255, 127, 255};

template <class T> String addAll(T array, String suffix) {
  String ret;
  for (int i = 0; i < nColors; ++i) {
    ret += "\"";
    ret += colorLabels[i];
    if (suffix != "") {
      ret += "_";
      ret += suffix;
    }
    ret += "\":";
    ret += array[i];
    if (i < nColors - 1) {
      ret += ",";
    }
  }
  return ret;
}

const byte nAnimations = 1;
animFunc animate[nAnimations] = {sinAnim::animate};
int currentAnimation = -1;
String animExtra = "";

void setColor(color cIdx, byte c) {
  lastColor[cIdx] = c;
  byte pin;
  if (cIdx == color::red) {
    pin = D1;
  } else if (cIdx == color::green) {
    pin = D2;
  } else if (cIdx == color::blue) {
    pin = D3;
  } else {
    return;
  }
  int duty = (1023 * c) / 255;
  analogWrite(pin, duty);
}

byte fixGamma(color cIdx, byte c) {
  double gamma = gammas[cIdx];
  byte max_out = max_outs[cIdx];
  return byte(pow(double(c) / 255.0, gamma) * max_out + 0.5);
}

void setColorGamma(color cIdx, byte c) {
  lastColorGamma[cIdx] = c;
  setColor(cIdx, fixGamma(cIdx, c));
}

void onRequest() {
  String extraStr = server->arg("extra");
  if (extraStr != "") {
    animExtra = extraStr;
  }
  String animStr = server->arg("anim");
  if (animStr != "") {
    currentAnimation = animStr.toInt();
  }
  for (int i = 0; i < nColors; ++i) {
    color cIdx = (color)i;
    String label(colorLabels[i]);
    String gammaStr = server->arg(label + "_gamma");
    if (gammaStr != "") {
      gammas[cIdx] = gammaStr.toFloat();
    }
    String maxStr = server->arg(label + "_max");
    if (maxStr != "") {
      max_outs[cIdx] = maxStr.toInt();
    }
    String colorStr = server->arg(label);
    if (colorStr != "") {
      setColorGamma(cIdx, colorStr.toInt());
    }
  }
  String rsp("{");
  rsp += addAll(lastColorGamma, "");
  rsp += ",";
  rsp += addAll(lastColor, "raw");
  rsp += ",";
  rsp += addAll(gammas, "gamma");
  rsp += ",";
  rsp += addAll(max_outs, "max");
  rsp += String(",\"anim\":") + currentAnimation;
  rsp += String(",\"extra\":\"") + animExtra + String("\"");
  rsp += "}";
  server->sendHeader("Access-Control-Allow-Origin", "*");
  server->send(200, "application/json", rsp);
}

bool reconfig = false;

void onConfig() {
  reconfig = true;
  server->send(200, "text/plain", "config reset");
}

void setup() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  digitalWrite(D0, HIGH);
  analogWrite(D1, 0);
  analogWrite(D2, 0);
  analogWrite(D3, 0);
  delay(1000);
  Serial.begin(9600);

  if (!runManager()) {
    return;
  }

  server = new ESP8266WebServer();
  server->on("/config", onConfig);
  server->on("/", onRequest);
  server->begin();
}

void loop() {
  checkFirmware();
  if (server) {
    server->handleClient();
  }
  if (reconfig) {
    reconfig = false;
    server->handleClient();
    delay(2000);
    WiFi.begin("-", "-");
    server->stop();
    delete server;
    server = NULL;
    forceManager();
  }
  if (currentAnimation >= 0 && currentAnimation < nAnimations) {
    animate[currentAnimation](animExtra);
  }
}
