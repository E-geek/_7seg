#include <StandardCplusplus.h>
#include <system_configuration.h>
#include <unwind-cxx.h>
#include <utility.h>

/*
********************************************************************
  Name    : shiftOutDisplay, Test code
  Author  : Benjo Charlie
  Date    : 13 Sept, 2013
  Version : 1.0
  Notes   : Shiftbit registers can be a little tricky to setup so I
          : designed this simple Hello World code for doing just that.
          : This example uses a 74HC595 Shift Bit Register in conjunction
          : with a common anode seven segment display.
          : This code sends the following values to a Shift Bit Register: 
          : 1, 2, 4, 8, 16, 32, 64, 128. 
          : Each of the 74HC595 outputs will "switch on" in turn.
********************************************************************

7 Segment (Common Anode) Display Map: (This can be altered to reflect your HW)

    D   E  5V   F   G
 ___|___|___|___|___|____
|                        |
|        F               |
|    E       G           |
|        D               |
|    A       C           |
|        B       H(Dot)  |
|________________________|
    |   |   |   |   |
    A   B  5V   C   H

74HC595 Map:
           _______
   OUT Q1-|1 *  16|-5V
   OUT Q2-|2    15|-OUT Q0
   OUT Q3-|3    14|-ARD PIN 11
   OUT Q4-|4    13|-GND
   OUT Q5-|5    12|-ARD PIN 8 ; 1uF TO GND
   OUT Q6-|6    11|-ARD PIN 12 
   OUT Q7-|7    10|-5V
      GND-|8_____9|-

********************************************************************
*/
#define DEBUG

#include <Wire.h>
#include <stdio.h>
#include <stdarg.h>

#define DS3231_ADDRESS  0x68
#define DS3231_CONTROL  0x0E
#define DS3231_STATUSREG 0x0F

#include "./lintex/clog.cpp"
#include "./lintex/i2c.cpp"

//Pin connected to ST_CP of 74HC595
#define LATCH_PIN 2 
//Pin connected to SH_CP of 74HC595
#define CLOCK_PIN 7
//Pin connected to DS of 74HC595
#define DATA_PIN 4

#define BTN_ENTER 11
#define BTN_CHANGE 10
#define BTN_SHIFT 9
#define BTN_NEXT 8

byte g_digits[18]= {
  0x88, // 0
  0xbb, // 1
  0x94, // 2
  0x91, // 3
  0xa3, // 4|ч
  0xc1, // 5
  0xc0, // 6
  0x9b, // 7
  0x80, // 8|в
  0x81, // 9
  0x7f, // .      // 10
  0xff,  // null  // 11
  B11000000, // Б // 12
  B10100010, // Н // 13
  B10001010, // П // 14
  B10000110, // Р // 15
  B11001100, // С // 16
  B11100100  // Т // 17
  };
  
byte weekDay0[7] = {14, 8,16, 4,14,16, 8};
byte weekDay1[7] = {13,17,15,17,17,12,16};

#include "./lintex/event-target.cpp"
#include "./lintex/button.cpp"
#include "./lintex/print.cpp"
#include "./lintex/actual.cpp"
#include "./lintex/logic.cpp"

Button *btnEnter;
Button *btnNext;
Button *btnChange;
Button *btnShift;

void setup() {
  //set pins to output so you can control the shift register
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  Wire.begin();
  Serial.begin(57600);
  lvl0 = 0;
  lvl1 = 0;
  shift = change = false;
  clog("Base init");
  btnNext = new Button(BTN_NEXT);
  btnEnter = new Button(BTN_ENTER);
  btnChange = new Button(BTN_CHANGE);
  btnShift = new Button(BTN_SHIFT);
  // bind
  btnNext->on("press", onNext);
  btnEnter->on("press", onEnter);
  btnChange->on("down", onChangeDown)->on("press", onChangePress);
  btnShift->on("down", onShiftDown)->on("press", onShiftPress);
  clog("Bind done");
}

void loop() {
  checkButtons();
  if ((lvl0 | lvl1) == 0) {
    actualClockTime();
  } else if (flash > 0) {
    flashDisplay();  
  }
}


