#include <iostream>
#include <stdio.h>
#include "LedFillGear.h"

LedFillGear::LedFillGear(int start,int len,
			 unsigned char r,unsigned char g,unsigned char b,int order) 
  : Gear(10,order) {
  this->start=start;
  this->len=len;
  this->r=r;
  this->g=g;
  this->b=b;
}

void LedFillGear::process() {
  LEDSTRIP->setRange(start,len,r,g,b);
}

char* LedFillGear::getName() {
  static char name[80];
  sprintf(name,"Fill %d->%d [%02X%02X%02X]",start,start+len-1,r,g,b);
  return name;
}
