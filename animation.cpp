#include <Arduino.h>

#include "common.hpp"

void setColor(byte n, byte c);

namespace sinAnim {
double pi2 = 2 * PI, pi2by3 = pi2 / 3;
double rFreq = 1, gFreq = 1, bFreq = 1, inc = 0.01;
double ri = 0, bi = 0, gi = 0;
double rOffset = 0, gOffset = pi2by3, bOffset = 2 * pi2by3;
byte width = 127, center = 128;

void animate(String extra) {
  byte totalDelay = 20;
  if (extra.length() > 0) {
    totalDelay = extra.toInt();
  }
  delay(totalDelay);
  byte r = sin(rFreq * ri + rOffset) * width + center;
  byte g = sin(gFreq * gi + gOffset) * width + center;
  byte b = sin(bFreq * bi + bOffset) * width + center;
  ri += inc;
  gi += inc;
  bi += inc;
  double rDiff = (rFreq * ri + rOffset) - pi2;
  if (rDiff >= 0) {
    ri = -(rDiff + rOffset) / rFreq;
  }
  double gDiff = (gFreq * gi + gOffset) - pi2;
  if (gDiff >= 0) {
    gi = -(gDiff + gOffset) / gFreq;
  }
  double bDiff = (bFreq * bi + bOffset) - pi2;
  if (bDiff >= 0) {
    bi = -(bDiff + bOffset) / bFreq;
  }
  setColor(0, r);
  setColor(1, g);
  setColor(2, b);
}
}
