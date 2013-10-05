objects = antlights.o main.o lpd8806.o Gear.o LedBlinkGear.o LedRefreshGear.o SecondHand.o LedSweepGear.o

CPPFLAGS = -g -O2 -Wall

antlights : $(objects)
	g++ -g -o antlights -lwiringPi -lrt $(objects) 

test : antlights
	gpio load spi && ./antlights 

antlights.o : antlights.h Gear.h LedBlinkGear.h SecondHand.h LedSweepGear.h
Gear.o : Gear.h SecondHandTick.h
LedBlinkGear.o : LedBlinkGear.h Gear.h LedRefreshGear.h lpd8806.h
LedSweepGear.o : LedSweepGear.h Gear.h lpd8806.h
LedRefreshGear.o : LedRefreshGear.h Gear.h lpd8806.h
SecondHand.o : SecondHand.h SecondHandTick.h Gear.h
main.o : lpd8806.h SecondHand.h
lpd8806.o : lpd8806.h

.PHONY : clean
clean :
	rm antlights $(objects) *~
