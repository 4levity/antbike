#include <time.h>
#include <pthread.h>

#include "SecondHandTick.h"

class Gear;
class SecondHand : public SecondHandTick {
 private:
  int ticksPerMinute;

  Gear* firstGear;
  Gear* firstGearToDelete;

  timespec lastTickActual;
  long lastMinute;
  long lastTick;
  long lastMeasuredInterval_us;
  
  bool runTicker;
  pthread_t tickerThread;
  pthread_mutex_t tickerMutex;

  int missedTicks;

 public:
  SecondHand(int hz);
  ~SecondHand();

  static void* runTickerThread(void* arg);
  static long usDiff(const timespec& t1,const timespec& t2);

  void ticker();

  Gear* insertGear(Gear* newGear);
  void removeGear(Gear* gear,bool deleteObj = false);
  void deleteGears();

  SecondHand* start();
  SecondHand* stop();

  long getTicksPerMinute();

  long getTick();
  char getSecond();

  int getMissedTicks();
};
