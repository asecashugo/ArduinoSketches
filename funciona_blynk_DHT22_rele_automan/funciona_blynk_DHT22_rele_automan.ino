
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

WidgetLED led1(V5);

bool salida;
int ver, salida_man, salida_auto, auto_mode;
double consigna, hist, delta;
float t, h;


void sendUptime()
{
    Serial.write("empezando funcion");
  h = dht.readHumidity();
  t = dht.readTemperature();
  t = ((int) (t * 10) / 10.0);
  h = ((int) (h * 10) / 10.0);

  digitalWrite(2,salida);  

        Serial.write("salida = ");
Serial.println(salida);
          Serial.write("salida_man = ");
Serial.println(salida_man);
              Serial.write("salida_auto = ");
Serial.println(salida_auto);
                  Serial.write("auto_mode = ");
  Serial.println(auto_mode);

  Blynk.virtualWrite(3, hist); // virtual pin
  Blynk.virtualWrite(4, ver); // virtual pin
  

  Blynk.virtualWrite(5, consigna); // virtual pin
  Blynk.virtualWrite(7, delta); // virtual pin

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
  consigna = auxV2/10.0; //para dar un decimal...
}





void setup()
{
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  digitalWrite(2,LOW);
  Blynk.begin(auth);
  dht.begin(); 
  timer.setInterval(1000, sendUptime);//1000=1s
  hist=1;
  salida_man=0;

//  Version
  ver = 103;

}

void loop() {

  Blynk.run();
  timer.run();

  delta = consigna - t;

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

if (salida == 1) 
{
  led1.on();
  Blynk.virtualWrite(6, 255); // virtual pin
  }
  else 
  {
    Blynk.virtualWrite(6, 0); // virtual pin
    }

}
