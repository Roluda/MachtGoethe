const int inputs[] = {A0, A1, A2, A3, A4};
const int outputs[] = {3, 5, 6, 9, 10};

#define BoxRelais A7

int measures = 1000;
unsigned long measureInterval = 2;
int highs[5];
float goals[] = {0.1, 0.3, 0.5, 0.7, 0.9};
float measureTolerance = 0.05;
bool isOpen = false;

void setup() {
  pinMode(BoxRelais, OUTPUT);

  Serial.begin(9600);
  for (int i = 0; i < 5; i++) {
    pinMode(outputs[i], OUTPUT);
    pinMode(inputs[i], INPUT);
  }
  StartWrite();
}

void loop() {
  delay(1000);
  ResetMeasure();
  for (int i = 0; i < measures; i++) {
    delay(measureInterval);
    for (int i = 0; i < 5; i++) {
      if (digitalRead(inputs[i]) == HIGH) {
        highs[i]++;
      }
    }
  }
  CompareRatios();
}

void StartWrite() {
  for (int i = 0; i < 5; i++) {
    analogWrite(outputs[i], 255 * goals[i]);
  }
}

void ResetMeasure() {
  for (int i = 0; i < 5; i++) {
    highs[i] = 0;
  }
}

void CompareRatios() {
  Serial.println("");
  Serial.println("MEASURED RATIOS:");
  Serial.println("");
  int correctValues = 0;
  for (int i = 0; i < 5; i++) {
    float ratio = (float)highs[i] / measures;
    Serial.println("");
    Serial.println("INPUT_" + String(i) + ": " + String(ratio));
    Serial.println("GOAL: " + String(goals[i]));
    float upperBound = goals[i] + measureTolerance;
    float lowerBound = goals[i] - measureTolerance;
    if (ratio <= upperBound && ratio >= lowerBound) {
      correctValues++;
      Serial.println("--> CORRECT");
    } else {
      Serial.println("--> INCORRECT");
    }
  }
  if (correctValues == 5) {
    Serial.println("");
    Serial.println("ALL GOALS MET");
    if (!isOpen) {
      isOpen = true;
      OpenBox();
    }
  } else {
    isOpen = false;
  }
}

void OpenBox() {
  digitalWrite(BoxRelais, HIGH);
  delay(5000);
  digitalWrite(BoxRelais, LOW);
}

