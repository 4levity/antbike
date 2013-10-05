#include "Gear.h"

class LedSweepGear : public Gear {
  int ledStart,len;
  int mapLen;
  int* map;
  unsigned char r,g,b,trail;

  void setOnMap(int led,unsigned char bright);
 public:
  LedSweepGear(int start,int len,int border,
	       int rpm,int order);
  ~LedSweepGear();
  LedSweepGear* configure(unsigned char r,
			  unsigned char g,
			  unsigned char b,unsigned char trail);
  void process();
  char* getName();
};
