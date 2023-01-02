
//#include <defs.h>
#include "Arduino.h"
#include <gpioSwitchInput.h>

#define serial_speed 38400

#define pinIO_no_of_switches 6 // setup the number of gpio's used
#define pinIO_inPins A7, A6, A0, A1, A2, A3 // in sa main.h

byte pinIO_Max_switches = pinIO_no_of_switches;
byte pinIO_switchState[pinIO_no_of_switches];
byte pinIO_pinsA_in[pinIO_no_of_switches] = { pinIO_inPins };

void gotInputPin(byte ioType, byte i,byte offset, byte count, byte state){// Callback when a switch changes
 Serial.print(F("ioType:")); Serial.print(ioType);
 Serial.print(F(", i:")); Serial.print(i);
 Serial.print(F(", count:")); Serial.print(count);
  Serial.print(F(", state:")); Serial.println(state);

}

gpioSwitchInputC gpioIn(6,0,pinIO_switchState,pinIO_pinsA_in);

void setup(){
      Serial.begin(serial_speed);
  delay(250);
  // io_print( "Serial.begin(" );  io_print_n( serial_speed );  io_println( ")" );
  Serial.print(F("Serial.begin("));
  Serial.print(serial_speed);
  Serial.println(F(")"));

  gpioIn.SetCallback(&gotInputPin);
}

void loop()
{
    gpioIn.SwitchesExe(); // needs to be called in the main loop, hopefully at least every 1/4 second or may mess up switch flick count.
    delay(100);
}