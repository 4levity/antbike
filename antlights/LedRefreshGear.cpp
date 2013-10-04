#include <iostream>
#include <stdexcept>
#include "LedRefreshGear.h"

LedRefreshGear* LedRefreshGear::ledGear = NULL;

LedRefreshGear::LedRefreshGear(int leds,int channel) : Gear(1,127) {
  this->ledStrip=new lpd8806(leds,channel);
  this->ledStrip->fill(0,0,0);
  this->ledStrip->update();
}

LedRefreshGear::~LedRefreshGear() {
  delete this->ledStrip;
}

LedRefreshGear* LedRefreshGear::create(int leds,int channel) {
  if(LedRefreshGear::ledGear==NULL) {
    LedRefreshGear::ledGear=new LedRefreshGear(leds,channel);
  } else {
    throw new std::runtime_error("LedRefreshGear::create cannot be called twice");
  }
  return LedRefreshGear::ledGear;
}

LedRefreshGear* LedRefreshGear::getSingle() {
  return LedRefreshGear::ledGear;
}

lpd8806* LedRefreshGear::strip() {
  return ledStrip;
}

void LedRefreshGear::process() {
  this->strip()->update();
}

char* LedRefreshGear::getName() {
  static char name[]="LED-refresh";
  return name;
}
