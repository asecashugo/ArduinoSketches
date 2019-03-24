// Julio el jardinero

#include <BlynkSimpleMKR1000.h>
#include <SimpleTimer.h>

///////////////////////////////////////Sensor temp hum aire DHT22//////////////
#include <DHT.h>
#define DHTPIN 5 //pin gpio 12 in sensor
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
///////////////////////////////////////////////////////////////////////////////

char auth[] = "91ae68f34846462d813ba7c208d46aa2";
char ssid[] = "HugoyMarieT";
char pass[] = "nomejodas";

SimpleTimer timer;

// Definicion de variables
//HMI INPUTS
int manual_on = 0;
BLYNK_WRITE(V0){  int auxV0 = param.asInt();
  manual_on = auxV0;}
int manual_selector = 0;
BLYNK_WRITE(V1){  int auxV1 = param.asInt();
  manual_selector = auxV1;}
int solo_sombra = 0;
BLYNK_WRITE(V2){  int auxV2 = param.asInt();
  solo_sombra = auxV2;}
double cons_lum = 0;
BLYNK_WRITE(V3){  int auxV3 = param.asDouble();
  cons_lum = auxV3;}
double cons_hum_1 = 0;
BLYNK_WRITE(V4){  int auxV4 = param.asDouble();
  cons_hum_1 = auxV4;}
double cons_hum_2 = 0;
BLYNK_WRITE(V5){  int auxV5 = param.asDouble();
  cons_hum_2 = auxV5;}
double cons_hum_3 = 0;
BLYNK_WRITE(V6){  int auxV6 = param.asDouble();
  cons_hum_3 = auxV6;}
//INPUTS
int luminosidad;
float temp_aire = 30;
float hum_aire = 50;
float temp_tierra = 25;
float hum_tierra_1 = 33;
float hum_tierra_2 = 44;
float hum_tierra_3 = 55;
//Internal
bool bomba_auto =0;
bool hum_insuf =0;
bool hum_insuf_1 =0;
bool hum_insuf_2 =0;
bool hum_insuf_3 =0;
bool lum_exces =0;
bool lum_ok=0;
//OUTPUTS
bool bomba =0;

// Definicion de funciones

void sendUptime()
{
  //Lectura entradas
hum_aire = dht.readHumidity();
  temp_aire = dht.readTemperature();
  temp_aire = ((int) (temp_aire * 10) / 10.0);
  hum_aire = ((int) (hum_aire * 10) / 10.0);

}

//////SETUP//////////

void setup()
{
  pinMode(6, OUTPUT);

  digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);
    delay(500);
      digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);
    delay(500);
      digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);
    delay(500);
    
  Serial.print("Connecting...");
  digitalWrite(6, LOW);
  Serial.println("Connecting ahora...");
  Blynk.begin(auth, ssid, pass, "rpihugo.ddns.net",8080);
//  while (Blynk.connect() == false) {}
  digitalWrite(6, HIGH);
  Serial.println("Connected...");
  //DHT
  dht.begin();
  //TIMER
  timer.setInterval(1000, sendUptime);//1000=1s
  
  }


///////LOOP///////////

void loop()
{
  Blynk.run();
  timer.run();


  //evaluacion humedad
hum_insuf_1 = (hum_tierra_1 < cons_hum_1);
hum_insuf_2 = (hum_tierra_2 < cons_hum_2);
hum_insuf_3 = (hum_tierra_3 < cons_hum_3);

//evaluacion luminosidad
lum_exces = luminosidad > cons_lum;

//decision hum
hum_insuf = hum_insuf_1 || hum_insuf_2 || hum_insuf_3;

//decision lum
lum_ok = !solo_sombra || !lum_exces;

//decision auto
bomba_auto = hum_insuf && lum_ok;

//comando final bomba
if (manual_selector == 1) 
{
  bomba = (manual_on == 1);
  }
  else 
  {
    bomba = bomba_auto;
    }

//Escritura salidas
  digitalWrite(4, bomba==1);

//Salidas virtuales Blynk
Blynk.virtualWrite(7, temp_tierra); // virtual pin
Blynk.virtualWrite(8, temp_aire); // virtual pin
Blynk.virtualWrite(9, hum_aire); // virtual pin

Blynk.virtualWrite(10, bomba); // virtual pin

Blynk.virtualWrite(11, hum_tierra_1); // virtual pin
Blynk.virtualWrite(12, hum_tierra_2); // virtual pin
Blynk.virtualWrite(13, hum_tierra_3); // virtual pin

Blynk.virtualWrite(15, luminosidad); // virtual pin

}
