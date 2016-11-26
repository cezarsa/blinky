#include "common.hpp"

const int RECV_PIN = D4;
IRrecv irrecv(RECV_PIN);

void initIR() { irrecv.enableIRIn(); }

String irEncoding(int type) {
  switch (type) {
  default:
  case UNKNOWN:
    return "UNKNOWN";
  case NEC:
    return "NEC";
  case SONY:
    return "SONY";
  case RC5:
    return "RC5";
  case RC6:
    return "RC6";
  case DISH:
    return "DISH";
  case SHARP:
    return "SHARP";
  case JVC:
    return "JVC";
  case SANYO:
    return "SANYO";
  case MITSUBISHI:
    return "MITSUBISHI";
  case SAMSUNG:
    return "SAMSUNG";
  case LG:
    return "LG";
  case WHYNTER:
    return "WHYNTER";
  case PANASONIC:
    return "PANASONIC";
  }
  return "";
}

#define N_IR_VALUES 10

int irTypes[N_IR_VALUES];
unsigned long irValues[N_IR_VALUES];
int irCurrent = 0;

String irJSON() {
  String rsp("\"ir\":{\"current\":");
  rsp += irCurrent;
  rsp += ",\"entries\":[";
  for (int i = 0; i < N_IR_VALUES; i++) {
    rsp += String("{\"encoding\":\"") + irEncoding(irTypes[i]) + String("\",");
    rsp += String("\"value\":\"") + String(irValues[i], HEX) + String("\"}");
    if (i < (N_IR_VALUES - 1)) {
      rsp += String(",");
    }
  }
  rsp += "]}";
  return rsp;
}

typedef void (*irCmd)();

struct cmd {
  unsigned long value;
  irCmd fn;
};

void on() {
  bool on = true;
  toggleLights(&on);
}

void off() {
  bool off = false;
  toggleLights(&off);
}

void toggle() { toggleLights(NULL); }

cmd cmdList[] = {{value : 0xce1972fd, fn : on},
                 {value : 0xd4dd0381, fn : off},
                 {value : 0x20df10ef, fn : toggle}};

void loopIR() {
  decode_results results;
  if (irrecv.decode(&results)) {
    if (results.value != 0xffffffff) {
      irTypes[irCurrent] = results.decode_type;
      irValues[irCurrent] = results.value;
      irCurrent = (irCurrent + 1) % N_IR_VALUES;
      int cmdsLen = sizeof(cmdList) / sizeof(cmd);
      for (int i = 0; i < cmdsLen; i++) {
        if (cmdList[i].value == results.value) {
          cmdList[i].fn();
          break;
        }
      }
    }
    irrecv.resume();
  }
}
