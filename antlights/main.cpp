#include <stdio.h>
#include <stdexcept>
#include <cstdlib>
#include <iostream>

#include "SecondHand.h"
#include "lpd8806.h"
#include "antlights.h"

void doTest() {
  int channel=0;

  // turn up to [maxleds] lights off, first
  int maxleds=32;
  std::cout << "blanking " << maxleds << " on channel " << channel << std::endl;
  lpd8806* largebar=new lpd8806(channel,maxleds);
  largebar->fill(0,0,0);
  largebar->update();
  delete largebar;

  // now define bar with smaller # of lights for lightshow
  int leds=24;
  lpd8806* bar=new lpd8806(leds,channel);
  std::cout << "gamma table:"<< std::endl;
  for(int i=0;i<16;i++) {
    for(int j=0;j<16;j++) {
      std::cout << bar->getGamma(i*16+j) << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "demo mode, leds = " << leds << std::endl;
  bar->demo(10);
  delete bar;

  startThreads();
  joinThreads();

  std::cout << "done, exit" << std::endl;

}

int main() {
  try {
    doTest();
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

