#ifndef __SECONDHANDTICK_H__
#define __SECONDHANDTICK_H__
class SecondHandTick {
 public:
  virtual long getTicksPerMinute() = 0;
  virtual long getTick() = 0;
  virtual char getSecond() = 0;
};
#endif
