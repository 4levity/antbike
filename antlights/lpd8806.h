#ifndef __LPD8806_H__
#define __LPD8806_H__
/*
  adapted from:

  LPD8806.py: Raspberry Pi library for the Adafruit LPD8806 RGB Strand

  Provides the ability to drive a LPD8806 based strand of RGB leds from the
  Raspberry Pi

  Colors are provided as RGB and converted internally to the strand's 7 bit
  values.

  The leds are available here: http://adafruit.com/products/306

  Wiring:
  Pi MOSI -> Strand DI
  Pi SCLK -> Strand CI

  Make sure to use an external power supply to power the strand

  Example:
  >> import LPD8806
  >> led = LPD8806.strand()
  >> led.fill(255, 0, 0)
*/

class lpd8806 {
 private:
  int leds;
  int device;
  unsigned char gamma[256];
  unsigned char *buffer;
  unsigned char command;

  double rotate(double angle,double increment);

 public:
  lpd8806(int leds=32,int channel=0);
  ~lpd8806();

  void fill(unsigned char r,unsigned char g,unsigned char b);
  void set(int led,unsigned char r,unsigned char g,unsigned char b);
  void update();
  void setRange(int start,int len,unsigned char r,unsigned char g,unsigned char b);
  void setAllRGB(unsigned char* rgbBuf);

  int getGamma(unsigned char v);
  int getLength();

  void demo(int iterations);
};
#endif
