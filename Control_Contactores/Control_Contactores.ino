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
#define SAMPLING 300
#define VOFFSET  512
#define ADC_PIN A0
#define ADC2_PIN A3
#define AMPLITUDE 233.00
#define REAL_VAC  143.00
int adc_max,adc_min;
int adc_max2,adc_min2;
int adc_vpp2;
int adc_vpp; 
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
boolean estado1;
boolean estadoAnterior1;
boolean estado2;
boolean estadoAnterior2;
int tiempoanti1=10;
int tiempoanti2=10;

void setup() 
{
  lcd.begin(20,4);
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
  emon1.voltage(0, 234.26, 1);  // Voltage: input pin, calibration, phase_shift
  emon1.current(0, 111.1);       // Current: input pin, calibration.
  emon2.voltage(3, 234.26, 1);  // Voltage: input pin, calibration, phase_shift
  emon2.current(3, 115.1);       // Current: input pin, calibration.
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
 if(MODO=="OFF")
 {
  digitalWrite(7,HIGH);
  digitalWrite(6,HIGH);
 }
 if(MODO=="AUTO")
 {
  automatico();
 }
 delay(1000);
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
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.println("APAGADO");
    Serial.println("APAGADO");
  MODO="OFF";
  }
}

void manual()
{
//  boolean estadoBotón;
//  boolean estadoAnteriorBotón;
  
    leerBaterias();
    leerAC();
   if(digitalRead(8)==HIGH)
    {
      if(antirrebote1(8));
      {
      digitalWrite(7,HIGH);
       lcd.println("                          ");
      delay(5000);
      digitalWrite(6,LOW);
      lcd.setCursor(0,3);
      lcd.println("PRINCIPAL           ");
      }
    }
  if(digitalRead(9)==HIGH)
    {
      if(antirrebote2(9))
      {
      digitalWrite(6,HIGH);
       lcd.println("                        ");
      delay(5000);
      digitalWrite(7,LOW);
      lcd.setCursor(0,3);
      lcd.println("          GENERADOR");
      }
    }
}

void automatico()
{
    leerAC();
    leerBaterias();
    Serial.println(funcionalidad);
    if(value<=11.7)
    {
      ciclo();
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
            
            float V = map(read_VAC2(),0,AMPLITUDE,0,REAL_VAC*100.00)/100.00;
            String data;
            float Irms=emon2.Irms;
            V=((V/2)-(Irms/(1.5)));
            data = String(V,1); 
            Serial.println(data);
            lcd.setCursor(0,2);
            lcd.println(data);
            if(V>80)
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

void comprobar()
{
      read_VAC();
  float V2 = map(adc_vpp,0,AMPLITUDE,0,REAL_VAC*100.00)/100.00;
  String data2;

  float Irms2=emon1.Irms;
  V2=((V2/2)-(Irms2/(1.5)));
  data2 = String(V2,1); 
  Serial.println(data2);
  lcd.setCursor(12,2);
  lcd.println(data2);
    if(V2>80)
    {
        digitalWrite(6, HIGH);
        delay(100);
        digitalWrite(ignis,HIGH);
        digitalWrite(7,LOW);
        delay(1600);
        digitalWrite(star,HIGH);
        while(digitalRead(3)!= HIGH)
        {
         delay(1000);
        }
    }
      delay(100);

  }



void leerBaterias()
{
    lcd.setCursor(0,1);
    sensorValue = analogRead(sensorPin);      
    value = fmap(sensorValue, 0, 1024, 0.0, 24.88); 
    lcd.println("Baterias: ");
    lcd.print(value);
}

float read_VAC()
{
  
  int cnt;
  adc_max = 0;
  adc_min = 1024;
  
  for(cnt=0;cnt<SAMPLING;cnt++)
  {
    int adc = analogRead(ADC_PIN);
    if(adc > adc_max)
    {
      adc_max = adc;
    }
    if(adc < adc_min)
    {
      adc_min = adc;
    }
  }
  adc_vpp = adc_max-adc_min;   
  return adc_vpp;
}

float read_VAC2()
{
int cnt2;
  adc_max2 = 0;
  adc_min2 = 1024;
  
  for(cnt2=0;cnt2<SAMPLING;cnt2++)
  {
    int adc2 = analogRead(ADC2_PIN);
    if(adc2 > adc_max2)
    {
      adc_max2 = adc2;
    }
    if(adc2 < adc_min2)
    {
      adc_min2 = adc2;
    }
  }
  adc_vpp2 = adc_max2-adc_min2;
  return adc_vpp;
}


boolean antirrebote1(int pin1)
{

  do
  {
    estado1=digitalRead(pin1);
    if(estado1 != estadoAnterior1)
    {
      estadoAnterior1=estado1;
      cont=0;
    }
    else
    {
      cont++;
    }
    delay(1);
  }
  while(cont <tiempoanti1);
  return estado1;
}


boolean antirrebote2(int pin2)
{
  int cont2=0;
  do
  {
    estado2=digitalRead(pin2);
    if(estado2 != estadoAnterior2)
    {
      estadoAnterior2=estado2;
      cont2=0;
    }
    else
    {
      cont2++;
    }
    delay(1);
  }
  while(cont2 < tiempoanti2);
  return estado2;
}

void leerAC()
{
   emon1.calcVI(20,2000);
  emon2.calcVI(20,2000);
  read_VAC2();
  float V = map(adc_vpp2,0,AMPLITUDE,0,REAL_VAC*100.00)/100.00;
  String data;
  float Irms=emon2.Irms;
  V=((V/2)-(Irms/(1.5)));
  data = String(V,1); 
  Serial.println(data);
  lcd.setCursor(0,2);
  lcd.println(data);

  read_VAC();
  float V2 = map(adc_vpp,0,AMPLITUDE,0,REAL_VAC*100.00)/100.00;
  String data2;
  float Irms2=emon1.Irms;
  V2=((V2/2)-(Irms2/(1.5)));
  data2 = String(V2,1); 
  Serial.println(data2);
  lcd.setCursor(12,2);
  lcd.println(data2);
}
