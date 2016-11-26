#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <math.h>

typedef void (*animFunc)(String);

enum color { red, green, blue };

namespace sinAnim {
void animateGamma(String);
void animateRaw(String);
}

void toggleLights(bool *force);
void setColor(color cIdx, byte c);
void setColorGamma(color cIdx, byte c);
bool runManager();
void forceManager();
void checkFirmware();

void initIR();
void loopIR();
String irJSON();
