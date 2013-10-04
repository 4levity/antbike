#include "lpd8806.h"
#include "Gear.h"

class LedRefreshGear : public Gear {
 private:
  static LedRefreshGear* ledGear;
  lpd8806* ledStrip;
  pthread_mutex_t stripBufferMutex;

  LedRefreshGear(int leds,int channel=0);  
 public:
  ~LedRefreshGear();

  static LedRefreshGear* getSingle(); 
  static LedRefreshGear* create(int leds, int channel=0);
  lpd8806* strip();

  void process();  // called every tick
  char* getName();
};

#define LEDSTRIP (LedRefreshGear::getSingle()->strip())
