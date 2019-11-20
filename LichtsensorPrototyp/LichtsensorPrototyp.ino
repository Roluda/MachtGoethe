
#define PHOTOSENSOR_GREEN A3
#define PHOTOSENSOR_RED A4
#define PHOTOSENSOR_BLUE A5
#define SUMMER 2
#define BoxRelais A0
#define DoneInput A1
#define BigRelais A2

int sensorValueGreen = 0;
int sensorValueRed = 0;
int sensorValueBlue = 0;
int greenThreshold = 50;
int redThreshold = 100;
int blueThreshold = 400;

bool solved = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(SUMMER, OUTPUT);
  pinMode(BoxRelais, OUTPUT);
  pinMode(BigRelais, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (solved && digitalRead(DoneInput)) {
    OpenBigMagnet();
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
  for (int i = 0; i < correctColors; i++) {
    Beep(200);
  }
  if (correctColors == 3) {
    delay(1000);
    OpenBox();
    solved = true;
    //Serial.println("CONGRATULATIONSSSSS");
    //delay(3000);
  }


  //Serial.println(sensorValueGreen, DEC);
  //Serial.println(sensorValueRed, DEC);
  //Serial.println(sensorValueBlue, DEC);
}

void Beep(int milliseconds) {
  digitalWrite(SUMMER, HIGH);
  delay(milliseconds);
  digitalWrite(SUMMER, LOW);
  delay(milliseconds);
}

void OpenBox() {
  digitalWrite(BoxRelais, HIGH);
  delay(5000);
  digitalWrite(BoxRelais, LOW);
}

void OpenBigMagnet() {
  digitalWrite(BigRelais, HIGH);
  delay(1000);
  digitalWrite(BigRelais, LOW);
}

