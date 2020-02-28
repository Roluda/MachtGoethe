// Connect: S pin to analog, middle pin to 5v, - pin to gnd

#define PHOTOSENSOR_GREEN A2
#define PHOTOSENSOR_RED A3
#define PHOTOSENSOR_BLUE A4
#define SUMMER 2
#define BOX 10

int sensorValueGreen = 0;
int sensorValueRed = 0;
int sensorValueBlue = 0;
int greenThreshold = 225;
int redThreshold = 175;
int blueThreshold = 190;

bool solved = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(SUMMER, OUTPUT);
  pinMode(BOX, OUTPUT);
  digitalWrite(BOX, HIGH);
  Serial.begin(9600);
}

void loop() {
  delay(1000);
  if (solved) {
    solved = false;
  }
  // put your main code here, to run repeatedly:
  sensorValueGreen = analogRead(PHOTOSENSOR_GREEN);
  sensorValueRed = analogRead(PHOTOSENSOR_RED);
  sensorValueBlue = analogRead(PHOTOSENSOR_BLUE);
  bool greenBright = false;
  bool redBright = false;
  bool blueBright = false;
  int correctColors = 0;
  if (sensorValueGreen < greenThreshold) {
    correctColors ++;
    greenBright = true;
    //Serial.println("YELLOW");
  }
  if (sensorValueRed < redThreshold) {
    correctColors ++;
    redBright = true;
    //Serial.println("RED");
  }
  if (sensorValueBlue < blueThreshold) {
    correctColors ++;
    blueBright = true;
    //Serial.println("BLUE");
  }
  for (int i = 1; i <= correctColors; i++) {
    tone(SUMMER, 220 << i, 100);
    delay(200);
  }
  if (correctColors == 3) {
    OpenBox();
    solved = true;
    //Serial.println("CONGRATULATIONSSSSS");
    //delay(3000);
  }


  //Serial.println(sensorValueGreen, DEC);
  //Serial.println(sensorValueRed, DEC);
  Serial.println(sensorValueBlue, DEC);
}

void Beep(int milliseconds) {

  digitalWrite(SUMMER, HIGH);
  delay(milliseconds);
  digitalWrite(SUMMER, LOW);
  delay(milliseconds);
}

void OpenBox() {
  digitalWrite(BOX, LOW);
  delay(5000);
  digitalWrite(BOX, HIGH);
}

