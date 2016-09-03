#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include "common.hpp"

ESP8266WebServer *server;

const byte nColors = 3;
char colorLabels[nColors] = {'r', 'g', 'b'};
byte lastColor[nColors] = {0, 0, 0};
byte lastColorGamma[nColors] = {0, 0, 0};
double gammas[nColors] = {1.5, 2.8, 1.8};
byte max_ins[nColors] = {255, 255, 255};
byte max_outs[nColors] = {255, 127, 255};

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
  byte max_in = max_ins[cIdx];
  byte max_out = max_outs[cIdx];
  return byte(pow(double(c) / double(max_in), gamma) * max_out + 0.5);
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
  String rStr = server->arg("r");
  if (rStr != "") {
    setColorGamma(color::red, rStr.toInt());
  }
  String gStr = server->arg("g");
  if (gStr != "") {
    setColorGamma(color::green, gStr.toInt());
  }
  String bStr = server->arg("b");
  if (bStr != "") {
    setColorGamma(color::blue, bStr.toInt());
  }
  String rsp("{");
  for (byte i = 0; i < nColors; i++) {
    rsp += "\"";
    rsp += colorLabels[i];
    rsp += "\":";
    rsp += lastColorGamma[i];
    rsp += ",\"";
    rsp += colorLabels[i];
    rsp += "_raw\":";
    rsp += lastColor[i];
    if (i < nColors - 1) {
      rsp += ",";
    }
  }
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
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
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
