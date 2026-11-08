// Gimbal Solar Tracker - by Luke Jiang
// Copyright (C) 2026 Luke Jiang. All Rights Reserved.
// final version

constexpr int ldrA = 1; // ldrtop
constexpr int ldrB = 2; // ldrbottom
constexpr int ldrC = 3; // ldrleft
constexpr int ldrD = 0; // ldrright

int top = 0;
int bottom = 0;
int left = 0;
int right = 0;
int night = 0;
int limitUD = 0;
int limitLR = 0;

constexpr int threshold_value = 25;

void setup() {
  // put your setup code here, to run once:
  delay(2000);
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

void moveUp() {
  digitalWrite(11, HIGH);
  digitalWrite(10, LOW);
}

void moveDown() {
  digitalWrite(11, LOW);
  digitalWrite(10, HIGH);
}

void moveCCW() {
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
}

void moveCW() {
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  wormgear();
}

void initialize() {
  Serial.println("Initialization begin!");
  stopHorizontal();
  while (digitalRead(8) == HIGH) { // while limit sensor has NOT been reached
    moveUp();
  }
  Serial.println("Limit sensor has been reached!");

  stopVertical();
  delay(100);

  Serial.print("Pin 8 reading: ");
  Serial.println(digitalRead(8));
  while (digitalRead(8) == LOW) { // while limit sensor is triggered
    Serial.println("Reversing motor direction...");
    moveDown();
  }
  stopVertical(); // stop vertical motor
  while (digitalRead(7) == HIGH) {
    moveCCW();
  }
  stopHorizontal();
  unsigned long start = millis();
  while (millis() - start < 15000) {
    moveCW();
  }
  stopHorizontal();
  // top = analogRead(ldrA); // right
  // bottom = analogRead(ldrB); // left
  // left = analogRead(ldrC); // bottom
  // right = analogRead(ldrD); // top
  top = analogRead(ldrD);
  bottom = analogRead(ldrC);
  left = analogRead(ldrB);
  right = analogRead(ldrA);
  delay(2000);
  if (bottom - top >= 100) {
    
    Serial.println("Threshold met!");
    unsigned long start1 = millis();
    while (millis() - start1 < 16000) {
      Serial.println("Moving clockwise!");
      moveCW();
    }
    stopHorizontal();
  }
  unsigned long start2 = millis();
  while (millis() - start2 < 5000) {
    Serial.println("Moving down...");
    moveDown();
  }
  stopVertical();
  Serial.println("Initialization STOP!");
  delay(2000);
  Serial.println("GO!");
}

void wormgear() {

  // top = analogRead(ldrA); // right
  // bottom = analogRead(ldrB); // left
  // left = analogRead(ldrC); // bottom
  // right = analogRead(ldrD); // top
  top = analogRead(ldrD);
  bottom = analogRead(ldrC);
  left = analogRead(ldrB);
  right = analogRead(ldrA);

  
  int diffelev = top - bottom;
  int diffazi = right - left;


  // normal operating conditions
  if (diffelev >= threshold_value && digitalRead(8) == HIGH) { // ensures limit sensor has not been activated
    // moves solar panel up
    moveUp();
  } else if (abs(diffelev) >= threshold_value && diffelev < 0 && digitalRead(9) == HIGH) {
    // moves solar panel down
    moveDown();
  } else {
    stopVertical(); // sends LOW signal to both pins 10 and 11
  }



  if (diffazi >= threshold_value && digitalRead(7) == HIGH) {
    moveCW();
  } else if (abs(diffazi) >= threshold_value && diffazi < 0 && digitalRead(6) == HIGH) {
    moveCCW();
  } else {
    stopHorizontal();
  }

  if (digitalRead(7) == LOW) {
    unsigned long start_time = millis();
    while (digitalRead(7) == LOW) {
      Serial.println("Reversing motor direction...");
      while (millis() - start_time < 4000) {
        moveCW();
      }
        Serial.println("Motor STOP!");
        stopHorizontal();
        delay(30000);
    }
  }

  if (digitalRead(6) == LOW) {
    unsigned long starting = millis();
    while (digitalRead(6) == LOW) {
      Serial.println("Reversing motor direction...");
      while (millis() - starting < 4000) {
        moveCCW();
      }
        Serial.println("Motor STOP!");
        stopHorizontal();
        delay(30000);
    }
  }
  
  while (digitalRead(8) == LOW) {
    Serial.println("Limit sensor reached!");
    moveDown();
    if (digitalRead(8) == HIGH) {
      Serial.println("Motor STOP!");
      stopVertical();
      delay(100);
      unsigned long timeBegin = millis();
      while (millis() - timeBegin < 17000) {
        moveCW();
      }
      stopHorizontal();
      delay(10000);
    }
  }

  while (digitalRead(9) == LOW) {
    Serial.println("Limit sensor reached!");
    moveUp();
    if (digitalRead(9) == HIGH) {
      Serial.println("Motor STOP!");
      stopVertical();
      delay(10000);
    }
  }
}
