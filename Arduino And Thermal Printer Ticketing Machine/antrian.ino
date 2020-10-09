#include <Servo.h>
#include <PString.h>
#include <Wire.h>
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include "RTClib.h"

#include "logo.h"

RTC_DS1307 RTC;

#define TX_PIN 3 // Arduino transmit  YELLOW WIRE  connect to RX printer
#define RX_PIN 2 // Arduino receive   GREEN WIRE   connect to TX printer

SoftwareSerial mySerial(RX_PIN, TX_PIN); 
Adafruit_Thermal printer(&mySerial);   

int antrian=0;
int AutoOn=0;

const int buttonManual = 10;        // The number of the buttonManual
int buttonStateManual=0; 
const int buttonAuto = 11;          // The number of the buttonAuto
int buttonStateAuto=0;
const int buttonFeed = 12;          // The number of the buttonFeed
int buttonStateFeed=0;

const int sensorAuto = 4;           // The number of the sensorAuto
int sensorStateAuto=0;
const int sensorEmptypaper = 5;     // The number of the sensorEmtypaper
int sensorStateEmptypaper=0;

const int Buzzer =  6;      // the number of the Buzzer pin            
const int ledAuto = 7;      // the number of the LEDAuto pin            
Servo myservo;              // create servo object to control a servo

void setup()
{
 pinMode(buttonManual, INPUT);
 digitalWrite(buttonManual, HIGH);       // turn on pullup resistors 
 pinMode(buttonAuto, INPUT);
 digitalWrite(buttonAuto, HIGH);         // turn on pullup resistors 
 pinMode(buttonFeed, INPUT);
 digitalWrite(buttonFeed, HIGH);         // turn on pullup resistors 
 pinMode(sensorAuto, INPUT);
 digitalWrite(sensorAuto, HIGH);         // turn on pullup resistors 
 pinMode(sensorEmptypaper, INPUT);
 digitalWrite(sensorEmptypaper, HIGH);   // turn on pullup resistors 
 
 pinMode(Buzzer, OUTPUT);
 pinMode(ledAuto, OUTPUT);
 
 Wire.begin();
    RTC.begin();
     if (! RTC.isrunning())
     {
      Serial.println("RTC is NOT running!");
      RTC.adjust(DateTime(__DATE__, __TIME__));
     }
 
 mySerial.begin(9600);  // Initialize SoftwareSerial
 printer.begin();       // Init printer (same regardless of serial type)
 myservo.attach(9);     // attaches the servo on pin 9 to the servo object
 myservo.write(0);      // tell servo to go to position in variable 'pos'
 delay(500);
 myservo.detach();      // detaches the servo
}

void loop()
{  
  buttonStateManual = digitalRead(buttonManual);
  buttonStateAuto = digitalRead(buttonAuto);
  buttonStateFeed = digitalRead(buttonFeed);
  
  sensorStateAuto = digitalRead(sensorAuto);
  sensorStateEmptypaper = digitalRead(sensorEmptypaper);

  if (buttonStateAuto == LOW)
      {
        AutoOn+=1;
        if (AutoOn == 2)
            {
              AutoOn=0;
            }
        delay(2000);
      }
  if (AutoOn == 0)
        {
         digitalWrite(ledAuto, LOW); 
        }
        else
        {
         digitalWrite(ledAuto, HIGH); 
        }

  if (sensorStateEmptypaper == LOW)
      {
        digitalWrite(Buzzer, LOW);  
        if ((buttonStateManual == LOW)||((sensorStateAuto == HIGH)&&(AutoOn == 1)))
            {
               delay(2000);
               myservo.attach(9);  // attaches the servo on pin 9 to the servo object
               myservo.write(0);   // tell servo to go to position in variable 'pos'
               delay(500);
               myservo.detach();   // detaches the servo
               
               char buffer[40];
               PString str(buffer, sizeof(buffer));
               if(antrian<100)
                  {
                    str.print(0);
                    if(antrian<10)
                      {
                        str.print(0);
                      }
                  }
               str.print(antrian);
               antrian+=1;

               printer.justify('C');
               printer.boldOn();

               printer.setSize('M');
               printer.println(F("Nomor antrian :"));
               printer.feed(1);    
               
               printer.setSize('L');
               printer.println(str);
               printer.feed(2);
               
               printer.boldOff(); 
               printer.setSize('S');
               printer.println(F("Terimakasih Anda Telah Menunggu"));
               DateTime now = RTC.now();
               str.begin();
               if(now.day()<10)
                  {
                    str.print(0);
                  }
               str.print(now.day(), DEC);
               str.print('-');
               if(now.month()<10)
                  {
                    str.print(0);
                  }
               str.print(now.month(), DEC);
               str.print('-');
               str.print(now.year(), DEC);
               str.print("           ");
               if(now.hour()<10)
                  {
                    str.print(0);
                  }
               str.print(now.hour(), DEC);
               str.print(':');
               if(now.minute()<10)
                  {
                    str.print(0);
                  }
               str.print(now.minute(), DEC);
               str.print(':');
               if(now.second()<10)
                  {
                    str.print(0);
                  } 
               str.print(now.second(), DEC);
               printer.println(str);

               printer.feed(2);           
               printer.setSize('S');
               printer.println(F("Coemplong Customer Service\nCenter"));
  
               myservo.attach(9);           // attaches the servo on pin 9 to the servo object
               myservo.write(137);          // tell servo to go to position in variable 'pos'
               delay(1000);                 // waits for the servo to reach the position
               myservo.write(0);            // tell servo to go to position in variable 'pos'
               delay(500);
               myservo.detach();            // detaches the servo

               // Print the 272x92 pixel logo in logo.h:
               printer.printBitmap(logo_width, logo_height, logo_data);
    
               printer.feed(1);
            
            }
        else
            {
               if (buttonStateFeed == LOW)
                  {
                    printer.feed(10);        // Feed paper to scroll and insert the paper
                  }     
            }
       printer.wake();                       // Wake Up the printer
       printer.setDefault();                 // Set the printer to default
      }
  else
      {
      digitalWrite(Buzzer, HIGH);            // Activate Buzzer when paper empty
      }
}
