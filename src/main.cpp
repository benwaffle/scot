#include <Arduino.h>
#include <SoftwareSerial.h>
#include <stdarg.h>

#define TESTING_MODE 0

const uint8_t potIn = A0;
const uint8_t motorTx = 2;
const uint8_t motorRx = 3; // unused
const uint8_t fullStop = 64;
const uint8_t maxForwardSpeed = 127;
const uint8_t maxReverseSpeed = 50;

enum
{
  STOPPED,
  BRAKING,
  FORWARD,
} state = STOPPED;

void p(const char *fmt, ...)
{
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, 128, fmt, args);
  va_end(args);
  Serial.print(buf);
}

const char *stateToString()
{
  switch (state)
  {
  case STOPPED:
    return "stopped";
  case BRAKING:
    return "braking";
  case FORWARD:
    return "forward";
  }
}

SoftwareSerial motor(motorRx, motorTx);

void setup()
{
  motor.begin(9600);
  Serial.begin(9600);
  pinMode(potIn, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
#if TESTING_MODE
  String num = Serial.readStringUntil('\n');
  if (num.length() > 0) {
    p("read %s\n", num.c_str());
    uint8_t val = num.toInt();
    if (val < 40 || val > 80) {
      val = fullStop;
    }
    p("write %d\n", val);
    motor.write(val);
  }
#else
  int pot = analogRead(potIn);
  if (pot < 100)
  {
    if (state == FORWARD)
    {
      p("braking\n");
      state = BRAKING;
      motor.write(maxReverseSpeed);
      delay(500);
      motor.write(fullStop);
      p("done braking. stopped\n");
      state = STOPPED;
    }
    else if (state == BRAKING)
    {
      p("should never get here\n");
    }
    else if (state == STOPPED)
    {
      p("stopped\n");
    }
  }
  else
  {
    state = FORWARD;
    int speed = map(pot, 100, 1023, fullStop, maxForwardSpeed);
    p("forward speed = %d\n", speed);
    motor.write(speed);
  }
#endif
}
