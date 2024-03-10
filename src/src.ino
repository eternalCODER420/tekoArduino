#include <Arduino.h>

const byte MovementSensorPin = 53;
const unsigned int MovementActivityTime = 5000;
const unsigned int FreeMemoryLimit = 1000;

bool movementActive;
bool currentMovementActive;
bool memoryLimitReached;

unsigned long timeTill;
unsigned long counter;

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
  movementActive = false;
  pinMode(MovementSensorPin, INPUT);
  // default values
  // monthDays[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // Days in each month
  baseValues.day = 10;
  baseValues.month = 3;
  baseValues.hour = 15;
  baseValues.minute = 5;
}

void loop()
{
  // put your main code here, to run repeatedly:
  // if (!loggedActivity) {
  //   Serial.print(F("Waiting"));
  //   Serial.println();
  // }

  currentMovementActive = digitalRead(MovementSensorPin) == HIGH;
  if (movementActive || currentMovementActive)
  { // movement sensor activity
    if ((!movementActive) && currentMovementActive)
    { // first contact
      Serial.println();
      Serial.print("Movement detected");
      timeTill = millis() + MovementActivityTime;
      movementActive = true;
      // counter = 0;
      if (!memoryLimitReached)
        createTimeStamp();
    }
    else if (movementActive && currentMovementActive)
    { // recurring contact (first contact not finished yet)
      timeTill = millis() + MovementActivityTime;
      // Serial.println("debug: still movement " + String(counter));
      // counter += 1;
    }
    else if (movementActive && (!currentMovementActive))
    { // check if contact finished
      // Serial.println("debug: no movement " + String(counter));
      // counter += 1;
      movementActive = (timeTill < millis());
      if (!movementActive)
      { // contact finished
        Serial.println();
        Serial.println("Movement stopped");
      }
    }
  }

  if (Serial.available() != 0)
  {
    Interpret(getUserInput());
  }
}
void PrintLog()
{
  Serial.println("Logged Entries:");
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
  Serial.print("/");
  Serial.print(printEntry->month);
  Serial.print("/-");
  Serial.print(printEntry->hour);
  Serial.print(":");
  Serial.print(printEntry->minute);
  Serial.print(":");
  Serial.println(printEntry->second);
}

void createTimeStamp()
{
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

  Serial.print("Free Memory:");
  int freeMemory = getFreeMemory();
  Serial.print(String(freeMemory));
  Serial.println();
  memoryLimitReached = freeMemory < FreeMemoryLimit;
  if (memoryLimitReached)
    Serial.print("Memory Limit Reached. Logging stopped. Use command cler to clear loglist!");

  Serial.println();

  LogEntry *tempEntry = logListStack;
  logListStack = newEntry;
  logListStack->next = tempEntry;
}

void Interpret(String UserCommand)
{
  if (UserCommand == "")
  {
    Serial.print("No Input.");
    Serial.println();
  }

  Serial.println();
  Serial.print("Command:");
  UserCommand.toUpperCase();
  UserCommand.trim();
  Serial.print(UserCommand);
  Serial.println();

  if (UserCommand == "CLER")
  {
    clearLog();
    return;
  }

  if (UserCommand == "HELP")
  {
    Serial.print("?,help = get Commands");
    Serial.println();
    Serial.print("cler = reset log");
    Serial.println();
    Serial.print("get,log,list = get log");
    Serial.println();
    Serial.print("set = start set time sequence");
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
    byte tempStorage;

    Serial.print("month:");
    while (!Serial.available())
    {
      delay(10);
    }
    tempStorage = getUserInput().toInt();
    if (tempStorage == 0 || tempStorage > 12)
    {
      Serial.println();
      Serial.print("invalid month.");
      return;
    }
    baseValues.month = tempStorage;

    Serial.print("day:");
    while (!Serial.available())
    {
      delay(10);
    }
    tempStorage = getUserInput().toInt();
    if (tempStorage == 0 || tempStorage > monthDays[baseValues.month - 1])
    {
      Serial.println();
      Serial.print("invalid day. Max Day:");
      Serial.print(String(monthDays[baseValues.month - 1]));
      return;
    }
    baseValues.day = tempStorage;

    Serial.print("hour:");
    while (!Serial.available())
    {
      delay(10);
    }
    tempStorage = getUserInput().toInt();
    if (tempStorage > 23)
    {
      Serial.println();
      Serial.print("invalid hour. Max Hour:23");
      return;
    }
    baseValues.hour = tempStorage;

    Serial.print("Minute:");
    while (!Serial.available())
    {
      delay(10);
    }
    tempStorage = getUserInput().toInt();
    if (tempStorage > 59)
    {
      Serial.println();
      Serial.print("invalid hour. Max Hour:59");
      return;
    }
    baseValues.minute = tempStorage;
    Serial.println();
    Serial.print("new base-time is set.");
    Serial.println();

    return;
  }
  Serial.print("input not valid. Write help to get valid inputs");
}

void clearLog()
{
  Serial.print(String(getFreeMemory()));
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
  Serial.print(String(getFreeMemory()));
  Serial.println();
}

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

// Snippet below from web
// Original: https://forum.arduino.cc/t/how-to-create-and-free-dynamic-arrays-with-arduino/934662
// Formatted: https://forum.arduino.cc/t/how-to-create-and-free-dynamic-arrays-with-arduino/934662/12
extern unsigned int __bss_end;
extern void *__brkval;

int getFreeMemory()
{
  int free_memory;
  if ((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
  return free_memory;
}
