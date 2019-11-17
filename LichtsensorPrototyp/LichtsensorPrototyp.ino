
#define PHOTOSENSOR_GREEN A1
#define PHOTOSENSOR_RED A0
#define PHOTOSENSOR_BLUE A2
#define LEDPIN 13

int sensorValueGreen= 0;
int sensorValueRed = 0;
int sensorValueBlue = 0;
int greenThreshold = 50;
int redThreshold = 100;
int blueThreshold = 400;

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  sensorValueGreen= analogRead(PHOTOSENSOR_GREEN);
  sensorValueRed= analogRead(PHOTOSENSOR_RED);
  sensorValueBlue= analogRead(PHOTOSENSOR_BLUE);
  bool greenBright = false;
  bool redBright = false;
  bool blueBright = false;
  if(sensorValueGreen<greenThreshold){
    greenBright=true;
    //Serial.println("YELLOW");
  }
  if(sensorValueRed<redThreshold){
    redBright = true;
    //Serial.println("RED");
  }
  if(sensorValueBlue<blueThreshold){
    blueBright = true;
    //Serial.println("BLUE");
  }
  if(greenBright && blueBright && redBright){
    //Serial.println("CONGRATULATIONSSSSS");
    //delay(3000);
  }
  //Serial.println(sensorValueGreen, DEC);
  Serial.println(sensorValueRed, DEC);
  //Serial.println(sensorValueBlue, DEC);
}
