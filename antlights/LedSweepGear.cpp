#include <iostream>
#include <math.h>
#include <stdio.h>
#include "LedSweepGear.h"
#include "LedRefreshGear.h"
#include "lpd8806.h"

LedSweepGear::LedSweepGear(int start,int len,int rpm,int order)
  : Gear(rpm,order) {
  this->start=start;
  this->len=len;
  
  int borderTry=3;
  int border=start>borderTry && start+len+borderTry<LEDSTRIP->getLength() ? borderTry : 0;
  int canvasStart=start-border;
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

  std::cout<<"map:";
  for(int i=0;i<mapLen;i++) {
    std::cout<<" ["<<i<<"]="<<map[i];
  }
			      std::cout<<std::endl;
}

LedSweepGear::~LedSweepGear() {
  delete map;
}

void LedSweepGear::setOnMap(int led,unsigned char bright) {
  if(led<0) 
    led+=mapLen;
  
  if(map[led]>=0) {
    LEDSTRIP->set(map[led],bright,bright,bright);
  }
}

void LedSweepGear::process() {
  LEDSTRIP->setRange(start,len,0,0,0);
  int sector=this->getSector(this->mapLen);
  this->setOnMap(sector,255);
  this->setOnMap(sector-1,210);
  this->setOnMap(sector-2,165);
  this->setOnMap(sector-3,125);
  this->setOnMap(sector-4,90);
  this->setOnMap(sector-5,50);

  //std::cout<<"sector "<<sector<<", position "<<map[sector]<<std::endl;
  
}

char* LedSweepGear::getName() {
  static char name[80];
  sprintf(name,"Sweep %d->%d",start,start+len-1);
  return name;
}
