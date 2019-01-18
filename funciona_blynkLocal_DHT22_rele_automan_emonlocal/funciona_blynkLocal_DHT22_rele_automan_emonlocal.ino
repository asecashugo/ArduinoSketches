
//#define BLYNK_PRINT Serial    // Comment this out to     disable prints and save space
#include <Bridge.h>
#include <YunClient.h>
#include <YunServer.h>
#include <BlynkSimpleYun.h>
char auth[] = "8e855ca4186d42ac906de1955d027cfd";
char ssid[] = "HugoyMarieT";
char pass[] = "nomejodas";
#include <DHT.h>
#define DHTPIN 5 //pin gpio 12 in sensor
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#include <SimpleTimer.h>
SimpleTimer timer;

WidgetLED led1(V5);

bool salida;
int salida_man, salida_auto, auto_mode;
double consigna, hist;
float t, h;


#include <Process.h>
#include "EmonLib.h"

EnergyMonitor cons;
EnergyMonitor gen;

// set up net client info:
const unsigned long postingInterval = 1000;  //delay between updates to emoncms.com
unsigned long lastRequest = 0;      // when you last made a request
String dataString = "";




void sendUptime()
{
  Serial.write("empezando funcion");
  h = dht.readHumidity();
  t = dht.readTemperature();
  t = ((int) (t * 10) / 10.0);
  h = ((int) (h * 10) / 10.0);

  digitalWrite(2, salida);

  Serial.write("salida = ");
  Serial.println(salida);
  Serial.write("salida_man = ");
  Serial.println(salida_man);
  Serial.write("salida_auto = ");
  Serial.println(salida_auto);
  Serial.write("auto_mode = ");
  Serial.println(auto_mode);

  Blynk.virtualWrite(8, t); // virtual pin
  Blynk.virtualWrite(9, h); // virtual pin

  Blynk.virtualWrite(10, salida); // virtual pin
  Blynk.virtualWrite(11, salida_auto); // virtual pin
  Blynk.virtualWrite(12, salida_man); // virtual pin

}


BLYNK_WRITE(V0)
{
  int auxV0 = param.asInt(); // assigning incoming value from pin V1 to a variable
  salida_man = auxV0;
}

BLYNK_WRITE(V1)
{
  int auxV1 = param.asInt(); // assigning incoming value from pin V1 to a variable
  auto_mode = auxV1;
}

BLYNK_WRITE(V2)
{
  int auxV2 = param.asDouble(); // assigning incoming value from pin V1 to a variable
  consigna = auxV2;
}

//BLYNK_WRITE(V3)
//{
//  int auxV3 = param.asDouble(); // assigning incoming value from pin V1 to a variable
//  hist = auxV3;
//}





void setup()
{

  Bridge.begin();

  Serial.begin(9600);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  Serial.write("Blynk.begin...");
  Blynk.begin(auth, "rpihugo.ddns.net", 8080);
  Serial.write("Blynk.begin hecho");
  dht.begin();
  timer.setInterval(1000, sendUptime);//1000=1s
  hist = 2;
  salida_man = 0;

  updateData();
  sendDataLocal();
  lastRequest = millis();

}

void loop() {

  Blynk.run();
  timer.run();

  if ((salida_auto == 0) && (t < consigna - 2))
  {
    salida_auto = 1;
  }
  else if ((salida_auto == 1) && (t > consigna))
  {
    salida_auto = 0;
  }

  if (auto_mode == 1)
  {
    salida = (salida_auto == 1);
  }
  else
  {
    salida = (salida_man == 1);
  }

  if (salida == 1)
  {
    led1.on();
    Blynk.virtualWrite(6, 255); // virtual pin
  }
  else
  {
    Blynk.virtualWrite(6, 0); // virtual pin
  }


  long now = millis();




  // if the sending interval has passed since your
  // last connection, then connect again and send data:
  if (now - lastRequest >= postingInterval) {
    updateData();
    sendDataLocal();
    lastRequest = now;

  }
}


void updateData() {
  // convert the readings to a String to send it:
  double temp_int = t;
  dataString = "Temp_int:";
  dataString += temp_int;
}



void sendDataLocal() {
  // form the string for the API header parameter:
  String apiString = "e22ff0cae49b72a2901811989be7eba0";
  // apiString += APIKEY;

  // form the string for the URL parameter:
  String url = "http://rpihugo.ddns.net/emoncms/input/post?";
  url += "node=Yun_Caldera&";
  url += "json={";
  url += dataString;
  url += "}&";
  url += "apikey=";
  url += apiString;
  // Send the HTTP PUT request

  // Is better to declare the Process here, so when the
  // sendData function finishes the resources are immediately
  // released. Declaring it global works too, BTW.
  Process emoncms;
  Console.print("\n\nSending data local CURL... ");
  Console.println(url);
  emoncms.begin("curl");
  emoncms.addParameter("-g");
  emoncms.addParameter(url);
  emoncms.run();



  // If there's incoming data from the net connection,
  // send it out the Console:
  while (emoncms.available() > 0) {
    char c = emoncms.read();
    Console.write(c);
  }

}

