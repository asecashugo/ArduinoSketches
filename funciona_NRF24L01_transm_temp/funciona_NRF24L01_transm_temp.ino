
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Incluimos librería
#include <DHT.h>
 
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 2
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11
 
// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

float data;

void setup() {
    dht.begin();

  
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}
void loop() {
data = dht.readTemperature();
//  const char text[] = "on";
  radio.write(&data, sizeof(data));
  delay(100);
}
