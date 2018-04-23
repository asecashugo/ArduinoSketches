
//#define BLYNK_PRINT Serial    // Comment this out to     disable prints and save space
#include <Bridge.h>
#include <YunClient.h>
#include <YunServer.h>
#include <BlynkSimpleYun.h>
    char auth[] = "1f3215deb5f54483976990985bdc50c5";
    #include <DHT.h>
    #define DHTPIN 5 //pin gpio 12 in sensor
    #define DHTTYPE DHT22  
    DHT dht(DHTPIN, DHTTYPE);
    #include <SimpleTimer.h>
    SimpleTimer timer;

bool salida;
int salida_man, salida_auto, auto_mode;
double consigna, hist;


void sendUptime()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  t = ((int) (t * 10) / 10.0);
  h = ((int) (h * 10) / 10.0);
  
  Blynk.virtualWrite(8, t); // virtual pin
  Blynk.virtualWrite(9, h); // virtual pin

  Blynk.virtualWrite(10, salida); // virtual pin
  Blynk.virtualWrite(11, salida_auto); // virtual pin
  Blynk.virtualWrite(12, salida_man); // virtual pin

if ((salida_auto == 0) && (t < consigna - hist))
{
  salida_auto = 1;
}
else if ((salida_auto == 1) && (t > consigna))
{
  salida_auto = 0;
}

if (auto_mode == 1) 
{
  salida = (salida_auto==1);
  }
  else 
  {
    salida = (salida_man==1);
    }

  digitalWrite(2,salida);  

}


BLYNK_WRITE(V0)
{
  int salida_man = param.asInt(); // assigning incoming value from pin V1 to a variable
}

BLYNK_WRITE(V1)
{
  int auto_mode = param.asInt(); // assigning incoming value from pin V1 to a variable
}

BLYNK_WRITE(V2)
{
  int consigna = param.asDouble(); // assigning incoming value from pin V1 to a variable
}

BLYNK_WRITE(V3)
{
  int hist = param.asDouble(); // assigning incoming value from pin V1 to a variable
}





void setup()
{
  pinMode(2, OUTPUT);
  digitalWrite(2,LOW);
  Blynk.begin(auth);
  dht.begin(); 
  timer.setInterval(10000, sendUptime);//1000=1s
  hist=5;
  salida_man=0;

}

void loop() {
  Blynk.run();
  timer.run();

}
