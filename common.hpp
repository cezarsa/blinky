#include <Arduino.h>
#include <math.h>

#include "config.h"

typedef void (*animFunc)(String);

namespace sinAnim {
void animate(String);
}
