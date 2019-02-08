#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Ethernet.h>
#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance
EnergyMonitor emon2;
LiquidCrystal_I2C lcd(0x27,16 ,2);
int i = 0;
float tenvals = 0.0;
float minval = 1000;
float maxval = 0.0;
String color1="danger";
String color2="danger";
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//Dirección MAC de la Ethernet Shield
IPAddress ip(192,168,9,150); //IP del Arduino
EthernetServer server(80); //Crea un servidor Web con el puerto 80 que es el puerto HTTP por defecto
String funcionalidad="";
#define AUTO 5
#define MANUAL 4
const int sensorPin = A1;  
int sensorValue;      
float value;
#define RESTART asm("jmp 0x0000"); 
int timer=0;
int contactor1=9;
int contactor2=8;
int ignis = 7;
int star = 6;
int contador = 1;
int cont = 0;
int repetir;
float voltaje;
float V=0.00;

 void setup() 
{
 lcd.init();
 lcd.clear();
  Serial.begin(9600);
   pinMode(AUTO,INPUT);
 pinMode(MANUAL,INPUT);
  Ethernet.begin(mac, ip);
  server.begin();
  lcd.backlight();
  pinMode(ignis, OUTPUT);
  pinMode(star, OUTPUT);
  pinMode(10,INPUT);
  digitalWrite(ignis, HIGH);
  digitalWrite(star, HIGH);
  emon1.voltage(0, 114.10, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon2.voltage(0,114.10,1.7);
  digitalWrite(contactor1,HIGH);
  digitalWrite(contactor2,HIGH);
}

void loop()
{
  digitalWrite(contactor1,LOW);
  lcd.setCursor(0,0);
  if(digitalRead(AUTO)==0)
  {
    funcionalidad="AUTOMATICO";
    for(int x=0;x<1;x++)
  {  
    sensorValue = analogRead(sensorPin);      
    value = fmap(sensorValue, 0, 1024, 0.0, 24.88); 
    Serial.println(value);
    lcd.println(funcionalidad);
    lcd.setCursor(0,2);
    lcd.println("Baterias: ");
    lcd.print(value);
    delay(10000);
  }
    Serial.println(funcionalidad);
    if(value<=11.7)
    {

      ciclo();
    }
  }
  if(digitalRead(MANUAL)==0)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    funcionalidad="MANUAL";
    Serial.println(funcionalidad);
    lcd.println(funcionalidad);
  }

}

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ciclo()
{
      digitalWrite(11,LOW);
      digitalWrite(10,HIGH);
      delay(500);
  otra:;
      lcd.setCursor(1,1);
      digitalWrite(ignis,LOW);
      delay(5000);
      digitalWrite(star,LOW);
      delay(5000);
      digitalWrite(star, HIGH);
      if(cont<3)
      {
         delay(30000);
           emon1.calcVI(20,2000); 
             float Vrms = (emon1.Vrms );
         while(i<10)
         {
         if (Vrms < 0) { Vrms = 0.0; }
         
         tenvals += Vrms;
         if (minval > Vrms) { minval = Vrms; }
         if (maxval < Vrms) { maxval = Vrms; }
         
         i++; 
         }
        
         if (i == 10)
          {
            Serial.println(maxval);
            lcd.setCursor(0,3);
            lcd.println("GENERADOR:");
            lcd.println(maxval);
            if(maxval>80)
            {
              while(timer<10800000)
              {
                 Serial.println(timer%((1000*60))*10);
                 if(timer%((1000*60)*10)==0)
                 {
                    comprobar();
                 }
                 delay(1000);
                 timer+=1000;
              }
                 digitalWrite(ignis,HIGH);
                 delay(10000);
                 RESTART
            }
         else 
          {
             cont++;
             digitalWrite(ignis,HIGH);
             delay(10000);
             goto otra;
          } 
      }
      else
      {
        delay(100);
        digitalWrite(ignis,HIGH);
        digitalWrite(star,HIGH);
  
        while(digitalRead(3)!= HIGH)
        {
         delay(1000);
        }
      }        
}
}

void comprobar()
{
          lcd.setCursor(0,4);
    emon2.calcVI(20,2000); 
     float Vrms = (emon1.Vrms );
 if (Vrms < 0) { Vrms = 0.0; }
 
 tenvals += Vrms;
 if (minval > Vrms) { minval = Vrms; }
 if (maxval < Vrms) { maxval = Vrms; }
 
 i++;

 if (i == 10)
  {
    Serial.println(maxval);
    lcd.println("ENEE: ");
    lcd.println(maxval);
    if(maxval>80)
    {

        digitalWrite(contactor2, HIGH);
        delay(100);
        digitalWrite(ignis,HIGH);
        digitalWrite(contactor1,LOW);
        delay(1600);
        digitalWrite(star,HIGH);
        while(digitalRead(3)!= HIGH)
        {
         delay(1000);
        }
    }
  i = 0;
  tenvals = 0.0;
  minval = 1000.0;
  maxval = 0.0;
  }
  delay(100);

}
