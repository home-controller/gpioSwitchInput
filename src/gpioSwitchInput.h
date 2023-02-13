/**
 * @file gpioSwitchInput.h
 * @author Jmnc2 (on github)
 * @brief handles local input MCU pins. Switches connected directly to a GPIO pin
 * @version 0.2.11
 * @date 2023-01-02
 *
 * @copyright Copyright (c) 2023
 *
 *
 * example use:
 * See example dir.
 */

#ifndef _gpio_in_h
#define _gpio_in_h

#include "Arduino.h"

// #ifndef byte
// typedef __UINT8_TYPE__ uint8_t;
// typedef uint8_t byte;
// typedef bool boolean;
// #define HIGH 0x1
// #define LOW  0x0
// #define INPUT_PULLUP 0x2
// #endif

// #define inPins A7,A6,A0,A1,A2
// #define no_of_switches 5
#define in_initHLa = B00000011 //{HIGH,HIGH}// the value to use to for on.

/*
 * simple count meaning:
 * 1: tog
 * 2, 3: all hard on/off
 */

/** Check for changes no more than 1/8 of a second, If sending mqtt or web page may be longer?. Should be good for debounce?
 * If same switch changes again in less than 2 seconds add one to switch state to max of 8.
 * switchState[i]  : 0b0000 0000
 *  state          : 0b0000 0001  Fully updated state, including updated mqtt etc. 0 = off 1 = on
 *  current state  : 0b0000 0010  State at last check. Current switch state but count may still be updating etc. 0 = off 1 = on
 *  change count   : 0b0000 1100  Number of changes within 0.1 and 2 seconds between changes. max changes 8, maybe 7. Use difference of bit 1 and 2 as first bit
 *  time         : 0b1111 0000 time in 1/8 seconds since last change. Within approx 1/8 second
 **/
// extern byte switchOnVal;// eg 0 for pull the relay LOW to turn on, 1 for pull high. Pull opposite to turn off. Or should it be set to input to turn off?

// extern byte switchInfo[no_of_switches][3];

// extern byte groupsSateA[];

/**
 * @brief type def for call back function for when a switch is switched.
 * // gotInputPin(byte ioType, byte i, byte count, byte state);
 *
 * @param ioType
 * @param index into the arrays storing the pin/switch state and the stitch to GPIO pin mapping
 * @param offset inside the arrays.
 * @param count of the number of times the switch has been flicked.
 * @param state of the switch.
 *
 * @return void. (no return value.)
 *
 */
using CallbackT = void (*)(byte, byte, byte, byte, byte);

class gpioSwitchInputC
{
private:
    /* data */
    byte nOfPins;       // The number of gpio pins used for swich(wall light switches etc.) input
    byte *switchStateA; // pointer to array of bytes used to store the stat of the switch.
    byte *gpioInA;      // Array of the GPIO pin numbers used. As this and the above are pointes to an array it can point to the start of the array inside a bigger array.
    byte offset = 0;    // This is only used with the callback func.
    unsigned long lastMils;

    // void (*callbackSwitched)(byte, byte, byte, byte, byte) = nullptr;
    CallbackT callbackSwitched = nullptr;

    void debugSwitch(byte i);
    void getInputStates();
    void Switched(byte sw_i, byte count, byte state);

public:
    gpioSwitchInputC(byte n, byte offSetN, byte stateA[], byte pinsA[]);
    void SetCallback(CallbackT fncP) { callbackSwitched = fncP; }

    void SetUpInputs();
    //boolean checkInput();

    boolean ReadSwitch(byte i);
    //~gpioSwitchInputC();
    void SwitchesExe();

    static boolean digitalReadATmega328P(byte pin);
};


#endif
