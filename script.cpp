#include <LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>

int ledPin= 3;
int ledNo= 12;
int tempSensorPin = 2;
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Adafruit_NeoPixel strip= Adafruit_NeoPixel(ledNo,ledPin,NEO_RGB+NEO_KHZ800);


int buzzerPin= 2;
int echoPin= 6;
int trigPin= 5;
int minDistance = 100;
int maxDistance = 300;

int onOffTime;
int IRSense;

int autoOffTrigger=0;

void setup() 
{
  pinMode(5, OUTPUT); //Motor pump control pin
  pinMode(2, OUTPUT); //Sensor sensing Pin
  
  
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial. begin(9600);  
  strip.begin();
  lcd.begin(16, 2);
  lcd.print("Your Temperature:");
  for(int i = 0; i < ledNo; i++)
  {
   strip.setPixelColor(i,strip.Color(0,0,0));
  }
  strip.show();
}

void loop() 
{
  
  //code for temperature sensor
  
  int tempReading = analogRead(tempSensorPin); 	//Read the temperature degree
  double temp;
  temp = (double)tempReading / 1024;  			//find % of input reading
  temp = temp * 5;                     			//multiply by 5V to get voltage
  temp = temp - 0.5;                   			//Subtract the offset 
  temp = temp * 100;                   			//Convert to degrees
  
  //code for LCD display
  
  lcd.setCursor(0, 1);			// set the cursor to column 0, line 1
  lcd.print(temp);

  //code for hand sanatizer dispenser
  
  int IRSense= analogRead(A0); // Read Sensor Value
  int onOffTime= analogRead(A1); // Read How much volume to dispense
  
  int time=map(onOffTime,0,1023,0,10);//convert to simple scale
  //Serial.println("IR: "+String(IRSense));
  //Serial.println("pot: "+String(onOffTime)+ ": "+String(time));
 
  if(IRSense >78) //If sense higher than 78 LED INDICATE
  {
    digitalWrite(7,1);
  }
  else 
  {
    digitalWrite(7,0);
  }
  //IF IR sense higher than 78
  //Motor pump will ON for Sometime mentioned in "time"
  //Then Turn Off
  if(IRSense >78 && autoOffTrigger==0)
  {
  	digitalWrite(4,1);
    digitalWrite(buzzerPin, HIGH); //buzzing when it dispenses the hand sanitizer
  	delay(time*1000); // 1000 is 1000 millisecond(s) 
  	digitalWrite(4,0);
    autoOffTrigger=1;
    Serial.println("Dispensing... ");
  }
  else if(IRSense <78)
  {
    //AutoOFFTrigger is used to cut off motor pump
    //Make for each sense, only onetime dispenser will come out
    autoOffTrigger=0;
  }  
  
  //Code for social distancing
  
  int distance = calcDistance();
  //distance between the user and he person behind him
  int ledsToGlow = map(distance, minDistance, maxDistance, ledNo, 1);
  // no of leds that tell closeness between the users
  if(ledsToGlow == 12)
  {
    digitalWrite(buzzerPin, HIGH);
  }
  else
  {
    digitalWrite(buzzerPin, LOW);
  }
  for(int i = 0; i < ledsToGlow; i++)
  {
    if(i < 4)
    {
      strip.setPixelColor(i,strip.Color(50,0,0));//green,red,blue
    }
    else if(i >= 4 && i < 8)
    {
      strip.setPixelColor(i,strip.Color(50,50,0));//green,red,blue
    }
    else if(i >= 8 && i < 12)
    {
      strip.setPixelColor(i,strip.Color(0,50,0));//green,red,blue
    }
  }
  for(int i = ledsToGlow; i < ledNo; i++)
  {
    strip.setPixelColor(i,strip.Color(0,0,0));
  }
  strip.show();
  delay(50);
}

int calcDistance()
{
  long distance,duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration/29/2;
  if(distance >= maxDistance)
  {
    distance = maxDistance;
  }
  if(distance <= minDistance)
  {
    distance = minDistance;
  }
  return distance;
}