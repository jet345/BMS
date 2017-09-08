#include <arduinoLoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>

unsigned long lastSend_r;
unsigned long lastSend_i;
int e;
String tmp ="";
char my_packet[20];


Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  
  //LoRa
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // Print a start message
  Serial.println(F("SX1272 module and Arduino: receive packets without ACK"));

  // Power ON the module
  e = sx1272.ON();
  Serial.print(F("Setting power ON: state "));
  Serial.println(e, DEC);
  
  // Set transmission mode and print the result
  e = sx1272.setMode(4);
  Serial.print(F("Setting Mode: state "));
  Serial.println(e, DEC);
  
  // Set header
  e = sx1272.setHeaderON();
  Serial.print(F("Setting Header ON: state "));
  Serial.println(e, DEC);
  
  // Select frequency channel
  e = sx1272.setChannel(CH_10_868);
  Serial.print(F("Setting Channel: state "));
  Serial.println(e, DEC);
  
  // Set CRC
  e = sx1272.setCRC_ON();
  Serial.print(F("Setting CRC ON: state "));
  Serial.println(e, DEC);
  
  // Select output power (Max, High or Low)
  e = sx1272.setPower('H');
  Serial.print(F("Setting Power: state "));
  Serial.println(e, DEC);
  
  // Set the node address and print the result
  e = sx1272.setNodeAddress(8);
  Serial.print(F("Setting node address: state "));
  Serial.println(e, DEC);
  
  // Print a success message
  Serial.println(F("SX1272 successfully configured"));
  Serial.println();

}

void loop() {

   // Receive message
  e = sx1272.receivePacketTimeout(10000);
  if ( e == 0 )
  {
    Serial.print(F("Receive packet, state "));
    Serial.println(e, DEC);

    for (unsigned int i = 0; i < sx1272.packet_received.length; i++)
    {
      my_packet[i] = (char)sx1272.packet_received.data[i];

      //servo 
     if(my_packet=='o') { //시리얼 모니터로 O 주면 열림
        Serial.println("open");
        for (pos = 35; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                     // waits 15ms for the servo to reach the position
        }
        delay(2000);//문열려있는시간
        
        Serial.print("wait...");/*
        for (pos = 180; pos <= 0; pos -= 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }*/
        Serial.println("close");
        myservo.write(35); //자동으로 문닫기         
      }/*
      else if(my_packet=='X'){ //X주면 위험알려줌
          Serial.println("DANGER");
          delay(2000);
      }*/
    }
    Serial.print(F("Message: "));
    Serial.println(my_packet);
  }
  else {
    Serial.print(F("Receive packet, state "));
    Serial.println(e, DEC);
  }
    
}
