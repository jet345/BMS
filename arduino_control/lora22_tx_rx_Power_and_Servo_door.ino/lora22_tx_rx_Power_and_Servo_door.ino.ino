#include <arduinoLoRa.h>
#include <arduinoUtils.h>
#include <math.h>
#include <Wire.h>
#include <SPI.h>
#include "ACS712.h"
#include <Servo.h>

unsigned long lastSend_r;
int e;
String tmp ="";
char message[20];
char my_packet[100];
int n;

//Power sensor and Servo
ACS712 sensor(ACS712_30A, A0);
Servo myservo;

int pos = 0;

void setup() {
  //servo and power sensor
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  sensor.calibrate();
  
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
  e |= sx1272.setNodeAddress(22);
  Serial.print(F("Setting node address: state "));
  Serial.println(e, DEC);  
  // Print a success message
  if (e == 0)
    Serial.println(F("SX1272 successfully configured"));
  else
    Serial.println(F("SX1272 initialization failed"));   

}

void loop() {
  int ascii;
   
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
    ascii=my_packet[0];
    //Door
      if( ascii==111 ) { //시리얼 모니터로 O 주면 열림
        Serial.println("open");
        for (pos = 35; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                     // waits 15ms for the servo to reach the position
        }
        delay(2000);//문열려있는시간
        
        Serial.print("wait...");       
        Serial.println("close");
        myservo.write(35); //자동으로 문닫기         
      }    
  }
  delay(100);
  //Serial.print("** mills - lastSend_r : ");
  //Serial.println(millis()-lastSend_r);
  if (millis() - lastSend_r > 300000) { //1분 마다

    //Power sensor
    float U = 230;
    float I = sensor.getCurrentAC();
    float P = U * I;

    tmp += "22/";
    tmp += String(P);
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
