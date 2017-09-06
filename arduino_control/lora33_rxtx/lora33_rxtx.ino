#include <SX1272.h>
#include <math.h>
#include <DHT.h>
#include <Wire.h>
#include <SPI.h>

//IRsend irsend;

#define DHTPIN 4
#define DHTTYPE DHT11
#define LEDPIN 8
#define GASPIN A0
#define LIGHT_LED 9

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastSend_r;
int e;
String tmp ="";
char message[20];
char my_packet[100];
int n;
//light
int BH1750address = 0x23; // i2c Addresse
byte buff[2];

void setup() {
  //Light sensor
  n=0;
  Wire.begin();
  
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
  e |= sx1272.setNodeAddress(33);
  Serial.print(F("Setting node address: state "));
  Serial.println(e, DEC);  
  // Print a success message
  if (e == 0)
    Serial.println(F("SX1272 successfully configured"));
  else
    Serial.println(F("SX1272 initialization failed"));   
    
  //Relay
  pinMode(LEDPIN,OUTPUT); 

  //DHT 
  dht.begin();

  //long
  lastSend_r = 0;

  //light
  pinMode(LIGHT_LED,OUTPUT);
}

//Funtion of Light Sensor
int BH1750_Read(int address) {
    int i=0;
    Wire.beginTransmission(address);
    Wire.requestFrom(address, 2);
    while(Wire.available()) {
      buff[i] = Wire.read(); //receive one byte
      i++;
    }
    Wire.endTransmission();
    return i;
}

void BH1750_Init(int address) {
      Wire.beginTransmission(address);
      Wire.write(0x10);
      Wire.endTransmission();
}

void Light_Sensor() {
  // Light sensor
  int i;
  uint16_t lux = 0;
  BH1750_Init(BH1750address);

  if(2==BH1750_Read(BH1750address)) {
    lux = ((buff[0]<<8)|buff[1])/1.2;
    
    if(lux>100) {
      tmp += "*";
      tmp += "/";  
    }
    else{
      tmp += "33";
      tmp += "/";
      tmp += String(lux);
      tmp += "/";      
    }
  }
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
    ascii = my_packet[0];
    if(ascii>=101 && ascii<=104){
        switch(ascii){
            case 101:
            //e , OFF
            analogWrite(LIGHT_LED,0);
            break;
            case 102:
            //f 1단
            analogWrite(LIGHT_LED,100);
            break;

            case 103:
            //g 2단
            analogWrite(LIGHT_LED,180);
            break;

            case 104:
            //h 3단
            analogWrite(LIGHT_LED,255);
            break;
        }
      }
    
  }
  delay(100);
  Serial.print("** mills - lastSend_r : ");
  Serial.println(millis()-lastSend_r);
  if (millis() - lastSend_r > 60000) { //3초간격?
    // Light
    Serial.println("HERE");
    Light_Sensor();
     // Read humidity
    int h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    int t = dht.readTemperature();
    // Reading gas sensor
    int g = analogRead(GASPIN);
    
    tmp += String(h);
    tmp += "/";
    tmp += String(t);
    tmp += "/";
    tmp += String(g);
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
