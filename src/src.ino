#include <Arduino.h>
char InputChar;
bool loggedActivity;
bool overflow;  // end of Stream
bool movementActive;
bool currentmovementActive;
bool MemoryLimitReached;
byte Streamlenght;
String Command;
const byte MovementSensorPin = 53;
const unsigned int MovementActivityTime = 5000;
const unsigned int FreeMemoryLimit = 1000;
unsigned long timeTill;
unsigned long counter;
unsigned long timeCalculation;
byte monthdays[12];

struct LogEntry {
  byte day;
  byte month;
  // byte year;
  byte hour;
  byte minute;
  byte second;
  LogEntry *next;  // linked list pointer
};

LogEntry *logListStack = NULL;  // Head of the linked list
LogEntry BaseValues;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  loggedActivity = false;
  Streamlenght = 0;
  overflow = false;
  movementActive = false;
  pinMode(MovementSensorPin, INPUT);
  // default values
  monthdays[0] = 31;  // jan
  monthdays[1] = 29;
  monthdays[2] = 31;
  monthdays[3] = 30;  // april
  monthdays[4] = 31;
  monthdays[5] = 30;
  monthdays[6] = 31;  // jul
  monthdays[7] = 31;
  monthdays[8] = 30;
  monthdays[9] = 31;  // okt
  monthdays[10] = 30;
  monthdays[11] = 31;
  BaseValues.day = 9;
  BaseValues.month = 3;
  BaseValues.hour = 22;
  BaseValues.minute = 50;
  BaseValues.month = 3;
  BaseValues.day = 9;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!loggedActivity) {
    Serial.print(F("Waiting"));
    Serial.println();
    loggedActivity = true;
  }

  currentmovementActive = digitalRead(MovementSensorPin) == HIGH;
  if (movementActive || currentmovementActive) {       // movement sensor activity
    if ((!movementActive) && currentmovementActive) {  // first contact
      Serial.println();
      Serial.print("Movement detected");
      timeTill = millis() + MovementActivityTime;
      movementActive = true;
      counter = 0;
      if
        (!MemoryLimitReached)
        CreateTimeStamp();
    } else if (movementActive && currentmovementActive) {  // recurring contact (first contact not finished yet)
      timeTill = millis() + MovementActivityTime;
      Serial.println("debug: still movement " + String(counter));
      counter += 1;
    } else if (movementActive && (!currentmovementActive)) {  // check if contact finished
      Serial.println("debug: no movement " + String(counter));
      counter += 1;
      movementActive = (timeTill < millis());
      if (!movementActive) {  // contact finished
        Serial.println();
        Serial.println("Movement stopped");
      }
    }
  }

  while (Serial.available() != 0) {
    Streamlenght += 1;
    InputChar = Serial.read();
    delay(100);

    if (overflow) {
      Serial.print(InputChar);
    } else {
      overflow = (Streamlenght > 4);
      if (!overflow) {
        Command += InputChar;
      } else {
        Serial.print(F("Input above 4:"));
        Serial.print(InputChar);
      }
    }
  }

  if (Streamlenght != 0) {
    Streamlenght = 0;
    loggedActivity = false;
    overflow = false;
    Serial.println();
    Serial.print("Command:");
    Command.toUpperCase();
    Serial.print(Command);
    Interpret(Command);
    //if (Command == "list")
    //  PrintLog();
    Command = "";
    Serial.println();
  }
}
void PrintLog() {
  Serial.println("Logged Entries:");
  LogEntry *currentEntry = logListStack;
  while (currentEntry != NULL) {
    PrintLogEntry(currentEntry);
    currentEntry = currentEntry->next;
  }
  Serial.println();
}

void PrintLogEntry(LogEntry *printEntry) {
  Serial.println();
  Serial.print(printEntry->day);
  Serial.print("/");
  Serial.print(printEntry->month);
  Serial.print("/");
  Serial.print(printEntry->hour);
  Serial.print(":");
  Serial.print(printEntry->minute);
  Serial.print(":");
  Serial.println(printEntry->second);
}

void CreateTimeStamp() {
  byte tempMonth;
  byte tempDaysInMonth;

  LogEntry *newEntry = new LogEntry;
  timeCalculation = millis() / 1000;  // millisends -> seconds
  newEntry->second = timeCalculation % 60;
  timeCalculation /= 60;  // get seconds
  timeCalculation += BaseValues.minute;
  newEntry->minute = timeCalculation % 60;
  timeCalculation /= 60;  // get minutes
  timeCalculation += BaseValues.hour;
  newEntry->hour = timeCalculation % 24;
  timeCalculation /= 24;  // get days
  timeCalculation += BaseValues.day;
  tempMonth = BaseValues.month;  // currentmonth
  tempDaysInMonth = monthdays[tempMonth - 1];
  while (tempDaysInMonth < timeCalculation) {
    timeCalculation -= tempDaysInMonth;
    tempMonth++;
    tempDaysInMonth = monthdays[tempMonth - 1];
  }
  newEntry->day = timeCalculation;
  newEntry->month = tempMonth;

  PrintLogEntry(newEntry);

  Serial.print("Free Memory:");
  int freeMemory = getFreeMemory();
  Serial.print(String(freeMemory));
  Serial.println();
  MemoryLimitReached = freeMemory < FreeMemoryLimit;
  if (MemoryLimitReached)
    Serial.print("Memory Limit Reached. Logging stopped. Use command clr to clear loglist!");

  Serial.println();

  LogEntry *tempEntry = logListStack;
  logListStack = newEntry;
  logListStack->next = tempEntry;
}

void Interpret(String UserCommand) {
  Serial.print(UserCommand);
  Serial.println();
    if (UserCommand == "CLER") {
    clearLog();
    return;
  }
  
  if (UserCommand == "HELP" || UserCommand == "?") {
    Serial.print("?,help = get Commands");
    Serial.println();
    Serial.print("clr = reset log");
    Serial.println();
    Serial.print("get,log,list = get log");
    Serial.println();
    Serial.print("set = start set time sequence");
    Serial.println();
    return;
  }
  if (UserCommand == "GET" || UserCommand == "LOG" || UserCommand == "LIST") {
    PrintLog();
    return;
  }

  if (UserCommand == "SET") {
    return;
  }
  // switch (Command) {
  //   case "HELP":
  //     Serial.print("?,help = get Commands");
  //     Serial.println();
  //     Serial.print("clr = reset log");
  //     Serial.println();
  //     Serial.print("get,log,list = get log");
  //     Serial.println();
  //     Serial.print("set = start set time sequence");
  //   default:
  //     Serial.println();

  // }
  Serial.print("input not valid. Write help to get valid inputs");
}

void clearLog() {
  Serial.print(String(getFreeMemory()));
  Serial.println();
  MemoryLimitReached = false;
  LogEntry *currentEntry = logListStack;
  LogEntry *nextEntry;
    while (currentEntry != NULL) {
     nextEntry = currentEntry->next;
     currentEntry = NULL;
     currentEntry = nextEntry;
  }
  Serial.print(String(getFreeMemory()));
  Serial.println();
}

// Snippet below from web
// Original: https://forum.arduino.cc/t/how-to-create-and-free-dynamic-arrays-with-arduino/934662
// Formatted: https://forum.arduino.cc/t/how-to-create-and-free-dynamic-arrays-with-arduino/934662/12
extern unsigned int __bss_end;
extern void *__brkval;

int getFreeMemory() {
  int free_memory;
  if ((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
  return free_memory;
}
