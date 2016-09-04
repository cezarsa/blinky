#include <Arduino.h>
#include <math.h>

typedef void (*animFunc)(String);

enum color { red, green, blue };

namespace sinAnim {
void animate(String);
}

void setColor(color cIdx, byte c);
void setColorGamma(color cIdx, byte c);
bool runManager();
void forceManager();
void checkFirmware();
