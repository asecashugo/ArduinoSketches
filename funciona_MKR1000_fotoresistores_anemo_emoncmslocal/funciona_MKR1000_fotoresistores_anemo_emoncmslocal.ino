//V[m/s]=0.767*f[Hz]+0.39
//viento máximo ~8m/s

//troubleshooting wifi
#include <SPI.h>
#include <WiFi101.h>
WiFiClient client;
int status = WL_IDLE_STATUS;
//Emoncms configurations
char server[] = "rpihugo.ddns.net";
unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop


#include <BlynkSimpleMKR1000.h>
#include <SimpleTimer.h>

char auth[] = "e22ff0cae49b72a2901811989be7eba0"; //local
char apikey[] = "a6724324477c9f5e07bb8390392cc71c"; //remote
char ssid[] = "HugoyMarieT_ext";
char pass[] = "nomejodas";

// include all Libraries needed:
#include <Process.h>
#include <Bridge.h>
#include "EmonLib.h"

const unsigned long postingInterval = 10000;  //delay between updates to emoncms.com
unsigned long lastRequest = 0;      // when you last made a request
String dataString = "";


//ANEMO

// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.
const int numReadings = 50;

bool state = LOW;
unsigned long lastReading = 0;
unsigned long lastReading1min = 0;
unsigned long lastTic = 0;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
double total = 0;                  // the running total
double average = 0;
int A0old = 0;
int delta = 0;
int A0new = 0;
int average_old = 0;
float freq = 0;
double freq1min = 0;
double vel = 0;
double potaerosim = 0;
int vref = 1020;
int hyst = 2;
int counterHIGH = 0;
int counterLOW = 0;
double periodo = 10000; //periodo en ms

int tic = 0; //tics del anemo
unsigned long tic1min = 0; //tics del anemo
unsigned long tic1min_contador = 0;

int inputPin = A4;

double runningAverage(double M) {
#define LM_SIZE 10
  static int LM[LM_SIZE];      // LastMeasurements
  static byte index = 0;
  static long sum = 0;
  static byte count = 0;

  // keep sum updated to improve speed.
  sum -= LM[index];
  LM[index] = M;
  sum += LM[index];
  index++;
  index = index % LM_SIZE;
  if (count < LM_SIZE) count++;
  return sum / count;
}

//fin ANEMO


void setup()
{

  //3.3v for anemo
  pinMode(3, OUTPUT);
  analogWrite(3, 128);

  //test

  pinMode(6, OUTPUT);

  digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);
  delay(500);
  digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);
  delay(500);

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();

  digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);
  delay(500);



  // start serial port:
  //  Bridge.begin();
  //  Serial.begin(9600);

  //ANEMO
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  //fin ANEMO


  // Do a first update immediately
  updateData(freq, freq1min);
  sendDataLocal2();
  lastRequest = millis();
}

void loop()
{

  //EMONCMS

  // get a timestamp so you can calculate reading and sending intervals:
  long now = millis();

  // if the sending interval has passed since your
  // last connection, then connect again and send data:
  if (now - lastRequest >= postingInterval) {
    updateData(freq, freq1min);
    sendDataLocal2();
    lastRequest = now;
  }

  now = millis();


  //ANEMO

  A0new = analogRead(inputPin);
  delta = A0new - vref;



  if ((state == LOW) && (delta > hyst)) {
    state = HIGH;
  }

  else if ((state == HIGH) && (delta < -hyst)) {
    state = LOW;
    tic++;
    tic1min_contador++;
    periodo = now - lastTic;
    lastTic = now;
  }


  //if (A0new >= A0old) {
  //  counterHIGH++;
  //}
  //if (A0new <= A0old) {
  //  counterLOW++;
  //}
  //if (counterHIGH >= numReadings) {
  //  counterLOW = 0;
  //}
  //if (counterLOW >= numReadings) {
  //  counterHIGH = 0;
  //}

  if (now - lastReading1min >= 60000) {
    tic1min = tic1min_contador;
    //curva caracteristica anemo
    vel = 0.767 * ((double)tic1min / 60) + 0.39;
    //Simulación Aero 500W
    potaerosim = 10 * vel + 1.7 * vel * vel;
    potaerosim = min (potaerosim, 500);
    potaerosim = max(potaerosim, 0);
    if (vel < 2) {
      potaerosim = 0;
    }
    if (vel > 16) {
      potaerosim = 0;
    }
    freq1min = tic1min_contador / 60;
    tic1min_contador = 0;
    lastReading1min = now;
  }

  if (now - lastReading >= 1000) {
    //freq = tic;
    tic = 0;
    lastReading = now;
  }

  A0old = A0new;

  digitalWrite(6, state);

  //SMOOTHENING

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = periodo;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits

  freq = 1000 / average;
  Serial.println(periodo);

  delay(1);

  //fin ANEMO

}


void updateData(double freq, double freq1min) {
  // convert the readings to a String to send it:

  const long A = 1000;     //Resistencia en oscuridad en KΩ
  const int B = 15;        //Resistencia a la luz (10 Lux) en KΩ
  const int Rc = 12;       //Resistencia calibracion en KΩ
  const int LDRPin = A0;   //Pin del LDR

  int Volt0, Volt1, Volt2, Volt3;
  int oeste, sur, este, norte, elevacion, azimut, irrad;
  long desv;

  Volt0 = analogRead(3);  // oeste naranja
  Volt1 = analogRead(2);  // sur amarillo
  Volt2 = analogRead(1);  // este verde
  Volt3 = analogRead(0);  // norte azul/marron


  //ilum = ((long)(1024-V)*A*10)/((long)B*Rc*V);  //usar si LDR entre GND y A0
  oeste = ((long)Volt0 * A * 10) / ((long)B * Rc * (1024 - Volt0)); //usar si LDR entre A0 y Vcc (como en el esquema anterior)
  sur = ((long)Volt1 * A * 10) / ((long)B * Rc * (1024 - Volt1)); //usar si LDR entre A0 y Vcc (como en el esquema anterior)
  este = ((long)Volt2 * A * 10) / ((long)B * Rc * (1024 - Volt2)); //usar si LDR entre A0 y Vcc (como en el esquema anterior)
  norte = ((long)Volt3 * A * 10) / ((long)B * Rc * (1024 - Volt3)); //usar si LDR entre A0 y Vcc (como en el esquema anterior)

  irrad = (norte + este + sur + oeste) / 4;

  elevacion = sur - norte;
  azimut = este - oeste;

  desv = (abs(elevacion) + abs(azimut)) / 2;

  // datos solar
  dataString = "Norte_lux:";
  dataString += norte;
  dataString += ",Este_lux:";
  dataString += este;
  dataString += ",Sur_lux:";
  dataString += sur;
  dataString += ",Oeste_lux:";
  dataString += oeste;
  dataString += ",Irradiacion_lux:";
  dataString += irrad;
  //datos anemo
  dataString += ",Freq_Hz:";
  dataString += freq;
  dataString += ",Tic1min:";
  dataString += tic1min;
  dataString += ",Vel1min_ms:";
  dataString += vel;
  dataString += ",PotAero500sim_w:";
  dataString += potaerosim;

  //  dataString += ",Elevacion_perc:";
  //  dataString += elevacion/80;
  //  dataString += ",Azimut_perc:";
  //  dataString += azimut/80;




}

// this method makes a HTTP connection to the server:
void sendDataLocal() {
  // form the string for the API header parameter:
  String apiString = "e22ff0cae49b72a2901811989be7eba0";
  // apiString += APIKEY;

  // form the string for the URL parameter:
  String url = "http://rpihugo.ddns.net/emoncms/input/post?";
  url += "node=MKR1000_Terrado&";
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
  Serial.print("\n\nSending data CURL... ");
  Serial.println(url);
  //  Serial.println(analogRead(A4));
  emoncms.begin("curl");
  emoncms.addParameter("-g");
  emoncms.addParameter(url);
  emoncms.run();



  // If there's incoming data from the net connection,
  // send it out the Serial:
  while (emoncms.available() > 0) {
    char c = emoncms.read();
    Serial.write(c);
  }

}

void sendDataLocal2() {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");
    // send the HTTP GET request:
    client.print("GET /emoncms/input/post?apikey=");
    client.print(auth);
    client.print("&node=MKR1000_Terrado");
    client.print("&json={");

    client.print(dataString);

    client.println("} HTTP/1.1");
    client.println("Host:rpihugo.ddns.net");
    client.println("User-Agent: Arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
    Serial.println(lastConnectionTime);
  }
  else {
    // if you couldn't make a connection:
    Serial.println("Connection failed");
    Serial.println("Disconnecting...");
    client.stop();
  }

}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
