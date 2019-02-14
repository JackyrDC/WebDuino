#include <EmonLib.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define I2C_ADDR    0x27
String MODO;
EnergyMonitor emon1;         
EnergyMonitor emon2;
LiquidCrystal_I2C lcd(I2C_ADDR,2, 1, 0, 4, 5, 6, 7);
int i = 0;
float tenvals = 0.0;
float minval = 1000;
float maxval = 0.0;
String funcionalidad="";
#define AUTO 4
#define MANUAL 5
const int sensorPin = A1;  
int sensorValue;      
float value;
#define RESTART asm("jmp 0x0000"); 
int timer=0;
int energiaCalle=9;
int energiaGenerador=8;
int ignis = 7;
int star = 6;
int contador = 1;
int cont = 0;
int repetir;
float voltaje;
float V=0.00;

void setup() 
{
  lcd.begin(16,2);
  Serial.begin(9600);
  pinMode(AUTO,INPUT);
  pinMode(MANUAL,INPUT);
  lcd.backlight();
  pinMode(ignis, OUTPUT);
  pinMode(star, OUTPUT);
  pinMode(10,INPUT);
  digitalWrite(ignis, HIGH);
  digitalWrite(star, HIGH);
  digitalWrite(energiaCalle,HIGH);
  digitalWrite(energiaGenerador,HIGH);
  emon1.voltage(0, 114.10, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon2.voltage(3,114.10,1.7);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,INPUT);
  pinMode(9,INPUT); 
  digitalWrite(6,HIGH);
  digitalWrite(7,HIGH);        
}

void loop() 
{
 modo();
 delay(1000);
  digitalWrite(energiaCalle,LOW);
 if(MODO=="MANUAL")
 {
    manual();
 }
}

void modo()
{
  if(digitalRead(AUTO)==1)
  {
    lcd.home();
    lcd.setCursor(5,0);
    lcd.println("  AUTO");
    Serial.println("AUTO");
    MODO="AUTO";
  }
  else if(digitalRead(MANUAL)==1)
  {
    lcd.home();
    lcd.setCursor(5,0);
    lcd.println(" MANUAL");
    Serial.println("MANUAL");
    MODO="MANUAL";
  }
  else
  {
    lcd.setCursor(5,0);
    lcd.println("APAGADO");
    Serial.println("APAGADO");
  MODO="OFF";
  }
}

void manual()
{
   if(digitalRead(8)==HIGH)
    {
      digitalWrite(7,HIGH);
      delay(10000);
      digitalWrite(6,LOW);
      lcd.setCursor(0,2);
      lcd.println("LINEA: PRINCIPAL");
    }
  if(digitalRead(9)==HIGH)
    {
      digitalWrite(6,HIGH);
      delay(10000);
      digitalWrite(7,LOW);
      lcd.setCursor(0,2);
      lcd.println("LINEA: GENERADOR");
    }
}
