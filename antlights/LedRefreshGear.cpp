#include <iostream>
#include <stdexcept>
#include "LedRefreshGear.h"

LedRefreshGear* LedRefreshGear::ledGear = NULL;

LedRefreshGear::LedRefreshGear(int leds,int channel) : Gear(1,999) {
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

void LedRefreshGear::destroy() {
  ledGear->stop();
  if(LedRefreshGear::ledGear!=NULL) {
    delete LedRefreshGear::ledGear;
    LedRefreshGear::ledGear=NULL;
  }
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

void LedRefreshGear::onStop () {
  std::cout<<"LED refresh stop, turning off strip"<<std::endl;
  this->ledStrip->fill(0,0,0);
  this->ledStrip->update();
}
