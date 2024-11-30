
#include "gpioSwitchInput.h"

#include <defs.h>
// #include <Arduino.h> // is needed for analogRead(),digitalRead(),pinMode(), millis() and the #defines A6,A7

// #include <CDC.cpp>
// #include </home/jmnc2/.platformio/packages/framework-arduino-avr/cores/arduino/HardwareSerial.h>
// #include "O.h"
// #include <avr/wdt.h>
// #include <EEPROM.h>
// #include "mqtt.h"
#ifndef _defs_h
#define ioLocalPin 0  // see defs.h
#endif

/**
 * @brief Construct a new gpio Switch Input object
 *
 * @param n Number of wall switch input pins therefore also the size of the pinsA[], At leas the part of it used here.
 * @param offSetN A value we add to the array pointers "stateA" and "pinsA". Only used 1 time here at setup.
 * @param stateA Array to store the state of the switches in (This is a pointer).
 * @param pinsA Array of GPIO pins with switches on them (This is a pointer).
 */
gpioSwitchInputC::gpioSwitchInputC(byte n, byte offSetN, byte stateA[], byte pinsA[]) : nOfPins(n), offset(offSetN) {
    // nOfPins = n;
    switchStateA = stateA + offset;
    gpioInA = pinsA + offset;
    // offset = offSetN;
    lastMils = millis();
#ifndef gpioDebugSetup
    SetUpInputs();  // Serial etc. probably isn't initalized. For debugging this will need to be called in setup. Should be fine to call twice.
#endif
}

/**
 * @brief Check GPIO pin for high or low logic level. Adds A6 and A7 to standard "digitalRead(pin);"
 *
 * @param pin GPIO pin number e.g. A1, D3, 27 etc.
 * @return boolean pin high true, pin low false
 */
boolean gpioSwitchInputC::digitalReadATmega328P(byte pin) {
    if ((pin == A6) or (pin == A7))  // digitalRead() don't works with A6 and A7 so check voltage with analogRead(pin) instead.
    {
        if (analogRead(pin) > 600) {
            return HIGH;
        }
        return LOW;
    }
    return digitalRead(pin);
}

/**
 * @brief Check GPIO pin from gpioInA[i] at index i for high or low logic level(ie. voltage).
 *
 * @param i index into array(gpioInA[i]) of GPIO pin numbers.
 * @return boolean pin high true, pin low false
 */
boolean gpioSwitchInputC::ReadSwitch(byte i) {
    byte pin;
    pin = gpioInA[i];
#ifdef __AVR_ATmega328P__
    return digitalReadATmega328P(pin);
#else
    return digitalRead(pin);
#endif
}

/**
 * @brief Set the GPIO input pints to INPUT_PULLUP and set the switchStateA[] array
 *
 */
void gpioSwitchInputC::SetUpInputs() {
    //   nOfPins = n;
    // switchStateA = stateA;
    // gpioInA=pinsA;

    byte i;
    // Serial.println(F("Todo: still only MCU pins input. No expanders") );//expanders etc. should be in different unit maybe.
    for (i = 0; i < nOfPins; i++) {
#ifdef gpioDebugSetup
        if (i == 0) Serial.print(F("\n\rGPIO pins connected to switches: "));
        else
            Serial.print(", ");

        if (gpioInA[i] >= A0) {
            Serial.print(F("A"));
            Serial.print(gpioInA[i] - A0);
            Serial.print('(');
            Serial.print(gpioInA[i]);
            Serial.print(")");
        } else
            Serial.print(gpioInA[i]);
        if ((i + 1) >= nOfPins)Serial.println();
#endif

            // Serial.print(F("' set to input, Switch No. = "));
            // Serial.println(i);
            pinMode(gpioInA[i], INPUT_PULLUP);  // sets the input pins as input. Pullup works on A0 to A5 but A6 & A7 still float
        // pinMode(gpioInA[i], INPUT);      // sets the input pins as input. If not use pullup may be able to detect bad connection.
        if (ReadSwitch(i)) {
            switchStateA[i] = 0b11;
        } else {
            switchStateA[i] = 0b0;
        }
    }
}

#ifdef _debug_switches
void gpioSwitchInputC::debugSwitch(byte i) {
    Serial.print(F("i: "));
    Serial.print(i);
    Serial.print(F(", pin: "));
    if (gpioInA[i] >= A0) {
        Serial.print(gpioInA[i]);
        Serial.print('(');
        Serial.print('A');
        Serial.print(gpioInA[i] - A0);
        Serial.print(')');
    } else
        Serial.print(gpioInA[i]);
    Serial.print(F(", \e[7;33mstate( "));
    Serial.print(ReadSwitch(i));

    Serial.print(F(",\e[34m saved: b"));
    Serial.print(switchStateA[i] & 0b1, BIN);
    Serial.print(F(",\e[33m New: b"));
    Serial.print((switchStateA[i] >> 1) & 0b1, BIN);
    Serial.print(F(")\e[0;94m, Count: "));
    Serial.print((switchStateA[i] >> 2) & 0b11);
    Serial.print(F("(b"));
    Serial.print((switchStateA[i] >> 2) & 0b11, BIN);
    Serial.print(F("),\e[93m 1/8s: b"));
    Serial.print((switchStateA[i]) >> 4, BIN);
    Serial.print(F("\e[97m, switchStateA[i]: "));
    Serial.print(switchStateA[i], BIN);
    Serial.print(F(", analogRead: "));
    Serial.println(analogRead(gpioInA[i]));
}
#endif

/**
 * @brief Update the 'current state' bit in gpioInA[] array.
 *
 * This updates the 'current state' bit in switchState array for each switch.
 * This will overwrite the value without updating any other bits, eg. count.
 *
 * If any pins are A6 or A7 then arduino digital read and INPUT_PULLUP don't work.
 */
void gpioSwitchInputC::getInputStates() {
    byte i;
    bool s;
#ifdef gpioDebug
    // #define _debug_switches_gis
    Serial.println(F("entering getInputStates()"));
#endif

    for (i = 0; i < nOfPins; i++) {
        s = ReadSwitch(i);
        if (s != ((switchStateA[i] >> 1) bitand 0b1))  // if changed since last check
        {
#ifdef gpioDebug
            Serial.print(F("Switch("));
            Serial.print(i);
            Serial.println(F(") state changed"));
#endif
            // debugSwitch(i);
            // xor returns 1 where the bits are different.
            if ((((switchStateA[i] >> 1) xor (switchStateA[i])) bitand 0b1) == 0b1)  // if bits 0010 and 0001 are different(curent and update switch pos)
            {
                if ((switchStateA[i] bitand 0b1100) < 0b1100)
                    switchStateA[i] += (byte)0b100;
            }
            switchStateA[i] = switchStateA[i] xor 0b10;  // xor flips any bit where 1 side is 1.
        }
/*
 * switchState[i]:
 *  state          : 0b0000 0001  Fully updated state, including updated mqtt etc. 0 = off 1 = on
 *  current state  : 0b0000 0010  State at last check. Current switch state but count may still be updating etc. 0 = off 1 = on
 *  change count   : 0b0000 1100  Number of changes within 0.1 and 2 seconds between changes. max changes 8, maybe 7. Use difference of bit 1 and 2 as first bit
 *  time         : 0b1111 0000 time in 1/8 seconds since last change. Within approx 1/8 second
 */
#ifdef _debug_switches_gis
        debugSwitch(i);
#endif
    }
#ifdef _debug_switches_gis
    wdt_disable();
    delay(10000);
    wdt_enable(WDTO_8S);
#endif
}

#define s1Msk B00000001
#define s2Msk B00000010
#define s3Msk B00000100

/** Check for changes no more than 1/8 of a second, If sending mqtt or web page may be longer?. Should be good for debounce?
 * If same switch changes again in less than 2 seconds add one to switch state to max of 8.
 * stateVar      : 0b0000 0000
 *  state          : 0b0000 0001  Fully updated state, including updated mqtt etc. 0 = off 1 = on
 *  current state  : 0b0000 0010  State at last check. Current switch state but count may still be updating etc. 0 = off 1 = on
 *  change count   : 0b0000 1100  Number of changes within 0.1 and 2 seconds between changes. max changes 8, maybe 7. Use difference of bit 1 and 2 as first bit
 *  time         : 0b1111 0000 time in 1/8 seconds since last change. Within approx 1/8 second
 **/

/**
 * @brief Called by SwitchesExe(); after a GPIO pin used for a input switch has been switched.
 *
 * ln sw_i index into gpioInA[]
 * @param count Number of times the Switch was flicked
 * @param state Current switch state.
 */
void gpioSwitchInputC::Switched(byte sw_i, byte count, byte state) {  // Count 0 is quick on. sw = 0 for first switch
#ifdef gpioDebug
    Serial.println(F("Called Switched"));
#endif
    if (callbackSwitched != nullptr) {
        callbackSwitched(ioLocalPin, sw_i, offset, count, state);
    }
}

/**
 * @brief This check if any switches have turned on or off etc, only checks if 1/4 of a second has passed to work as debounce and save time.
 * 
 */
void gpioSwitchInputC::SwitchesExe() {
    byte state1;    // The state that is pushed to the lights, MQTT etc.
    byte pinState;  // The new up to date GPIO pin state. Lights, MQTT etc. may still need updating.
    byte i, count1, time1;

#ifdef _debug_switches
    // Serial.println(F("entering SwitchesExe()") );
#endif
    if ((millis() - lastMils) < (1000 / 4))  // 1/4th of a second. If called just under 1/8 of a second and again 1/8 later.
    {                                        // Could be 1/4 second between switch checks even when called at close to every 1/8th second.
        return;
    }
    getInputStates();  // update the curent switch state bit. Just that, no turning on lights etc.
    for (i = 0; i < nOfPins; i++) {
        state1 = switchStateA[i] bitand 0b1;
        pinState = (switchStateA[i] >> 1) bitand 0b1;
        count1 = state1 xor pinState;
        count1 |= (switchStateA[i] >> 1) bitand 0b110;
        time1 = switchStateA[i] >> 4;
        if ((count1 == 0) and ((time1 == 0) or (time1 == 0b1111)))
            continue;  // If everything is up to date then skip to check next switch;
        // if(switchStateA[i]==0b00010011)
        if (time1 >= 0b1111)  // Switch flicking disabled.
        {
            Switched(i, 1, 1);
            if (pinState > 0)
                switchStateA[i] = 0xf0 + 0b11;
            else
                switchStateA[i] = 0xf0;
        } else {
            if ((count1 == 1) and (time1 == 0) and (pinState == 1))  // first switch, with flick count.
            {
                Switched(i, 0, 1);                      // Switch relays etc. Quick call
                                                        // save time1 = 1 back to the array
                                                        // switchStateA[i] &= (byte)0b1111;
                switchStateA[i] = 0b10010;              //
            } else if (count1 >= 7 || time1 >= 0b1001)  // check for max flicks or timeout after 2 seconds. max is 0v1110 as 0xf is no switch flick 8+1=9
            {                                           // reached max change count or time since last change > 2 seconds
#ifdef _debug_switches
                Serial.print(F("Switch state update "));
                Serial.print(F(", count1 = "));
                Serial.print(count1);
                Serial.print(F(", timeout in 1/8 seconds = "));
                Serial.print(time1);
                Serial.print(F(", switch No. = "));
                Serial.println(i);
#endif
                Switched(i, count1, pinState);  // Switch relays etc.
                // and update switchState[i] to curent state
                // reset time and count.
                if (pinState > 0)
                    switchStateA[i] = 0b11;
                else
                    switchStateA[i] = 0;
            } else {
                switchStateA[i] &= (byte)0b1111;
                if (time1 < 0b1110)
                    time1++;
                switchStateA[i] |= ((time1) << 4);
            }
        }
    }
    lastMils = millis();
}
