#include <Arduino.h>
#include "SerialCommands.h"
#include "MemoryManagment.h"
#include "Logger.h"

// Author: simrem singh

const byte MOVMENT_SENSOR_PIN = 53;
const unsigned int MOVEMENT_ACTIVITY_TIME = 5000;

bool movementActive;
bool movementSensed;

unsigned long timeTillActivityDepricated;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(MOVMENT_SENSOR_PIN, INPUT);
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
  movementSensed = digitalRead(MOVMENT_SENSOR_PIN) == HIGH;
  if (movementActive || movementSensed)
  { // movement sensor activity
    if ((!movementActive) && movementSensed)
    { // first contact
      Serial.println();
      Serial.print(F("Movement detected"));
      timeTillActivityDepricated = millis() + MOVEMENT_ACTIVITY_TIME;
      movementActive = true;

      createTimeStamp();
    }
    else if (movementActive && movementSensed)
    { // recurring contact (first contact not finished yet)
      timeTillActivityDepricated = millis() + MOVEMENT_ACTIVITY_TIME;
    }
    else// if (movementActive && (!movementSensed))
    { // check if contact finished
      movementActive = (timeTillActivityDepricated < millis());
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

void interpret(String userCommand)
{
  if (userCommand == "")
  {
    Serial.print(F("No Input."));
    Serial.println();
  }

  Serial.println();
  Serial.print(F("Command:"));
  userCommand.toUpperCase();
  userCommand.trim();
  Serial.print(userCommand);
  Serial.println();

  if (userCommand == "CLER" || userCommand == "CLR")
  {
    clearLog();
    return;
  }

  if (userCommand == "HELP" || userCommand == "?")
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
  if (userCommand == "GET" || userCommand == "LOG" || userCommand == "LIST")
  {
    PrintLog();
    return;
  }

  if (userCommand == "SET")
  {
    setBaseTime();
    return;
  }
  Serial.print(F("input not valid. Write help to get valid inputs"));
}

