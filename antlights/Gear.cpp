#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <math.h>

#include "Gear.h"

Gear::Gear(int rpm,int runOrder,bool start) {
  this->rpm=rpm;
  this->runOrder=runOrder;
  this->nextGear=NULL;
  this->lastAngle=0;
  this->centerOffsetTick=-1; // needs to be calculated at first tick
  this->active=start;
  if(pthread_mutex_init(&(this->gearMutex),NULL)!=0) {
    throw std::runtime_error("error initializing Gear mutex");
  }
}

Gear::~Gear() {
  pthread_mutex_destroy(&(this->gearMutex));
}

Gear* Gear::getNext() {
  return this->nextGear;
}

void Gear::setNext(Gear* newNext) {
  this->nextGear=newNext;
}

void Gear::tick(SecondHandTick* source) {
  if(this->active) {
    pthread_mutex_lock(&(this->gearMutex));    
    ////////////////////////////////// CRITICAL SECTION
    long t=source->getTick();
    long tpm=source->getTicksPerMinute();
    
    if(this->centerOffsetTick < 0) {
      // recalculate offset to retain current angle
      //((numticks/maxticks)*2pi)/rpm=a
      //a*rpm=2pi*(numticks/maxticks)
      //(a*rpm)/2pi=numticks/maxticks
      //maxticks*(a*rpm)/2pi=numticks !
      this->centerOffsetTick =
	tpm * (this->lastAngle * this->rpm)
	/ (2.0 * M_PI);
    }
    double sourceComplete = (1.0 * t) 
      / (1.0 * tpm);
    int sourceSector = (int)(sourceComplete * (1.0 * this->rpm));
    this->lastAngle = ((sourceComplete * (1.0 * this->rpm)) 
		       - sourceSector) * 2.0 * M_PI;    
    ////////////////////////////////// CRITICAL SECTION
    pthread_mutex_unlock(&(this->gearMutex));    

    //    std::cout<<"processing gear "<<this->getName()<< " order " <<runOrder<<std::endl;
    // perform per-tick processing!
    this->process();
  } else { 
    //std::cout<<"skipping inactive gear "<<this->getName()<<std::endl;
  }
}

Gear* Gear::start() {
  pthread_mutex_lock(&(this->gearMutex));    
  ////////////////////////////////// CRITICAL SECTION
  this->active=true;
  pthread_mutex_unlock(&(this->gearMutex));    
  return this;
}

Gear* Gear::stop(bool keepAngle) {
  pthread_mutex_lock(&(this->gearMutex));
  ////////////////////////////////// CRITICAL SECTION
  if(this->active) {
    if(keepAngle) {
      this->centerOffsetTick= -1;
      // retain angle, recalculate drive offset on restart
    }
    this->active=false;
  } 
  ////////////////////////////////// CRITICAL SECTION
  pthread_mutex_unlock(&(this->gearMutex));    
  return this;
}

Gear* Gear::recenter() {
  pthread_mutex_lock(&(this->gearMutex));
  ////////////////////////////////// CRITICAL SECTION
  this->lastAngle=0.0;
  this->centerOffsetTick=-1.0; // recalculate
  pthread_mutex_unlock(&(this->gearMutex));    
  return this;
}

Gear* Gear::toggle(bool recenterOnStart) {
  pthread_mutex_lock(&(this->gearMutex));
  ////////////////////////////////// CRITICAL SECTION
  this->active = !(this->active);
  if(this->active && recenterOnStart) {
    this->lastAngle=0.0;
    this->centerOffsetTick=-1.0; // recalculate
  }
  pthread_mutex_unlock(&(this->gearMutex));    
  return this;
}

bool Gear::isActive() {
  pthread_mutex_lock(&(this->gearMutex));
  ////////////////////////////////// CRITICAL SECTION
  bool a=this->active;
  pthread_mutex_unlock(&(this->gearMutex));    
  return a; 
}

int Gear::getRunOrder() {
  return this->runOrder;
}

double Gear::getAngle() {
  double angle;
  pthread_mutex_lock(&(this->gearMutex));
  ////////////////////////////////// CRITICAL SECTION
  angle=this->lastAngle;
  pthread_mutex_unlock(&(this->gearMutex));    
  return angle;
}

int Gear::getSector(int sectors) {
  return (int)((getAngle() / (2.0 * M_PI)) * sectors);
}
