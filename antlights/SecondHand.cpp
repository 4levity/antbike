#include <iostream>
#include <stdexcept>
#include "SecondHand.h"
#include "Gear.h"

SecondHand::SecondHand(int hz) {
  this->ticksPerMinute=hz*60;
  this->firstGear=NULL;
  this->firstGearToDelete=NULL;
  this->lastTickActual.tv_sec=0;
  this->lastTickActual.tv_nsec=0;
  this->lastMinute=0;
  this->lastTick=0;
  this->lastMeasuredInterval_us=0;
  this->runTicker=false; // tickerThread not yet created

  if(pthread_mutex_init(&(this->tickerMutex),NULL)!=0) {
    throw new std::runtime_error("error initializing SecondHand mutex");
  }

  this->missedTicks=0;
}

SecondHand::~SecondHand() {
  deleteGears();
  pthread_mutex_destroy(&(this->tickerMutex));
}

void SecondHand::deleteGears() {
  Gear* g;
  while((g=this->firstGearToDelete) != NULL) {
    this->firstGearToDelete=g->getNext();
    delete g;
  }
}

void* SecondHand::runTickerThread(void* arg) {
  SecondHand* secondHand=(SecondHand*)arg;
  secondHand->ticker();
  return NULL;
}

long SecondHand::usDiff(const timespec& t1,const timespec& t2) {
  return (1000000 * (t2.tv_sec-t1.tv_sec)) + ((t2.tv_nsec-t1.tv_nsec)/1000);
}

void SecondHand::ticker() {
  timespec targetTime,tickActual,lastMinuteTop,doneTime;
  while(this->runTicker) {
    clock_gettime(CLOCK_MONOTONIC, &tickActual);
 
    pthread_mutex_lock(&(this->tickerMutex));
    ///////////////////////////////////// CRITICAL SECTION ...
    this->lastMeasuredInterval_us=lastTickActual.tv_sec=0?0:usDiff(doneTime,tickActual);
    this->lastTickActual= tickActual;
    lastMinuteTop.tv_sec=(tickActual.tv_sec / 60) * 60;
    lastMinuteTop.tv_nsec=0;
    long elapsed_us=usDiff(lastMinuteTop,tickActual);
    long thisTick=(long) ((elapsed_us/(60*1000000.0))*(1.0*(this->ticksPerMinute)));
    long thisMinute=tickActual.tv_sec/60;


    // TODO: calculated missed ticks
    this->lastMinute=thisMinute;
    this->lastTick=thisTick;
    this->deleteGears();
    Gear* g=this->firstGear;
    ///////////////////////////////////// CRITICAL SECTION
    pthread_mutex_unlock(&(this->tickerMutex));
    if(g==NULL) {
      //std::cout<<"No gears.";
    }
    // process gears
    while(g != NULL) {
      //std::cout<<"Gear ["<<g->getName()<<"] ";
      g->tick(this);

      pthread_mutex_lock(&(this->tickerMutex));
      ///////////////////////////////////// CRITICAL SECTION
      g=g->getNext();
      pthread_mutex_unlock(&(this->tickerMutex));
    }
    //std::cout<<std::endl;

    // calculate sleep time
    long nextTick=this->lastTick + 1;
    if(nextTick >= this->ticksPerMinute) {
      targetTime.tv_sec = lastMinuteTop.tv_sec + 60;
      targetTime.tv_nsec= 0;
    } else {
      long target_us=(long)( (nextTick / (1.0 * this->ticksPerMinute)) * 60.0 * 1000000.0);
      targetTime.tv_sec = lastMinuteTop.tv_sec + (target_us / 1000000);
      targetTime.tv_nsec= (target_us % 1000000) * 1000;
    }
    clock_gettime(CLOCK_MONOTONIC, &doneTime);
    long estSleepFor=usDiff(doneTime,targetTime);
    /*std::cout<<"slept for "<<this->lastMeasuredInterval_us<<"us, now tick#"<< thisTick <<" of "<< this->ticksPerMinute
	     <<" (misses="<<this->missedTicks<<")"
	     << " sleep for " << estSleepFor << "us"
	     << std::endl;*/
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &targetTime, NULL);
  }
}

Gear* SecondHand::insertGear(Gear* newGear) {
  pthread_mutex_lock(&(this->tickerMutex));
  ///////////////////////////////////// CRITICAL SECTION ...
  if(this->firstGear!=NULL) {
     if(newGear->getRunOrder() <= this->firstGear->getRunOrder()) {
      newGear->setNext(this->firstGear);
      this->firstGear=newGear;
    } else {
       Gear* g=this->firstGear;
       do {
	 if(g->getNext()==NULL) {
	   newGear->setNext(NULL);
	   g->setNext(newGear);
	   g=NULL;
	 } else if(newGear->getRunOrder() <= g->getNext()->getRunOrder()) {
	   newGear->setNext(g->getNext());
	   g->setNext(newGear);
	   g=NULL;
	 } else {
	   g=g->getNext();
	 }
       } while(g!=NULL);
     }
  } else {
    this->firstGear=newGear;
    newGear->setNext(NULL);
  }
  ///////////////////////////////////// CRITICAL SECTION
  pthread_mutex_unlock(&(this->tickerMutex));    
  return newGear;
}

void SecondHand::removeGear(Gear* targetGear,bool deleteGearObj) {
  bool foundMatch=false;
  pthread_mutex_lock(&(this->tickerMutex));
  ///////////////////////////////////// CRITICAL SECTION ...
  if(this->firstGear == NULL) {
    return;
  } else if(targetGear == this->firstGear) {
    this->firstGear=targetGear->getNext();
    foundMatch=true;
  } else {
    Gear* g=this->firstGear;
    do {
      if(g->getNext() == targetGear) {
	g->setNext(targetGear->getNext());
	foundMatch=true;
      }
    } while((g=g->getNext()) != NULL);
  }
  if(foundMatch) { // mark for deletion?
    if(deleteGearObj) {
      targetGear->setNext(this->firstGearToDelete);
      this->firstGearToDelete=targetGear;
    } else {
      targetGear->setNext(NULL);
    }
  }
  ///////////////////////////////////// CRITICAL SECTION
  pthread_mutex_unlock(&(this->tickerMutex));    
}

SecondHand* SecondHand::start() { 
  //std::cout << "SecondHand starting" << std::endl;
  pthread_mutex_lock(&(this->tickerMutex));    
  ///////////////////////////////////// CRITICAL SECTION
  if(this->runTicker) {
    ////////////// EARLY RETURN
    pthread_mutex_unlock(&(this->tickerMutex));    
    return this;
  }
  this->runTicker=true;
  ///////////////////////////////////// CRITICAL SECTION
  pthread_mutex_unlock(&(this->tickerMutex));    

  int err= pthread_create(&this->tickerThread,NULL,&runTickerThread,(void*)this);
  if(err!=0) {
    throw new std::runtime_error("failed to create ticker thread");
  }
  return this;
}

SecondHand* SecondHand::stop() {
  pthread_mutex_lock(&(this->tickerMutex));    
  ///////////////////////////////////// CRITICAL SECTION
  if(!(this->runTicker)) {
    ////////////// EARLY RETURN
    pthread_mutex_unlock(&(this->tickerMutex));    
    return this;
  }
  runTicker=false;
  ///////////////////////////////////// CRITICAL SECTION
  pthread_mutex_unlock(&(this->tickerMutex));    
  pthread_join(this->tickerThread,NULL);
  
  return this;
}

long SecondHand::getTicksPerMinute() {
  return this->ticksPerMinute;
}

long SecondHand::getTick() { 
  long tick;
  pthread_mutex_lock(&(this->tickerMutex));    
  ///////////////////////////////////// CRITICAL SECTION
  tick=this->lastTick;
  pthread_mutex_unlock(&(this->tickerMutex));    
  return tick;
}

char SecondHand::getSecond() {
  char second;
  pthread_mutex_lock(&(this->tickerMutex));    
  ///////////////////////////////////// CRITICAL SECTION
  second = (this->lastTickActual.tv_sec % 60);
  pthread_mutex_unlock(&(this->tickerMutex));    
  return second;
}

int SecondHand::getMissedTicks() {
  int missed;
  pthread_mutex_lock(&(this->tickerMutex));    
  ///////////////////////////////////// CRITICAL SECTION
  missed=this->missedTicks;
  pthread_mutex_unlock(&(this->tickerMutex));    
  return missed;
}

