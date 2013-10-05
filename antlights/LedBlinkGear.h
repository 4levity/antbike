#include "Gear.h"
class LedBlinkGear : public Gear {
  int ledStart,len;
  unsigned char r,g,b;
  const unsigned char* rgbOverlay;
  double onAngle;

 public:
  LedBlinkGear(int start,int len,unsigned char r,unsigned char g,unsigned char b,
	       int order,int blinksPerMinute,char dutyCyclePercent);
  LedBlinkGear(const unsigned char* rgbOverlay,
	       int order,int blinksPerMinute,char dutyCyclePercent);
  
  void process();
  char* getName();
};
 
