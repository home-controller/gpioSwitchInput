# GPIO Switch Input

I.h = Higher level switch input. e.g. input: wall light switches.
       1: Keeps track of current state. On, off etc.
       2: make sure any change in state is not checked again for a bit to de-bounce switches etc.
       3: keep track of quick switches, slower than debounce but faster than a second or 2 for extra functionality.
          Turning all light on for example.
       4: ? handle keeping track of what the switch will do when switched.
       5: move to higher level? group handling.

## refactoring

1. IO.h will be split into a different library for each type. i2c expanders arduino's pins & shift registers etc.
2. I.h and O.h will still stay more or less the same but should allow different low level IO libs to be added.
3. Should only need the Stadard <Arduino.h> for Serial.print() etc.

## To use

At top of main.h or .ino etc. add

```c++
 #define pinIO_no_of_switches 5 //setup the number of gpio's used
 #define pinIO_inPins A7,A6,A0,A1,A2 in sa main.h
 byte pinIO_switchState[pinIO_no_of_switches]; 
 byte pinIO_pinsA_in[pinIO_no_of_switches];
```

 Then in main loop.

```c++
pinIO_SwitchesExe(); //needs to be called in the main loop, hopefully at least every 1/4 second or may mess up switch flick count.
```
