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
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <wiringPiSPI.h>
#include <iostream>
#include <stdexcept>
#include "lpd8806.h"

lpd8806::lpd8806(int leds, int channel) {
  this->command=0;
  for(int i=0;i<256;i++) {
    // see : http://learn.adafruit.com/light-painting-with-raspberry-pi
    (this->gamma)[i] = 0x80 |
      (int)( pow( ((float)i)/255.0, 2.5) * 127.0 + 0.5 );
  }
  this->leds=leds;
  this->buffer=new unsigned char[leds*3];
  this->fill(0,0,0);
  this->device = wiringPiSPISetup(channel, 500000); //default SPI clock speed
}
lpd8806::~lpd8806() {
  close(this->device);
}
void lpd8806::fill(unsigned char r,unsigned char g,unsigned char b) {
  for(int i=0;i< (this->leds);i++) {
    this->set(i,r,g,b);
  }
}
void lpd8806::set(int led,unsigned char r,unsigned char g,unsigned char b) {
  if(led<0 || led>=leds) return;

  this->buffer[3*led+0]=this->gamma[g];
  this->buffer[3*led+1]=this->gamma[r];
  this->buffer[3*led+2]=this->gamma[b];
}
void lpd8806::setRange(int start,int len,
		       unsigned char r,unsigned char g,unsigned char b) {
  if(start<0 || (start+len)>getLength())
    return;

  for(int led=start;led<(start+len);led++) {
    this->set(led,r,g,b);
  }
}
void lpd8806::setAllRGB(const unsigned char* rgbBuf,
			bool transparent0) { // must be getLength*3 bytes!
  for(int i=0;i< this->getLength();i++) {
    unsigned char r,g,b;
    r=rgbBuf[3*i+0];
    g=rgbBuf[3*i+1];
    b=rgbBuf[3*i+2];
    
    if(r!=0 || g!=0 || b!=0 || (!transparent0)) {
      this->set(i,r,g,b);
    }
  }
  
}
void lpd8806::update() {
  int fail=0;
  for(int i=0;i< (this->leds)*3;i++) {
    fail+=1-write(this->device, &(this->buffer[i]), 1);
  }
  fail+=1-write(this->device,&(this->command),1);
  if(fail>0) {
    throw new std::runtime_error("SPI write fail");
  }
  //std::cout<<"update() "<<fail<<" failures"<<std::endl;
}
int lpd8806::getGamma(unsigned char v) {
  return this->gamma[v];
}
int lpd8806::getLength() {
  return this->leds;
}
double lpd8806::rotate(double r,double angle) {
  r+=angle;
  if(r>(M_PI*2)) {
    r-=(M_PI*2);
  }
  return r;
}

void lpd8806::demo(int iterations) {
  double r_x_increment=(M_PI/this->leds)/2.0;
  double g_x_increment=(M_PI/this->leds)/2.0;
  double b_x_increment=(M_PI/this->leds)/2.0;
  double r_time_increment=M_PI/8.0;
  double g_time_increment=M_PI/9.0;
  double b_time_increment=M_PI/10.0;
  double rStart=0.0;
  double gStart=0;//M_PI/3.0;
  double bStart=0;//M_PI*2.0/3.0;
  double delayus=15000;

  double rPos=rStart;
  double gPos=gStart;
  double bPos=bStart;
  for(int cyc=0;cyc<iterations;cyc++) {
    rPos=rStart;
    gPos=gStart;
    bPos=bStart;

    for(int led=0;led< this->getLength();led++) {
      int r = ((sin(rPos)+1.0)*127);
      int g = ((sin(gPos)+1.0)*127);
      int b = ((sin(bPos)+1.0)*127);
      this->set(led,r,g,b);

      rPos=rotate(rPos,r_x_increment);
      gPos=rotate(gPos,g_x_increment);
      bPos=rotate(bPos,b_x_increment);
    }
    this->update();

    rStart=rotate(rStart,r_time_increment);
    gStart=rotate(gStart,g_time_increment);
    bStart=rotate(bStart,b_time_increment);

    usleep(delayus);
  }
  this->fill(0,0,0);
  this->update();
}



