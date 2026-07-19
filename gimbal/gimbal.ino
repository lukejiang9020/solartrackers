// Gimbal Solar Tracker - by Luke Jiang
// Copyright (C) 2026 Luke Jiang. All Rights Reserved.
// best working version so far

constexpr int ldrtop = 1;
constexpr int ldrbottom = 2;
constexpr int ldrleft = 3;
constexpr int ldrright = 0;
constexpr int ldrNight = 4;

int top = 0;
int bottom = 0;
int left = 0;
int right = 0;
int night = 0;
int limitUD = 0;
int limitLR = 0;
int isNight = 0;

constexpr int threshold_value = 100;
constexpr int threshold_reset = 76.3;

void setup() {
  // put your setup code here, to run once:
  delay(10000);
  Serial.begin(9600);
  Serial.println("BEGIN");
  pinMode(10, OUTPUT); // horizontal - move right
  pinMode(11, OUTPUT); // horizontal - move left
  pinMode(12, OUTPUT); // vertical - move up
  pinMode(13, OUTPUT); // vertical - move down
  pinMode(9, INPUT); // vertical - 90 deg (topmost)
  pinMode(8, INPUT); // vertical - 0 deg (second top-most)
  pinMode(7, INPUT); // black and yellow wired limit sensor
  pinMode(6, INPUT); // bottom left
  initialize();
}

void stopVertical() {
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}

void stopHorizontal() {
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  wormgear();
}

void initialize() {
  Serial.println("Initialization begin!");
  stopHorizontal();
  while (digitalRead(8) == HIGH) {
    digitalWrite(11, HIGH);
    digitalWrite(10, LOW);
  }
  Serial.println("Limit sensor has been reached!");

  stopVertical();
  delay(100);

  Serial.print("Pin 8 reading: ");
  Serial.println(digitalRead(8));
  unsigned long startTime = millis();
  while (millis() - startTime < 2000) {
    Serial.println("Reversing motor direction...");
    digitalWrite(11, LOW);
    digitalWrite(10, HIGH);
  }
  stopVertical();
  while (digitalRead(7) == HIGH) {
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
  }
  unsigned long start = millis();
  while (millis() - start < 7000) {
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
  }
  stopHorizontal();
  Serial.println("Initialization STOP!");
  delay(5000);
  Serial.println("GO!");
}

void wormgear() {
  top = analogRead(ldrtop);
  bottom = analogRead(ldrbottom);
  left = analogRead(ldrleft);
  right = analogRead(ldrright);
  night = analogRead(ldrNight);

  Serial.print("Top: ");
  Serial.println(top*5.0/1023);
  Serial.print("Bottom: ");
  Serial.println(bottom*5.0/1023);
  Serial.print("Left: ");
  Serial.println(left*5.0/1023);
  Serial.print("Right: ");
  Serial.println(right*5.0/1023);
  // delay(4000);
  // Serial.print("Pin 9 reading: ");
  // Serial.println(digitalRead(8));

  
  int diffelev = top - bottom;
  int diffazi = right - left;


  // normal operating conditions
  if (diffazi >= threshold_value && digitalRead(8) == HIGH) { // ensures limit sensor has not been activated
    // moves solar panel up
    digitalWrite(10, HIGH);
    digitalWrite(11, LOW);
  } else if (abs(diffazi) >= threshold_value && diffazi < 0 && digitalRead(9) == HIGH) {
    // moves solar panel down
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
  } else {
    stopVertical(); // sends LOW signal to both pins 10 and 11
  }



  if (diffelev >= threshold_value && digitalRead(7) == HIGH) {
    // digitalWrite(10, LOW);
    // digitalWrite(11, LOW);
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
  } else if (abs(diffelev) >= threshold_value && diffelev < 0 && digitalRead(6) == HIGH) {
    // digitalWrite(10, LOW);
    // digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
  } else {
    stopHorizontal();
  }

  while (digitalRead(7) == LOW) {
    Serial.println("Limit sensor reached!");
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
    if (digitalRead(7) == HIGH) {
      Serial.println("Motor STOP!");
      stopHorizontal();
      delay(30000);
    }
  }

  while (digitalRead(6) == LOW) {
    Serial.println("Limit sensor reached!");
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    if (digitalRead(7) == HIGH) {
      Serial.println("Motor STOP!");
      stopHorizontal();
      delay(30000);
    }
  }

  
  while (digitalRead(8) == LOW && !isNight) {
    Serial.println("Limit sensor reached!");
    digitalWrite(10, HIGH);
    digitalWrite(11, LOW);
    if (digitalRead(8) == HIGH) {
      Serial.println("Motor STOP!");
      stopVertical();
      delay(100);
      unsigned long timeBegin = millis();
      while (millis() - timeBegin < 17000) {
        digitalWrite(12, HIGH);
        digitalWrite(13, LOW);
      }
      stopHorizontal();
      delay(10000);
    }
  }

  while (digitalRead(9) == LOW && !isNight) {
    Serial.println("Limit sensor reached!");
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    if (digitalRead(9) == HIGH) {
      Serial.println("Motor STOP!");
      stopVertical();
      delay(10000);
    }
  }
}
