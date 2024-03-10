#include <Arduino.h>
#include "SerialCommands.h"
#include "MemoryManagment.h"

// Author: simrem singh

const byte MovementSensorPin = 53;
const unsigned int MovementActivityTime = 5000;

bool movementActive;
bool currentMovementActive;

unsigned long timeTill;

byte monthDays[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // Days in each month

struct LogEntry
{
  byte day;
  byte month;
  // byte year;
  byte hour;
  byte minute;
  byte second;
  LogEntry *next; // linked list pointer
};

LogEntry *logListStack = NULL; // Head of the linked list
LogEntry baseValues;

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
    else if (movementActive && (!currentMovementActive))
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


void initLogger()
{
  baseValues.day = 10;
  baseValues.month = 3;
  baseValues.hour = 15;
  baseValues.minute = 5;
}

void PrintLog()
{
  Serial.println(F("Logged Entries:"));
  LogEntry *currentEntry = logListStack;
  while (currentEntry != NULL)
  {
    PrintLogEntry(currentEntry);
    currentEntry = currentEntry->next;
  }
  Serial.println();
}

void PrintLogEntry(LogEntry *printEntry)
{
  Serial.println();
  Serial.print(printEntry->day);
  Serial.print(F("/"));
  Serial.print(printEntry->month);
  Serial.print(F("/-"));
  Serial.print(printEntry->hour);
  Serial.print(F(":"));
  Serial.print(printEntry->minute);
  Serial.print(F(":"));
  Serial.println(printEntry->second);
}

void createTimeStamp()
{
  if (memoryLimitReached)
    return;

  byte tempMonth;
  byte tempDaysInMonth;
  unsigned long timeCalculation;

  LogEntry *newEntry = new LogEntry;
  timeCalculation = millis() / 1000; // millisends -> seconds
  newEntry->second = timeCalculation % 60;
  timeCalculation /= 60; // get seconds
  timeCalculation += baseValues.minute;
  newEntry->minute = timeCalculation % 60;
  timeCalculation /= 60; // get minutes
  timeCalculation += baseValues.hour;
  newEntry->hour = timeCalculation % 24;
  timeCalculation /= 24; // get days
  timeCalculation += baseValues.day;
  tempMonth = baseValues.month; // currentmonth
  tempDaysInMonth = monthDays[tempMonth - 1];
  while (tempDaysInMonth < timeCalculation)
  {
    timeCalculation -= tempDaysInMonth;
    tempMonth++;
    tempDaysInMonth = monthDays[tempMonth - 1];
  }
  newEntry->day = timeCalculation;
  newEntry->month = tempMonth;

  PrintLogEntry(newEntry);

  Serial.print(F("Free Memory:"));
  int freeMemory = getFreeMemory();
  Serial.print(String(freeMemory));
  Serial.println();
  memoryLimitReached = freeMemory < FreeMemoryLimit;
  if (memoryLimitReached)
    Serial.print(F("Memory Limit Reached. Logging stopped. Use command cler to clear loglist!"));

  Serial.println();

  LogEntry *tempEntry = logListStack;
  logListStack = newEntry;
  logListStack->next = tempEntry;
}

void SetBaseTime()
{
  byte tempStorage;

  Serial.print(F("month:"));
  while (!Serial.available())
  {
    delay(10);
  }
  tempStorage = getUserInput().toInt();
  if (tempStorage == 0 || tempStorage > 12)
  {
    Serial.println();
    Serial.print(F("invalid month."));
    return;
  }
  baseValues.month = tempStorage;

  Serial.print(F("day:"));
  while (!Serial.available())
  {
    delay(10);
  }
  tempStorage = getUserInput().toInt();
  if (tempStorage == 0 || tempStorage > monthDays[baseValues.month - 1])
  {
    Serial.println();
    Serial.print(F("invalid day. Max Day:"));
    Serial.print(String(monthDays[baseValues.month - 1]));
    return;
  }
  baseValues.day = tempStorage;

  Serial.print(F("hour:"));
  while (!Serial.available())
  {
    delay(10);
  }
  tempStorage = getUserInput().toInt();
  if (tempStorage > 23)
  {
    Serial.println();
    Serial.print(F("invalid hour. Max Hour:23"));
    return;
  }
  baseValues.hour = tempStorage;

  Serial.print(F("Minute:"));
  while (!Serial.available())
  {
    delay(10);
  }
  tempStorage = getUserInput().toInt();
  if (tempStorage > 59)
  {
    Serial.println();
    Serial.print(F("invalid hour. Max Hour:59"));
    return;
  }
  baseValues.minute = tempStorage;
  Serial.println();
  Serial.print(F("new base-time is set."));
  Serial.println();
}

void clearLog()
{
  Serial.println();
  memoryLimitReached = false;
  LogEntry *currentEntry = logListStack;
  LogEntry *nextEntry;
  while (currentEntry != NULL)
  {
    nextEntry = currentEntry->next;
    delete currentEntry;
    currentEntry = NULL;
    currentEntry = nextEntry;
  }
  logListStack = NULL;
  Serial.print(F("Log cleared."));
  Serial.println();
}
