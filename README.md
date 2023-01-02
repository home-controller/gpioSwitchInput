# GPIO Switch Input

Local MCU GPIO pin input: wall light switches.
       1: Keeps track of current state. On, off etc.
       2: make sure any change in state is not checked again for a bit to de-bounce switches etc.
       3: keep track of quick switches, slower than debounce but faster than a second or 2 for extra functionality.
          Turning all light on for example.
       4: Callback function for when a switch in changed.
       5: Only works from a function called in the main loop at the min. For switch flicking count to work will need to be called at least every 1/2 second or so.

## dependencies

1. Should only need the Standard <Arduino.h> for Serial.print() etc. and <avr/wdt.h> for watchdog timer.
2. I use <defs.h>(<https://github.com/home-controller/defs>) instead of <Arduino.h>. defs.h just #includes <Arduino.h> and #defines some stuff so that it is all in 1 place.

## To use

At top of main.h or .ino etc. add
/example folder may have more and/or (more up to date) examples

```c++
 #define pinIO_no_of_switches 5 //setup the number of gpio's used
 #define pinIO_inPins A7,A6,A0,A1,A2
 byte pinIO_switchState[pinIO_no_of_switches]; 
 byte pinIO_pinsA_in[pinIO_no_of_switches] = {pinIO_inPins};
 gpioSwitchInputC gpioIn(pinIO_no_of_switches, 0, pinIO_switchState, pinIO_pinsA_in);
```

 Then in main loop.

```c++
gpioIn.pinIO_SwitchesExe(); //needs to be called in the main loop, hopefully at least every 1/4 second or may mess up switch flick count.
```
