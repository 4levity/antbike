#include "Gear.h"

class LedSweepGear : public Gear {
  int start,len;
  int mapLen;
  int* map;

  void setOnMap(int led,unsigned char bright);
 public:
  LedSweepGear(int start,int len,
	       int rpm,int order);
  ~LedSweepGear();
  void process();
  char* getName();
};
