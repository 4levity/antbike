#include "lpd8806.h"
#include "Gear.h"

class LedRefreshGear : public Gear {
 private:
  static LedRefreshGear* ledGear;
  lpd8806* ledStrip;

  LedRefreshGear(int leds,int channel=0);  
 public:
  ~LedRefreshGear();

  static LedRefreshGear* getSingle(); 
  static LedRefreshGear* create(int leds, int channel=0);
  static void destroy();
  lpd8806* strip();

  void process();  // called every tick
  char* getName();
  void onStop();
};

#define LEDSTRIP (LedRefreshGear::getSingle()->strip())
