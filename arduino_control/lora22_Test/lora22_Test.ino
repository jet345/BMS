#include <arduinoLoRa.h>
#include <arduinoUtils.h>
#include <math.h>
#include <Wire.h>
#include <SPI.h>
#include <IRremote.h>
#include <DHT.h>


IRsend irsend;
#define DHTPIN 4
#define DHTTYPE DHT11

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastSend_r;
int e;
String tmp ="";
char message[20];
char my_packet[100];
int n;

void setup() {
  //Light sensor
  n=0;
  Wire.begin();
  //Serial
  Serial.begin(9600);

  //LoRa
  // Print a start message
  Serial.println(F("SX1272 module and Arduino: send packets without ACK"));
  // Power ON the module
  e = sx1272.ON();
  Serial.print(F("Setting power ON: state "));
  Serial.println(e, DEC);  
  // Set transmission mode and print the result
  e |= sx1272.setMode(4);
  Serial.print(F("Setting Mode: state "));
  Serial.println(e, DEC);  
  // Set header
  e |= sx1272.setHeaderON();
  Serial.print(F("Setting Header ON: state "));
  Serial.println(e, DEC);  
  // Select frequency channel
  e |= sx1272.setChannel(CH_08_900);
  Serial.print(F("Setting Channel: state "));
  Serial.println(e, DEC); 
  // Set CRC
  e |= sx1272.setCRC_ON();
  Serial.print(F("Setting CRC ON: state "));
  Serial.println(e, DEC);  
  // Select output power (Max, High or Low)
  e |= sx1272.setPower('H');
  Serial.print(F("Setting Power: state "));
  Serial.println(e, DEC);  
  // Set the node address and print the result
  e |= sx1272.setNodeAddress(11);
  Serial.print(F("Setting node address: state "));
  Serial.println(e, DEC);  
  // Print a success message
  if (e == 0)
    Serial.println(F("SX1272 successfully configured"));
  else
    Serial.println(F("SX1272 initialization failed"));   

  //DHT 
  dht.begin();

  //long
  lastSend_r = 0;
  
}

void loop() {
  int n;
  e = sx1272.receivePacketTimeout(1000);
  if ( e == 0 )
  {
    Serial.print(F("Receive packet, state "));
    Serial.println(e, DEC);

    for (unsigned int i = 0; i < sx1272.packet_received.length; i++)
    {
      my_packet[i] = (char)sx1272.packet_received.data[i];
    }
    Serial.print(F("Message: "));
    Serial.println(my_packet);
    n = atoi(my_packet);
    if(n>=10&&n<=29){
      switch(n){
          case 11://a 
          irsend.sendLG(0x88C0051, 28);
          break;
          
          case 20: //b
          irsend.sendLG(0x8808541, 28);
          break;
  
          case 21://c
          irsend.sendLG(0x8808642, 28);
          break;
  
          case 22://d
          irsend.sendLG(0x8808743, 28);
          break;

          case 23://d
          irsend.sendLG(0x8808844, 28);
          break;

          case 24://d
          irsend.sendLG(0x8808945, 28);
          break;

          case 25://d
          irsend.sendLG(0x8808A46, 28);
          break;

          case 26://d
          irsend.sendLG(0x8808B47, 28);
          break;

          case 27://d
          irsend.sendLG(0x8808C48, 28);
          break;
      }  
    }   
  }
 delay(100);
 if (millis() - lastSend_r > 300000) { // 5분간격
    // Light
     // Read humidity
    int h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    int t = dht.readTemperature();
    tmp +="11/";
    tmp +="100/";
    tmp += String(h);
    tmp += "/";
    tmp += String(t);
    tmp +="/10";
    tmp += " ";
  
    tmp.toCharArray(message, 20);
    e = sx1272.sendPacketTimeout(8, message);
    tmp = "";
    
    Serial.print(F("Packet sent, state "));
    Serial.println(message);
    message[0] = '\0';
    lastSend_r=millis(); 
  }
  delay(100);
}

