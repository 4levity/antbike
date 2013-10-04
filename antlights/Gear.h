#ifndef __GEAR_H__
#define __GEAR_H__
#include <time.h>
#include <pthread.h>
#include "SecondHandTick.h"

class Gear {
 private:
  int rpm; // unchanging
  int runOrder; // unchanging

  Gear* nextGear;

  double lastAngle;
  long centerOffsetTick;
  bool active;

  pthread_mutex_t gearMutex;

 public:
  Gear(int rpm,int runOrder, bool start=true);
  ~Gear();

  Gear* getNext();
  void setNext(Gear* newNext);
  void tick(SecondHandTick* source);

  Gear* start();
  Gear* stop(bool keepAngle);
  Gear* recenter();
  bool isActive();

  char getRunOrder();
  double getAngle();
  int getSector(int sectors);

  virtual void process() = 0; // called at every tick while Gear is active
  virtual char* getName() = 0;
};

#endif
