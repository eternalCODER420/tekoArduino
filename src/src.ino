#include <Arduino.h>
char InputChar;
bool loggedActivity;
bool overflow; // end of Stream
bool movementActive;
bool currentmovementActive;
byte Streamlenght;
String Command;
const byte MovementSensorPin = 53;
unsigned int MovementActivityTime = 5000;
unsigned long timeTill;
unsigned long counter;
unsigned long timeCalculation;
byte day;
byte month;
byte year;
byte hour;
byte minute;
byte second;
byte BaseDay;
byte Basemonth;
// byte Baseyear;
byte Basehour;
byte Baseminute;
byte monthdays[12];
byte tempMonth;
byte tempDaysInMonth;

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

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  loggedActivity = false;
  Streamlenght = 0;
  overflow = false;
  movementActive = false;
  pinMode(MovementSensorPin, INPUT);
  // default values
  monthdays[0] = 31; // jan
  monthdays[1] = 29;
  monthdays[2] = 31;
  monthdays[3] = 30; // april
  monthdays[4] = 31;
  monthdays[5] = 30;
  monthdays[6] = 31; // jul
  monthdays[7] = 31;
  monthdays[8] = 30;
  monthdays[9] = 31; // okt
  monthdays[10] = 30;
  monthdays[11] = 31;
  BaseDay = 9;
  Basemonth = 3;
  // Baseyear = 24;
  Basehour = 22;
  Baseminute = 50;
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!loggedActivity)
  {
    Serial.print(F("Waiting"));
    Serial.println();
    loggedActivity = true;
  }

  currentmovementActive = digitalRead(MovementSensorPin) == HIGH;
  if (movementActive || currentmovementActive)
  { // movement sensor activity
    if ((!movementActive) && currentmovementActive)
    { // first contact
      Serial.println();
      Serial.print("Movement detected");
      timeTill = millis() + MovementActivityTime;
      movementActive = true;
      counter = 0;
      CreateTimeStamp();
    }
    else if (movementActive && currentmovementActive)
    { // recurring contact (first contact not finished yet)
      timeTill = millis() + MovementActivityTime;
      Serial.println("debug: still movement " + String(counter));
      counter += 1;
    }
    else if (movementActive && (!currentmovementActive))
    { // check if contact finished
      Serial.println("debug: no movement " + String(counter));
      counter += 1;
      movementActive = (timeTill < millis());
      if (!movementActive)
      { // contact finished
        Serial.println();
        Serial.println("Movement stopped");
      }
    }
  }

  //   while(digitalRead(MovementSensor) == HIGH)
  //   {
  //     delay(50);
  //   }
  //   Serial.println();
  //   Serial.println("Movement stopped");
  // }

  while (Serial.available() != 0)
  {
    Streamlenght += 1;
    InputChar = Serial.read();
    delay(100);

    if (overflow)
    {
      Serial.print(InputChar);
    }
    else
    {
      overflow = (Streamlenght > 4);
      if (!overflow)
      {
        Command += InputChar;
      }
      else
      {
        Serial.print(F("Input above 4:"));
        Serial.print(InputChar);
      }
    }
  }

  if (Streamlenght != 0)
  {
    Streamlenght = 0;
    loggedActivity = false;
    overflow = false;
    Serial.println();
    Serial.print("Command:");
    Command.toLowerCase();
    Serial.print(Command);
    if (Command == "list")
      PrintLog();
    Command = "";
    Serial.println();
  }
}
void PrintLog()
{
  Serial.println("Logged Entries:");
  LogEntry *currentEntry = logListStack;
  while (currentEntry != NULL)
  {
    Serial.println();
    Serial.print(currentEntry->day);
    Serial.print("/");
    Serial.print(currentEntry->month);
    Serial.print("/");
    // Serial.print(currentEntry->year);
    // Serial.print(" ");
    Serial.print(currentEntry->hour);
    Serial.print(":");
    Serial.print(currentEntry->minute);
    Serial.print(":");
    Serial.println(currentEntry->second);
    currentEntry = currentEntry->next;
  }
  Serial.println();
}

void CreateTimeStamp()
{
  LogEntry *newEntry = new LogEntry;
  timeCalculation = millis() / 1000; // millisends -> seconds
  // timeCalculation += basseconds;
  
  // Serial.print(String(timeCalculation));
  // Serial.println();
  newEntry->second = timeCalculation % 60;
  timeCalculation /= 60; // get seconds
  timeCalculation += Baseminute;
  // Serial.print(String(timeCalculation));
  //   Serial.println();
  newEntry->minute = timeCalculation % 60;
  timeCalculation /= 60; // get minutes
  timeCalculation += Basehour;
  // Serial.print(String(timeCalculation));
  //   Serial.println();
  newEntry->hour = timeCalculation % 24;
  timeCalculation /= 24; // get days
  timeCalculation += BaseDay;
  tempMonth = Basemonth; // currentmonth
  tempDaysInMonth = monthdays[tempMonth - 1];
  while (tempDaysInMonth < timeCalculation)
  {
    timeCalculation -= tempDaysInMonth;
    tempMonth++;
    tempDaysInMonth = monthdays[tempMonth - 1];
  }
  newEntry->day = timeCalculation;
  newEntry->month = tempMonth;


    // Serial.println();
    // Serial.print(newEntry->day);
    // Serial.print("/");
    // Serial.print(newEntry->month);
    // Serial.print("/");
    // // Serial.print(newEntry->year);
    // // Serial.print(" ");
    // Serial.print(newEntry->hour);
    // Serial.print(":");
    // Serial.print(newEntry->minute);
    // // Serial.print(":");
    // // Serial.println(newEntry->second);


  // newEntry->year = baseyear;
  LogEntry *tempEntry = logListStack;
  logListStack = newEntry;
  logListStack->next = tempEntry;

  // newEntry->next = NULL;
}


void Interpret()
{
  // switch (Command) {
  // case "HELP":
  //   Serial.print("?,help = get Commands");
  //   Serial.println();
  //   Serial.print("clr = reset log");
  //   Serial.println();
  //   Serial.print("get,log = get log");
  //   Serial.println();
  //   Serial.print("set = start set time sequence");
  // default:
  //   Serial.println();
  //   Serial.print("input not valid. Write help to get valid inputs");
}
