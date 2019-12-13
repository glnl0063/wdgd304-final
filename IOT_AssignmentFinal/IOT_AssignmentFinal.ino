/*
  ----- VERSION 13 -----
  Sensors: DS1307 Real Time Clock, Light Detecting Resistor, Buzzer
  Libraries: Uses TimeLib Library, DS1307RTC Library, Wire Library for 12C
 
*/

// include required libraries
// library for I2C for RTC sensor
#include <Wire.h>

// library for time
#include <TimeLib.h>

// library for RTC sensor
#include <DS1307RTC.h>

// define a constant for RTC I2C Address
#define DS1307_CTRL_ID 0x68

// define a constant for light sensor
#define photoCellPin 0

// define a constant for buzzer sensor
#define buzzer 4

// variables to count square wave pulses
int ticks = 0;
int prevTickValue = 0;

// variable for LDR
int photoCellReading;

// variable for user's input
int userTime; 

// variable for medicationTracking
boolean medicationTaken = false; 

void setup() {

    // send debugging information via the Serial monitor  
    Serial.begin(9600);
    
    // attach interrupt to pin D2
    attachInterrupt(0,handleInt,FALLING);
  
    // set square wve at 1Hz
    setSQW(0x10);
  
    // state that buzzer is an output
    pinMode(buzzer,OUTPUT);  
    
}

void loop() {

    // see if serial data is being received
    if (Serial.available() > 0) {
    
      // created a new string variable to receive serial data
      // userTime = 0;
    
      // loop through received data and append to the variable
      while (Serial.available() > 0) {   

        // pass data as integer
        userTime += Serial.parseInt();      

      }
    
      // print received serial data
      Serial.println("Usertime: " + userTime);
  
    }
    
    // read LDR sensor every 3 seconds    
    if (ticks == 3){
      
      // get readings from LDR & assign it to variable 
      photoCellReading = analogRead(photoCellPin);  
    
      // reset the tick to zero
      ticks = 0;
    
    }
    
    // update serial monitor display if a second has elapsed  
    if (ticks != prevTickValue) {

      // assign tick to previous value
      prevTickValue = ticks;

      // pass in the reading for LDR into the function
      printCurrentTime(photoCellReading);
    }

}

// interrupt the handler
void handleInt() {

    // increase ticks by 1
    ticks++;
  
}

// setup RTC Module for 1 Hz square wave
void setSQW(uint8_t value) {
    Wire.beginTransmission(DS1307_CTRL_ID);
    Wire.write(7);
    Wire.write(value);
    Wire.endTransmission();
}


// print to the serial monitor
void printCurrentTime(int photoCellReading){

  // formats the timestamp into time elements like hours and mins
  tmElements_t tm;

  // if the real time clock can read time
  if (RTC.read(tm)) {

    // display time & date
    int currentHour = tm.Hour; 
    int currentMinute = tm.Minute;
    int curentSecond = tm.Second;  
    
    // if it's midnight
    if (medicationTaken && currentHour == 0  && currentMinute == 0) {
      
        medicationTaken = false; 
        digitalWrite(buzzer, LOW);             
        
        // test: display message
        // Serial.print(" New Day started!");           

    // user didn't taken their med, the box is closed and it's past the scheduled time
    } else if (!medicationTaken && photoCellReading <= 500 && currentHour >= userTime) {
        
        // medicationTaken = false; 
        digitalWrite(buzzer, HIGH);   

        // test: display message           
        // Serial.print(" Medication not taken");    

    // user are taking their med, the box is open and it's past the scheduled time      
    } else if (!medicationTaken && photoCellReading > 500 && currentHour >= userTime) {
        
        medicationTaken = true; 
        digitalWrite(buzzer, LOW); 

        // print on serial
        Serial.print("Keep it up!");  

        // test: display message         
        // Serial.print(" User taking medication");   

    // user have taken their med, the box is closed and it's past the scheduled time      
    } else if (medicationTaken && photoCellReading <= 500 && currentHour >= userTime) {

        medicationTaken = true; 
        digitalWrite(buzzer, LOW);      

        // test: display message        
        // Serial.print(" User already took medication");             

    // user have taken their med, the box is open and it's past the scheduled time      
    } else if (medicationTaken && photoCellReading > 500 && currentHour >= userTime) {
        
        // medicationTaken = true; 
        digitalWrite(buzzer, LOW);     

        // test: display message         
        // Serial.print(" User took medication, but box is open");                   

    // else do this       
    } else {
      
        digitalWrite(buzzer, LOW);
            
        // test: display message  
        // Serial.print(" OTHER REASON!");         
        
    }
    
    Serial.println();
    
  }
}
