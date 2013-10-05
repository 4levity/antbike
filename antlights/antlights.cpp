#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <stdexcept>
#include <wiringSerial.h>
#include <pthread.h>
#include <termios.h>

#include "LedBlinkGear.h"
#include "SecondHand.h"
#include "LedSweepGear.h"
#include "LedRefreshGear.h"
#include "antlights.h"


pthread_t serialThread;
pthread_t commandLineThread;

/* bitmaps and various things are all hard coded for a string of 24 leds */
#define LEDS 24
bool shutdown=false;

LedBlinkGear* turnLeft;
#define TURNSIGNAL_LAYER 500
#define TURNSIGNAL_BPM 60
const unsigned char turnSignalLeftRGB[LEDS*3] = {
  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0xFF,0xFF,0x0,0xFF,0xFF,0x0,
  0xFF,0xFF,0x0,0xFF,0xFF,0x0,0x0,0x0,0x0,  0x0,0x0,0x0,
  0xFF,0xFF,0x0,0xFF,0xFF,0x0,0xFF,0xFF,0x0,0x0,0x0,0x0};

LedBlinkGear* turnRight;
const unsigned char turnSignalRightRGB[LEDS*3] = {
  0x0,0x0,0x0,  0xFF,0xFF,0x0,0xFF,0xFF,0x0,0xFF,0xFF,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0xFF,0xFF,0x0, 0xFF,0xFF,0x0,
  0xFF,0xFF,0x0,0xFF,0xFF,0x0,0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0};

LedBlinkGear* brakes;
#define BRAKELIGHTS_LAYER 400
const unsigned char brakesRGB[LEDS*3] = {
  0xFF,0x0,0x0, 0xFF,0x00,0x0,0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,  0x0,0x0,0x0,
  0x0,0x0,0x0,  0x0,0x0,0x0,  0xFF,0x0,0x0, 0xFF,0x0,0x0};

LedBlinkGear* runningLights;
#define RUNNINGLIGHTS_LAYER 80
const unsigned char runningLightsRGB[LEDS*3] = {
  0x0,0x0,0x0,   0x3F,0x00,0x0, 0x3f,0x3f,0x0, 0x0,0x0,0x0,
  0x0,0x0,0x0,   0x0,0x0,0x0,   0x0,0x0,0x0,   0x3f,0x3f,0x0,
  0x0,0x0,0x0,   0x0,0x0,0x0,   0x1f,0x1f,0x1f,0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,0x1f,0x1f,0x1f,0x0,0x0,0x0,   0x0,0x0,0x0,
  0x3f,0x3f,0x0, 0x0,0x0,0x0,   0x0,0x0,0x0,   0x0,0x0,0x0,
  0x0,0x0,0x0,   0x3f,0x3f,0x0, 0x3F,0x0,0x0,  0x0,0x0,0x0};

LedBlinkGear* brights;
#define HEADLIGHTS_LAYER 100
const unsigned char brightsRGB[LEDS*3] = {
  0x0,0x0,0x0,   0x00,0x00,0x0, 0x00,0x00,0x0, 0x0,0x0,0x0,
  0x0,0x0,0x0,   0x0,0x0,0x0,   0x0,0x0,0x0,   0x00,0x00,0x0,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0x0,0x0,0x0,   0x0,0x0,0x0,   0x0,0x0,0x0,   0x00,0x00,0x0,
  0x0,0x0,0x0,   0x0,0x0,0x0,   0x0,0x0,0x0,   0x00,0x00,0x0};

#define KITT_LAYER 450
#define KITT_TRAIL 5
#define KITT_BORDER 3
#define KITT_RPM 30
LedSweepGear* kitt;
#define SECURITY_LAYER 50
#define SECURITY_TRAIL 3
#define SECURITY_BORDER 20
#define SECURITY_RPM 20
LedSweepGear* security;

LedBlinkGear* background;

#define LED_REFRESH_RATE_HZ 24
SecondHand* LedSpinner;

int fdSerial=-1;


void createGears() {
  // black background at layer 0, started
  LedSpinner->insertGear(background=(new LedBlinkGear(0,24,0,0,0, 0, 1, 100)));

  // led refresher (layer 999), started
  LedRefreshGear::create(24);
  LedSpinner->insertGear(LedRefreshGear::getSingle());

  // available light effects, stopped
  LedSpinner->insertGear(security=(new LedSweepGear(0,LEDS,
						    SECURITY_BORDER,
						    SECURITY_RPM,
						    SECURITY_LAYER))
			 ->configure(0x7f,0,0,SECURITY_TRAIL));//dim red
  security->stop();
  LedSpinner->insertGear(kitt=(new LedSweepGear(LEDS/3,LEDS/3,
						KITT_BORDER,KITT_RPM,
						KITT_LAYER))
			 ->configure(0xff,0xff,0xff,KITT_TRAIL));//white
  kitt->stop();
  LedSpinner->insertGear(runningLights=(new LedBlinkGear(runningLightsRGB,
							 RUNNINGLIGHTS_LAYER,
							 1,100)));
  runningLights->stop();
  LedSpinner->insertGear(brakes=(new LedBlinkGear(brakesRGB,
							 BRAKELIGHTS_LAYER,
						  1,100)));
  brakes->stop();
  LedSpinner->insertGear(brights=(new LedBlinkGear(brightsRGB,
							 HEADLIGHTS_LAYER,
						   1,100)));
  brights->stop();
  LedSpinner->insertGear(turnLeft=(new LedBlinkGear(turnSignalLeftRGB,
							 TURNSIGNAL_LAYER,
						    TURNSIGNAL_BPM,50)));
  turnLeft->stop();
  LedSpinner->insertGear(turnRight=(new LedBlinkGear(turnSignalRightRGB,
							 TURNSIGNAL_LAYER,
						     TURNSIGNAL_BPM,50)));
  turnRight->stop();

}

void destroyGears() {
  delete turnLeft;
  delete turnRight;
  delete brights;
  delete brakes;
  delete runningLights;
  delete kitt;
  delete security;
  delete background;
  LedRefreshGear::destroy();
}

#define SERIALBUFLEN 20
void* serialComms(void* arg) {
  fdSerial = serialOpen("/dev/ttyAMA0",115200);
  if(fdSerial == -1) {
    std::cout <<"error opening serial port"<<std::endl;
  } else {
    int lastCh,bufLen;
    unsigned char buf[SERIALBUFLEN];
    //struct termios options;
    //    tcgetattr(fdSerial, &options);
    //options.c_cc[VTIME]=100;// 5 x 0.1 sec = 0.5 sec timeout
    //tcsetattr(fdSerial, TCSANOW, &options);

    while(!shutdown) {
      bufLen=0;
      do {
	lastCh=serialGetchar(fdSerial);
	if(lastCh>0) {
	  buf[bufLen]=((unsigned char)lastCh) % 256;
	  bufLen++;
	}
      } while(lastCh>0 && bufLen<SERIALBUFLEN && !shutdown);
      for(int i=0;i<bufLen;i++) {
	// process buffer
	std::cout << buf[i]<<std::flush;
      }
      if(lastCh<=0) {
	// timeout could indicate problem with mcu
	//std::cout<<"|timeout|"<<std::flush;
      }
    }
    serialClose(fdSerial);
  }
  return NULL;
}

void* commandLine(void* arg) {
  std::string cmd;
  std::cout<<"manual lighting commands, ? for help"<<std::endl;
  while(!shutdown) {
    std::cout <<"antlight:";
    getline(std::cin, cmd);
    if(cmd.compare(0,1,"b")==0) {
      std::cout << "brakes " << (brakes->toggle(true)->isActive()?"on":"off")
		<< std::endl;
    } else if(cmd.compare(0,1,"k")==0) {
      std::cout << "kitt " << (kitt->toggle(true)->isActive()?"on":"off")
		<< std::endl;
    } else if(cmd.compare(0,1,"l")==0) {
      turnRight->stop();
      turnLeft->start()->recenter();
      std::cout << "left turn" << std::endl;
    } else if(cmd.compare(0,1,"r")==0) {
      turnLeft->stop();
      turnRight->start()->recenter();
      std::cout << "left turn" << std::endl;
    } else if(cmd.compare(0,1,"c")==0) {
      turnLeft->stop();
      turnRight->stop();
      std::cout << "centered" << std::endl;
    } else if(cmd.compare(0,1,"n")==0) {
      std::cout << "running lights " 
		<< (runningLights->toggle(true)->isActive()?"on":"off") 
		<< std::endl;
    } else if(cmd.compare(0,1,"h")==0) {
      std::cout << "headlights " 
		<< (brights->toggle(true)->isActive()?"on":"off") 
		<< std::endl;
    } else if(cmd.compare(0,1,"s")==0) {
      std::cout << "security " 
		<< (security->toggle(true)->isActive()?"on":"off" )
		<< std::endl;
    } else if(cmd.compare(0,1,"q")==0) {
      shutdown=true;;
      std::cout << "quit (please wait) " 
		<< std::endl;
    }
  }
  return NULL;
}

void startThreads() { 
  LedSpinner=new SecondHand(LED_REFRESH_RATE_HZ);  
  createGears();
  LedSpinner->start();

  pthread_create(&serialThread,NULL,&serialComms,NULL);
  pthread_create(&commandLineThread,NULL,&commandLine,NULL);
}

void joinThreads() {
  // 
  pthread_join(serialThread,NULL);
  pthread_join(commandLineThread,NULL);
  LedSpinner->stop();
}
