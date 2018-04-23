/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

float temp;

void setup() {
    pinMode(6, OUTPUT);    
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
//    char text[32] = "";
//    radio.read(&text, sizeof(text));
    radio.read(&temp, sizeof(temp));
    Serial.println(temp);
//    if (text == "on") {
      
//      }
//      else{
//        digitalWrite(2, LOW);  
//      }
    }
    else{
          Serial.println("radio not available");
  }
            delay(1000);
                    digitalWrite(6, HIGH);  
            delay(1000);
                    digitalWrite(6, LOW);  
            }
