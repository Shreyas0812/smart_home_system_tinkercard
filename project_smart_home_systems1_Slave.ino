//For i2c
#include <Wire.h>

//For Keypad
#include <Keypad.h>

//For LCD
#include <LiquidCrystal.h>

//For servo
#include <Servo.h>

#define Password_Length 5

//to save message
char dataRx[10] = "";

//start everything
bool person_proximity = false;


Servo myservo;
LiquidCrystal lcd(12, 11, A0, A1, A2, A3);

int pos = 0;
char Data[Password_Length];
char Master[Password_Length] = "0812";
int data_count = 0;
int master_count = 0;
bool Pass_is_good;
bool door = false;
char customKey;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad customKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  Wire.begin(4);	//Join i2c bus with address #4
  Wire.onReceive(receiveEvent); //Send Reply
  
  myservo.attach(9, 2000, 2400);
  //ServoClose();
  lcd.begin(16, 2);
}

void loop()
{
  /*
  digitalWrite(13, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(13, LOW);
  delay(1000); // Wait for 1000 millisecond(s)
  */
  //Serial.print(person_proximity);
  if (person_proximity)
  {
    if (door == true)
    {
      customKey = customKeypad.getKey();
      if (customKey == '#')
      {
        lcd.clear();
        ServoClose();
        lcd.print("Door is closed");
        delay(3000);
        door = false;
      }
    }
    else
      Open();
  }
  //delay(1000);
}

void receiveEvent(int howMany)
{
  int index = 0;
  while(Wire.available())
  {
    char c = Wire.read();
    dataRx[index++] = c;
    dataRx[index] = '\0';
    Serial.print(c);
    person_proximity = true;
  }
  
  //Serial.println("Final");
  //Serial.println(dataRx);
  /*
  while (true)
  {
    if (door==true)
    {
      customKey = customKeypad.getKey();
      if (customKey == '#')
      {
        lcd.clear();
        ServoClose();
        lcd.print("Door is closed");
        delay(3000);
        door = false;
      }
    }
    else 
    {
      Open();
    }
  }
  */
}

void Open()
{
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  
  customKey = customKeypad.getKey();
  if (customKey)
  {
    Data[data_count] = customKey;
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);
    data_count++;
  }
  //Serial.print(data_count);
  if (data_count == Password_Length-1)
  {
    if (!strcmp(Data, Master))
    {
      //open the door
      lcd.clear();
      ServoOpen();
      lcd.print("Door is Open ");
      door = true;
  	  //Sending back to master
      delay(5000);
      
      //wait to enter
      lcd.setCursor(0, 1);
      lcd.print("Waiting");
      for (int i =0; i<9; i++)
      {
        delay(1000);
        lcd.print(".");
      }
      
      //closing the door
      lcd.clear();
      lcd.print("Closing the Door");
      delay(1000);
      ServoClose();
      door = false;
      person_proximity = false;
    }
    else
    {
      lcd.clear();
      lcd.print("Wrong Password");
      door = false;
    }
    delay(1000);
    lcd.clear();
    clearData();
  }
}

void ServoOpen()
{
  for (pos = 0; pos<=90; pos+=10)
  {
    myservo.write(pos);
  }
}

void ServoClose()
{
  for (pos=90; pos>=0; pos-=10)
  {
    myservo.write(pos);
  }
  Serial.print('Close the door');
}

void clearData()
{
  while (data_count != 0)
  {
    Data[data_count--] = 0;
  }
}
