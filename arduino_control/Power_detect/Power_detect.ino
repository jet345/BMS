#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include "ACS712.h"
ACS712 sensor(ACS712_30A, A0);
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int e;
int n;
char message[20];
char my_packet[100];
unsigned long lastSend_r;
byte buff[2];
String tmp = "";
float U = 220;
int pos = 0; 
float P_avg;
int count;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  lastSend_r=0;
  count=0;
  
}

void loop(void)
{
  count++;
  float I = sensor.getCurrentAC();
  float P = U * I;
  P_avg = P_avg + P;
 //Serial.println(P);
  if(count%60==0){
    int t = dht.readTemperature();
    int h = dht.readHumidity();
    
    P_avg = P_avg / 60;
    tmp+=String(t);
    tmp+=",";
    tmp+=String(h);
    tmp+=",";
    tmp+=String(P_avg);

    Serial.println(tmp);
    P_avg=0;
    tmp="";
  }
  delay(1000);

  
  
  
  /*if(millis()-lastSend_r > 60000){
    P_avg = P_avg/60;
    Serial.print()
    //delay(1000);
    
    lastSend_r=millis();

    P_avg=0;
  }*/

}

