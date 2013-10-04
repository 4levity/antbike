#include "Gear.h"
class LedBlinkGear : public Gear {
  int start,len;
  unsigned char r,g,b;
  unsigned char* rgbOverlay;
  double onAngle;

 public:
  LedBlinkGear(int start,int len,unsigned char r,unsigned char g,unsigned char b,
	       int order,int blinksPerMinute,char dutyCyclePercent);
  LedBlinkGear(unsigned char* rgbOverlay,
	       int order,int blinksPerMinute,char dutyCyclePercent);
  
  void process();
  char* getName();
};
 
