#include "LedRefreshGear.h"

class LedFillGear : public Gear {
  int start,len;
  unsigned char r,g,b;

 public:
  LedFillGear(int start,int len,
		unsigned char r,unsigned char g,unsigned char b,int order);
  void process();
  char* getName();
};
