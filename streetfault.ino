#include <LiquidCrystal.h>
#include <WiFi.h>
#include "ThingSpeak.h"

LiquidCrystal lcd(4,16,17,5,18,19);

// WIFI
const char* ssid = "Iot";
const char* password = "iot12345";

WiFiClient client;

unsigned long channelID = 3217881;
const char* writeAPIKey = "AY033E197CMMIM7S";

// PINS
#define IR_Sensor 14
#define Street_Light_1 12
#define Street_Light_2 15
#define ACS1_Pin 34
#define ACS2_Pin 35
#define LDR1_Pin 27
#define LDR2_Pin 26

// ADC
#define ADC_REF 3.3
#define ADC_RES 4095.0
#define ACS_SENSITIVITY 0.185

float offset1=0;
float offset2=0;

float current1=0;
float current2=0;

int fault1Count=0;
int fault2Count=0;

bool smsSent=false;

unsigned long lastTSupdate=0;
const long TSinterval=10000;

// ======================================

void setup()
{

Serial.begin(9600);

pinMode(IR_Sensor,INPUT);
pinMode(LDR1_Pin,INPUT);
pinMode(LDR2_Pin,INPUT);

pinMode(Street_Light_1,OUTPUT);
pinMode(Street_Light_2,OUTPUT);

lcd.begin(16,2);

lcd.print("STREET LIGHT");
lcd.setCursor(0,1);
lcd.print("MONITORING");
delay(2000);
lcd.clear();

analogReadResolution(12);
analogSetAttenuation(ADC_11db);

// WIFI CONNECT

lcd.print("WiFi Connecting");

WiFi.begin(ssid,password);

while(WiFi.status()!=WL_CONNECTED)
{
delay(500);
}

lcd.clear();
lcd.print("WiFi Connected");
delay(1500);
lcd.clear();

ThingSpeak.begin(client);

// SENSOR CALIBRATION

lcd.print("Calibrating...");

long sum1=0;
long sum2=0;

for(int i=0;i<300;i++)
{
sum1+=analogRead(ACS1_Pin);
sum2+=analogRead(ACS2_Pin);
delay(5);
}

offset1=(sum1/300.0)*ADC_REF/ADC_RES;
offset2=(sum2/300.0)*ADC_REF/ADC_RES;

lcd.clear();
lcd.print("Calibration OK");
delay(1500);
lcd.clear();

}

// ======================================

void loop()
{

bool lightsON=digitalRead(IR_Sensor);

// LIGHT CONTROL

if(lightsON)
{
digitalWrite(Street_Light_1,HIGH);
digitalWrite(Street_Light_2,HIGH);
}
else
{
digitalWrite(Street_Light_1,LOW);
digitalWrite(Street_Light_2,LOW);
}

// CURRENT MEASUREMENT

current1=readCurrent(ACS1_Pin,offset1);
current2=readCurrent(ACS2_Pin,offset2);

// LCD CURRENT DISPLAY

// -------- Screen 1 : Current Values --------

lcd.clear();

lcd.setCursor(0,0);
lcd.print("L1:");
lcd.print(current1,2);
lcd.print("A");

lcd.setCursor(0,1);
lcd.print("L2:");
lcd.print(current2,2);
lcd.print("A");

delay(2000);


// -------- Screen 2 : System Status --------

lcd.clear();

lcd.setCursor(0,0);
lcd.print("SYSTEM NORMAL");

delay(2000);
// FAULT CHECK

bool fault1=false;
bool fault2=false;

if(lightsON)
{
if(digitalRead(LDR1_Pin)==HIGH) fault1=true;
if(digitalRead(LDR2_Pin)==HIGH) fault2=true;
}

// FAULT ACTION

if(fault1||fault2)
{

if(!smsSent)
{

lcd.clear();

if(fault1&&fault2)
{
lcd.print("BOTH LAMP FAULT");
sendSMS("Lamp One & Lamp Two Fault");
}

else if(fault1)
{
lcd.print("LAMP ONE FAULT");
sendSMS("Lamp One Fault");
}

else if(fault2)
{
lcd.print("LAMP TWO FAULT");
sendSMS("Lamp Two Fault");
}

smsSent=true;

delay(3000);   // show message clearly

}

fault1Count+=fault1;
fault2Count+=fault2;

}
else
{
smsSent=false;

lcd.clear();
lcd.print("SYSTEM NORMAL");
delay(1500);
}

// THINGSPEAK UPDATE

if(millis()-lastTSupdate>TSinterval)
{

int status1 = (lightsON && digitalRead(LDR1_Pin)==LOW) ? 1 : 0;
int status2 = (lightsON && digitalRead(LDR2_Pin)==LOW) ? 1 : 0;

ThingSpeak.setField(1,current1);
ThingSpeak.setField(2,current2);
ThingSpeak.setField(3,fault1Count);
ThingSpeak.setField(4,fault2Count);
ThingSpeak.setField(5,status1);
ThingSpeak.setField(6,status2);

ThingSpeak.writeFields(channelID,writeAPIKey);

lastTSupdate=millis();

fault1Count=0;
fault2Count=0;

}

}

// ======================================

float readCurrent(int pin,float offset)
{

const int samples=100;
long sum=0;

for(int i=0;i<samples;i++)
{
sum+=analogRead(pin);
delayMicroseconds(200);
}

float adcAvg=sum/(float)samples;

float voltage=adcAvg*ADC_REF/ADC_RES;

float current=(voltage-offset)/ACS_SENSITIVITY;

return abs(current);

}

// ======================================

void sendSMS(String message)
{

Serial.println("AT+CMGF=1");
delay(2000);

Serial.println("AT+CMGS=\"7200462481\"");
delay(2000);

Serial.println(message);
delay(2000);

Serial.write(26);

delay(5000);

}
