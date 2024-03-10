#ifndef SERIALCOMMANDS_H //check if already defined/included in compiler
#define SERIALCOMMANDS_H //only define if not defined before

//Author: simrem singh

#include <Arduino.h>

String getUserInput()
{
  String command;
  byte streamlenght = 0;
  bool overflow = false; // end of Stream - prevent streamlenght overflow
  char inputChar;
  while (Serial.available() != 0)
  {
    streamlenght += 1;
    inputChar = Serial.read();
    delay(100);

    if (overflow)
    {
      Serial.print(inputChar);
    }
    else
    {
      overflow = (streamlenght > 4);
      if (!overflow)
      {
        command += inputChar;
      }
      else
      {
        Serial.print(F("Input above 4:"));
        Serial.print(inputChar);
      }
    }
  }
  return command;
}

#endif