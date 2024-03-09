char InputChar;
bool loggedActivity;
bool overflow;  //end of Stream
bool movementActive;
bool currentmovementActive;
byte Streamlenght;
String Command;
byte MovementSensor = 53;
unsigned long timeTill;

struct LogEntry {
  byte day;
  byte month;
  byte year;
  byte hour;
  byte minute;
  byte second;
  LogEntry* next;  // linked list pointer
};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  loggedActivity = false;
  Streamlenght = 0;
  overflow = false;
  movementActive = false;
  pinMode(MovementSensor, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!loggedActivity) {
    Serial.print(F("Waiting"));
    Serial.println();
    loggedActivity = true;
  }

  currentmovementActive = digitalRead(MovementSensor) == HIGH;
  if (movementActive || currentmovementActive) {//movement sensor activity
    if ((!movementActive) && currentmovementActive) {//first contact
      Serial.println();
      Serial.print("Movement detected");
      timeTill = millis() + 5000;
      movementActive = true;
    } else if (movementActive && currentmovementActive) {//recurring contact (first contact not finished yet)
      timeTill = millis() + 5000;
    } else if (movementActive && (!currentmovementActive)) {//check if contact finished
      movementActive = (timeTill < millis());
      if (!movementActive) {//contact finished
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
    Command.toLowerCase();
    Serial.print(Command);
    Command = "";
    Serial.println();
  }
}

void Interpret() {
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
