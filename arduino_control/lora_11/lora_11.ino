#include <SX1272.h>
/*-----------------------------------------------------------------------------------------------------
* Test de la transmission LoRa sans multiprotocol shield
* ----------------------------------------------------------------------------------------------------*/

#include <Wire.h>
#include <SPI.h>

int e;
int a;
const int gas_pin =A0;
char message1 [] = "qwertyuiopasdfghjklzvxbcnvmmm@ ";
char message2 [] = "Packet 2, broadcast test";
String tmp="";
char my_packet[100];
int n;
void setup()
{
 n=0;
 Serial.begin(9600);
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
}

void loop()
{
  n++;
  tmp+=analogRead(gas_pin);
  tmp+=" ";
  tmp.toCharArray(message2,10);
  tmp="";
  // Send message1 and print the result
  e = sx1272.sendPacketTimeout(8, message2);
  Serial.print(F("Packet1 sent, state "));
  Serial.println(e, DEC);
  Serial.print("gaspin = ");
  Serial.println(message2);
  //
  message2[0]='\0';
  delay(3000*n-2000);
  
  
  /*e = sx1272.receivePacketTimeout(10000);
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
  }
  else {
    Serial.print(F("Receive packet, state "));
    Serial.println(e, DEC);
  }*/

 
}
