# GPIO Switch Input

Arduino C++ code to handle local GPIO input for switches e.g. light wall switches.

The IO pins to be used for switches are added to an array. The switch state is also keept in anouther array. When a switch is changed a callback function is called. Switch debouncing and fast switch changed are also tracked.

The idea is to make is easy to add expanders etc. by just overriding minimal metods to keep a standard interface.

I am using this with my wired ethernet web page and MQTT light controller code.

### Local MCU GPIO pin input, i.e. light wall switches.

1. Keeps track of current state. On, off etc.
2. make sure any change in state is not checked again for a bit to de-bounce switches etc. (if the fuction called in the main loop is called faster than a 1/4 of a second it just returns without checking.) That should stop bounse but a guess a voltage spike could still give a faulse triger but I think that should not be a problem at least as long as the wire are not tway long.
3. keep track of quick switches, slower than debounce but faster than a second or 2 for extra functionality. Turning all light on for example.
4. Callback function for when a switch in changed.
5. Only works from a function called in the main loop at the min. For switch flicking count to work will need to be called at least every 1/2 second or   so.
6. Todo add code so it works with the rassbery PI pico and other RP2040 boards
7. [ ] Todo Speed up the Pinread by using the pin registers instead of using the Arduino DigitalRead() function. Even if it just uses registers to check for change it should help speed up the main loop time. even more inportet if checkig with a timer interupt.
8. [ ] Maybe add the option to use an timer to check for change insted of calling in the main loop.
9. [ ] The timings on the quick shwitch change count needs more testing to see haw usable it is is real use.

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
