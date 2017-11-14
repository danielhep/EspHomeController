#include <VaRGB.h>
#include <VaRGBCurves.h>
#include <Ticker.h>

void setColorCB(vargb::ColorSettings *set_to);
void scheduleCompleteCB(vargb::Schedule *sched);

Ticker ticker;

vargb::VaRGB RGB_prog(setColorCB, scheduleCompleteCB);

int lastR = 0;
int lastG = 0;
int lastB = 0;

bool ledsOn = true;

void scheduleCompleteCB(vargb::Schedule *sched)
{
  // RGB_prog.resetTicks();
  Serial.println("Schedule Complete");
  ticker.detach();
}

void setColorCB(vargb::ColorSettings *set_to)
{
  Serial.print("Writing color: ");
  Serial.print(set_to->red);
  Serial.print(" ");
  Serial.print(set_to->green);
  Serial.print(" ");
  Serial.print(set_to->blue);
  Serial.println();

  int red = set_to->red;
  int green = set_to->green;
  int blue = set_to->blue;

  lastR = red;
  lastG = green;
  lastB = blue;

  if (red == green && blue == green)
  {
    analogWrite(LIGHT_CH4_PIN, red);
    analogWrite(LIGHT_CH1_PIN, 0);
    analogWrite(LIGHT_CH2_PIN, 0);
    analogWrite(LIGHT_CH3_PIN, 0);
  }
  else
  {
    analogWrite(LIGHT_CH1_PIN, red);
    analogWrite(LIGHT_CH2_PIN, green);
    analogWrite(LIGHT_CH3_PIN, blue);
    analogWrite(LIGHT_CH4_PIN, 0);
  }
}

void fadeToColor(byte Rb, byte Gb, byte Bb)
{
  int R = map(Rb, 0, 255, 0, 1023);
  int G = map(Gb, 0, 255, 0, 1023);
  int B = map(Bb, 0, 255, 0, 1023);
  Serial.print("Updating color to ");
  Serial.print(R);
  Serial.print(G);
  Serial.print(B);
  Serial.println();
  vargb::Curve::Constant *initial = new vargb::Curve::Constant(lastR, lastG, lastB, 0);
  vargb::Curve::Linear *curve = new vargb::Curve::Linear(R, G, B, 3);
  vargb::Curve::Constant *constant = new vargb::Curve::Constant(R, G, B, 1);
  vargb::Schedule *newSchedule = new vargb::Schedule();

  newSchedule->addTransition(initial);
  newSchedule->addTransition(curve);
  newSchedule->addTransition(constant);
  RGB_prog.resetTicks();
  RGB_prog.setSchedule(newSchedule);
  ticker.attach_ms(RGB_prog.tickDelayTimeMs(), tick);
}

void fadeToColor()
{
  fadeToColor(lastR, lastG, lastB);
}

void setLedsState(bool state)
{
  if (ledsOn != state)
  {
    ledsOn = state;
    if (!state)
    {
      fadeToColor(0, 0, 0);
    }
    else
    {
      fadeToColor(255, 255, 255);
    }
  }
}

void tick()
{
  RGB_prog.tick();
}

void ledSetup()
{
  fadeToColor(0, 0, 0);
}

void ledLoop()
{
  RGB_prog.tick();
}

unsigned int getLoopDelay()
{
  return RGB_prog.tickDelayTimeMs();
}