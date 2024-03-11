#include <Arduino.h>
#include "SerialCommands.h"
#include "MemoryManagment.h"
#include "Logger.h"

// Author: simrem singh

const byte MovementSensorPin = 53;
const unsigned int MovementActivityTime = 5000;

bool movementActive;
bool currentMovementActive;

unsigned long timeTill;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(MovementSensorPin, INPUT);
  // default values
  initLogger();

}

void loop()
{
  checkMovement();
  checkUserInput();
}

void checkMovement()
{
  currentMovementActive = digitalRead(MovementSensorPin) == HIGH;
  if (movementActive || currentMovementActive)
  { // movement sensor activity
    if ((!movementActive) && currentMovementActive)
    { // first contact
      Serial.println();
      Serial.print(F("Movement detected"));
      timeTill = millis() + MovementActivityTime;
      movementActive = true;

      createTimeStamp();
    }
    else if (movementActive && currentMovementActive)
    { // recurring contact (first contact not finished yet)
      timeTill = millis() + MovementActivityTime;
    }
    else// if (movementActive && (!currentMovementActive))
    { // check if contact finished
      movementActive = (timeTill < millis());
      if (!movementActive)
      { // contact finished
        Serial.println();
        Serial.println(F("Movement stopped"));
      }
    }
  }
}

void checkUserInput()
{
  if (Serial.available() != 0)
  {
    interpret(getUserInput());
  }
}

void interpret(String UserCommand)
{
  if (UserCommand == "")
  {
    Serial.print(F("No Input."));
    Serial.println();
  }

  Serial.println();
  Serial.print(F("Command:"));
  UserCommand.toUpperCase();
  UserCommand.trim();
  Serial.print(UserCommand);
  Serial.println();

  if (UserCommand == "CLER" || UserCommand == "CLR")
  {
    clearLog();
    return;
  }

  if (UserCommand == "HELP" || UserCommand == "?")
  {
    Serial.print(F("?,help = get Commands"));
    Serial.println();
    Serial.print(F("cler = reset log"));
    Serial.println();
    Serial.print(F("get,log,list = get log"));
    Serial.println();
    Serial.print(F("set = start set time sequence"));
    Serial.println();
    return;
  }
  if (UserCommand == "GET" || UserCommand == "LOG" || UserCommand == "LIST")
  {
    PrintLog();
    return;
  }

  if (UserCommand == "SET")
  {
    SetBaseTime();
    return;
  }
  Serial.print(F("input not valid. Write help to get valid inputs"));
}

