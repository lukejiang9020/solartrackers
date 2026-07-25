// Servo motor solar tracker
// Copyright (C) 2026 Luke Jiang
#include <Servo.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
//Initialize variables    

constexpr int ldrtopright = 0;
constexpr int ldrtopleft = 1;
constexpr int ldrbottomright = 2;
constexpr int ldrbottomleft = 3;


int topleft = 0;
int bottomleft = 0;
int topright = 0;
int bottomright = 0;
int limitUD = 0;
int limitLR = 0;

//Declare two servos
Servo servo_updown;
Servo servo_rightleft;

constexpr int threshold_value = 50;
constexpr float threshold_reset = 76.3;           //measurement sensitivity

// constexpr int lightPin = 2;

// volatile bool wakeFlag = false;

void setup()
{
  Serial.begin(9600);                                //serial connection setup  //opens  serial port, sets data rate to 9600 bps
  Serial.println("CLEARDATA");                       //clear  all data that’s been place in already
  Serial.println("LABEL,t,voltage,current,power,Mode");   //define the column headings (PLX-DAQ command)
  

  servo_updown.attach(6);             //Servo motor up-down movement
  servo_rightleft.attach(7); 
  initialize();    //Servo motor right-left movement
}

void loop()
{
//  pv_power();

    float volt = analogRead(A5)*5.0/1023;
    float voltage = 2*volt;                //  Volt=(R1/R1+R2)*Voltage / R1=R2=10Ohms  => voltage=2*volt)
    float current = voltage/20;            //  I=voltage/(R1+R2) 
    float power  = voltage*current;
    if (volt == 0) {
      Serial.println("Please connect solar panel on pin A5!");
    } else {
      Serial.print("DATA,TIME,"); // PLX-DAQ command
      Serial.print(voltage);    //send the voltage to serial port
      Serial.print(",");
      Serial.print(current);    //send the current to serial port
      Serial.print(",");
      Serial.print(power);  //send the power to serial port
      Serial.print(",");
    }
    automaticsolartracker();
    // testMotor();
}

void initialize() {
  if (servo_rightleft.read() < 90) {
    for (int pos = servo_rightleft.read(); pos < 90; pos++) {
      servo_rightleft.write(pos);
      delay(30);
    }
  } else {
    for (int pos = servo_rightleft.read(); pos > 90; pos--) {
      servo_rightleft.write(pos);
      delay(30);
    }
  }

  if (servo_updown.read() < 90) {
    for (int pos = servo_updown.read(); pos < 90; pos++) {
      servo_updown.write(pos);
      delay(30);
    }
  } else {
    for (int pos = servo_updown.read(); pos > 90; pos--) {
      servo_updown.write(pos);
      delay(30);
    }
  }
}

void automaticsolartracker() {

     topleft = analogRead(ldrtopleft);
     bottomright = analogRead(ldrbottomright);
     topright = analogRead(ldrtopright);
     bottomleft = analogRead(ldrbottomleft);
     // calculating average

     // calculating average
     int avgtop = (topright + topleft) / 2;     //average  of top LDRs
     int avgbot = (bottomright + bottomleft) / 2;     //average of bottom LDRs
     int avgleft = (topleft + bottomleft) / 2;    //average of left LDRs
     int avgright  = (topright + bottomright) / 2;   //average of right LDRs

     int diffelev = avgtop - avgbot;
     int diffazi = avgright - avgleft;
     
     
      if (diffazi >= threshold_value && servo_rightleft.read() <= 180){
          servo_rightleft.write((servo_rightleft.read() + 1.0));
      }
      if (abs(diffazi) >= threshold_value && diffazi < 0 && servo_rightleft.read() > 0) { 
          servo_rightleft.write((servo_rightleft.read() - 1.0));
      }
             
      //up-down movement of solar tracker

      if (diffelev >= threshold_value && servo_updown.read() <= 150) { 
          servo_updown.write((servo_updown.read() + 1.0));
      }
      if (abs(diffelev) >= threshold_value && diffelev < 0 && servo_updown.read() > 30) {
          servo_updown.write((servo_updown.read() - 1.0));
      }
           
 }
