#include <iostream>
#include <math.h>
#include <stdio.h>
#include "LedSweepGear.h"
#include "LedRefreshGear.h"
#include "lpd8806.h"

LedSweepGear::LedSweepGear(int ledStart,int len,int border,int rpm,int order)
  : Gear(rpm,order) {
  this->ledStart=ledStart;
  this->len=len;
  int canvasStart=ledStart-border;
  int canvasLen=len+(2*border);
  int canvasEnd=canvasStart+canvasLen;
  this->mapLen=(canvasLen*2)-2;
  map=new int[this->mapLen];
  for(int i=0;i<canvasLen;i++) {
    if(i>=border && i<canvasLen-border) {
      map[i]=i+canvasStart;
    } else {
      map[i]=-1;
    }
  }
  for(int i=1;i<canvasLen-1;i++) {
    if(i>=border && i<canvasLen-border) {
      map[canvasLen+i-1]=canvasEnd-i-1;
    } else {
      map[canvasLen+i-1]=-1;
    }
  }
  this->r=0xff;
  this->g=0xff;
  this->b=0xff;
  this->trail=5;
}

LedSweepGear::~LedSweepGear() {
  delete map;
}

LedSweepGear* LedSweepGear::configure(unsigned char r,unsigned char g,unsigned char b,
			     unsigned char trail) {
  this->r=r;
  this->g=g;
  this->b=b;
  this->trail=trail;
  return this;
}

void LedSweepGear::setOnMap(int led,unsigned char bright) {
  if(led<0) 
    led+=mapLen;
  
  if(map[led]>=0) {
    LEDSTRIP->set(map[led],
		  ((int)bright*(int)r)/255.0,
		  ((int)bright*(int)g)/255.0,
		  ((int)bright*(int)b)/255.0);
      //		  (((int)bright*(int)b)/255.0)*(223.0/255.0)+32);
		  //		  ((int)bright*(int)b)/255.0);
  }
}

void LedSweepGear::process() {
  LEDSTRIP->setRange(ledStart,len,0,0,0);
  int sector=this->getSector(this->mapLen);
  for(int i=0;i<=trail;i++) {
    this->setOnMap(sector-i,255-((255.0/trail)*i));
  }

  //std::cout<<"sector "<<sector<<", position "<<map[sector]<<std::endl;
  
}

char* LedSweepGear::getName() {
  static char name[80];
  sprintf(name,"Sweep %d-%d [%02X%02X%02X] ...%d...",
	  ledStart,ledStart+len-1,r,g,b,trail);
  return name;
}
