#include <math.h>
#include <stdio.h>
#include "LedBlinkGear.h"
#include "LedRefreshGear.h"

LedBlinkGear::LedBlinkGear(int start,int len,
			   unsigned char r,unsigned char g,unsigned char b,
			   int order,int blinksPerMinute,char dutyCyclePercent) :
  Gear(blinksPerMinute,order) {
  this->ledStart=start;
  this->len=len;
  this->r=r;
  this->g=g;
  this->b=b;
  this->onAngle=(dutyCyclePercent/100.0)*2.0*M_PI;
  this->rgbOverlay=NULL;
}

LedBlinkGear::LedBlinkGear(const unsigned char* rgbOverlay,
			   int order,int blinksPerMinute,char dutyCyclePercent) :
  Gear(blinksPerMinute,order) {
  this->onAngle=(dutyCyclePercent/100.0)*2.0*M_PI;
  this->rgbOverlay=rgbOverlay;
}

void LedBlinkGear::process() {
  if(getAngle()<this->onAngle) {
    if(rgbOverlay!=NULL) {
      LEDSTRIP->setAllRGB(rgbOverlay,true);
    } else {
      LEDSTRIP->setRange(ledStart,len,r,g,b);
    }
  }
}

char* LedBlinkGear::getName() {
  static char name[80];
  if(rgbOverlay!=NULL) {
    sprintf(name,"Blink overlay");
  } else {
    sprintf(name,"Blink %d->%d [%02X%02X%02X]",ledStart,ledStart+len-1,r,g,b);
  }
  return name;
}
