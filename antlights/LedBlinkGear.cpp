#include <math.h>
#include <stdio.h>
#include "LedBlinkGear.h"
#include "LedRefreshGear.h"

LedBlinkGear::LedBlinkGear(int start,int len,
			   unsigned char r,unsigned char g,unsigned char b,
			   int order,int blinksPerMinute,char dutyCyclePercent) :
  Gear(blinksPerMinute,order) {
  this->start=start;
  this->len=len;
  this->r=r;
  this->g=g;
  this->b=b;
  this->onAngle=(dutyCyclePercent/100.0)*2.0*M_PI;
  this->rgbOverlay=NULL;
}

LedBlinkGear::LedBlinkGear(unsigned char* rgbOverlay,
			   int order,int blinksPerMinute,char dutyCyclePercent) :
  Gear(blinksPerMinute,order) {
  this->onAngle=(dutyCyclePercent/100.0)*2.0*M_PI;
  this->rgbOverlay=rgbOverlay;
}

void LedBlinkGear::process() {
  if(getAngle()<this->onAngle) {
    if(rgbOverlay!=NULL) {
      int len=LEDSTRIP->getLength();
      for(int i=0;i<len;i++) {
	unsigned char r,g,b;
	r=rgbOverlay[3*i+0];
	g=rgbOverlay[3*i+1];
	b=rgbOverlay[3*i+2];
	if(r!=0 || g!=0 || b!=0) {
	  LEDSTRIP->set(i,r,g,b);
	}
      }
    } else {
      LEDSTRIP->setRange(start,len,r,g,b);
    }
  }
}

char* LedBlinkGear::getName() {
  static char name[80];
  if(rgbOverlay!=NULL) {
    sprintf(name,"Blink overlay");
  } else {
    sprintf(name,"Blink %d->%d [%02X%02X%02X]",start,start+len-1,r,g,b);
  }
  return name;
}
