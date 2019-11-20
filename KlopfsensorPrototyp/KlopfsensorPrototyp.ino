#define PIEZO A0

float knock_th = 200;
int currentKnock = 0;
int pattern[] = {0,1,1,0,2,2,0,1,1,0,2,2,0,1,1,3}; //0 = any delta to last; 1= short delta to last, 2= long delta to last , 3=end
unsigned long shortKnock = 0;
unsigned long longKnock =0;
unsigned long lastKnock = 0;
unsigned long knockDelta = 0;
float toleranceFactor = 0.3;
float shortToLongFactor = 2;

unsigned long maximumIdleTime = 4000;

void setup() {
  Serial.begin(9600);
  pinMode(PIEZO, INPUT);
}

void loop() {
  if(currentKnock!=0){
    if(millis()-lastKnock > maximumIdleTime){
      Reset();
    }
  }
  if(CheckKnock()){
    switch(pattern[currentKnock]){
      case 0:
        lastKnock=millis();
        Progress();
        break;
      case 1:
        knockDelta=millis()-lastKnock;
        lastKnock=millis();
        if(shortKnock==0){
          shortKnock = knockDelta;
        }
        if(InPattern(shortKnock, knockDelta)&&DifferentiableKnocks()){
          Progress();
        }else{
          Reset();
        }
        break;
      case 2:
        knockDelta=millis()-lastKnock;
        lastKnock=millis();
        if(longKnock==0){
          longKnock = knockDelta;
        }
        if(InPattern(longKnock, knockDelta)&&DifferentiableKnocks()){
          Progress();
        }else{
          Reset();
        }
        break;
    }
  }
}

bool CheckKnock(){
  float knockValue=analogRead(PIEZO);
  if(knockValue >= knock_th){
    Serial.println("KNOCK: " + String(currentKnock));
    delay(50);
    return true;
  }else{
    return false;
  }
}

void Progress(){
  currentKnock++;
  switch(pattern[currentKnock]){
    case 0:
      LetterComplete();
      break;
    case 3:
      Open();
      break;
  }
}

void LetterComplete(){
  Serial.println("Letter Done");
}

void Open(){
  Serial.println("YOU FKIN DID IT");
  Reset();
}

void Reset(){
  Serial.println("RESET");
  shortKnock=0;
  longKnock=0;
  knockDelta =0;
  currentKnock =0;
}

bool DifferentiableKnocks(){
  if(shortKnock>0&&longKnock>0){
    if(shortKnock*shortToLongFactor>longKnock){
      return false;
    }
  }
  return true;
}

bool InPattern(unsigned long expectedTime, unsigned long actualTime){
  float upperBound = expectedTime*(1+toleranceFactor);
  float lowerBound = expectedTime*(1-toleranceFactor);
  if(actualTime<=upperBound && actualTime>=lowerBound){
    //Serial.println("PatternHit");
    return true;
  }else{
    //Serial.println("PatternMissed");
    return false;
  }
}

