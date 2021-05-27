//LCD sreen
#include <LiquidCrystal.h>
//i2c to open the door
#include <Wire.h>

//initialize library with numbers of interface pins
LiquidCrystal lcd (12, 11, 5, 4, 3, 2);

//ultrasound sensor
long distance_ultrasonic = 0;

//time
unsigned long previousMillis = 0;
unsigned long currentMillis;
unsigned long Doorinterval = 40000; //40 sec

//Relay control using Photoresistor and PIR sesnor
int relayNo = 13;	//DIGITAL pin number
int inputPir = 8;	//DIGITAL pin number
int inputLDR = A0; //analog pin
int pirVal;
int ldrVal;

//Gas Sensor and Buzzer
int inputGas = A3;
const byte BUZZER_Pin = 10; 

void setup()
{
  //pinMode(13, OUTPUT);
  
  //set number of rows and cols in lcd
  lcd.begin(16,2);
  
  //i2c to open the door
  Wire.begin();
  
  //For fan, lights, pir sensor and lddr
  pinMode(relayNo, OUTPUT);
  pinMode(inputPir, INPUT);
  pinMode(inputLDR, INPUT);
  
  //For Gas Sensor and Buzzer
  pinMode(inputGas, INPUT);
  pinMode(BUZZER_Pin, OUTPUT); 
  
  //To print stuff on Serial Monitor
  Serial.begin(9600);
}

void loop()
{
  /*
  digitalWrite(13, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(13, LOW);
  delay(1000); // Wait for 1000 millisecond(s)
  */
  //ultrasonic getting distance and printing on Serial Monitor and lcd screen
  distance_ultrasonic = readUltrasonicDistance(7,6);
  
  Serial.println("Distance from the Door: ");
  Serial.print(distance_ultrasonic);
  Serial.println("cm");
  
  lcd.setCursor (0, 0); //set to position where to display text
  lcd.print("D:");
  lcd.print(distance_ultrasonic);
  lcd.print("cm");
  
  if (distance_ultrasonic<=100)
  {
    //Running at 15 sec time intervals
    currentMillis = millis();
    
    if (currentMillis - previousMillis > Doorinterval)
    {
      previousMillis = currentMillis;
      //Call Slave Arduino for door unlock
      Serial.println("Checking password and open door");  
      Wire.beginTransmission(4);
      Wire.write("Start");
      Wire.endTransmission();
      delay(500);
    }
  }
  
  delay(10);
  //LDR and PIR to turn on lights and fan
  
  pirVal = digitalRead(inputPir);	//pin 8
  ldrVal = analogRead(inputLDR); 	//pin A0
  Serial.println(pirVal);
  Serial.print(ldrVal);
  
  if (ldrVal < 600)
  {
    Serial.print("Dark");
    if (pirVal == HIGH)
    {
      Serial.print("Person present");
      digitalWrite(relayNo, HIGH);
      lcd.setCursor(0, 1);
      lcd.print("L: On ");
      delay(2000);
    }
    else
    {
      Serial.println("NOONE HERE");
      digitalWrite(relayNo, LOW);
      lcd.setCursor(0, 1);
      lcd.print("L: Off");
      delay(400);
    }
  }
  else
  {
    Serial.println("Light outside");
    digitalWrite(relayNo, LOW);
    lcd.setCursor(0, 1);
    lcd.print("L: Off");
    delay(400);
  }
  
  int GasVal = analogRead(inputGas);
  Serial.print(GasVal);
  lcd.setCursor(8, 0);
  if (GasVal<=85)
  {
    lcd.print("G:Low ");
    digitalWrite(BUZZER_Pin, LOW);
  }
  else if (GasVal<=120)
  {
    lcd.print("G:Med ");
    digitalWrite(BUZZER_Pin, LOW);
  }
  else if (GasVal<=200)
  {
    lcd.print("G:High");
    digitalWrite(BUZZER_Pin, LOW);
  }
  else
  {
    lcd.print("G:EXIT");
    digitalWrite(BUZZER_Pin, HIGH); 
  }
  
  delay(100);
}

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT); //clear the pin
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  
  // Set trigger pin to HIGH for 10 microsec 
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  pinMode(echoPin, INPUT);
  //read echo pin and return sound wave travel time 
  return (pulseIn(echoPin, HIGH)*0.01723); //0.01723 for cm
}
                                       
                                