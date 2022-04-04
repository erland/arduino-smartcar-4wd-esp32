#include <Arduino.h>
#include "perflog.h"

PerfLog::PerfLog() {
  start = 0;
}

void PerfLog::begin() {
  start = micros();
}

void PerfLog::end(const char *message) {
  Serial.print(message);
  Serial.print(": ");
  Serial.println(micros()-start);
}
