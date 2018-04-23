/*
  Emoncms sensor client with Strings 
 
 This sketch connects an analog sensor to Emoncms,
 using a ms14 with Yún + Lenonard. 
 
 created 15 March 2010
 updated 27 May 2013
 by Tom Igoe
   
 updated 6, Oct, 2013 to support Emoncms
 by Edwin Chen
   
 */



// include all Libraries needed:
#include <Process.h>
#include <Bridge.h>
#include "EmonLib.h"
#include <Console.h>

#include <YunClient.h>
#include <YunServer.h>
#include <BlynkSimpleYun.h>

char auth[] = "1f3215deb5f54483976990985bdc50c5"; // token para Wifistato


EnergyMonitor cons;
EnergyMonitor gen;
EnergyMonitor testvolt;

// set up net client info:
const unsigned long postingInterval = 2000;  //delay between updates to emoncms.com
unsigned long lastRequest = 0;      // when you last made a request
String dataString = "";

//WifiSignal
char c2[4] = {'1','2','3','4'};

long intervalo1 = 5000;
long intervalo2 = 500;

long tiempo1 = 0;
long tiempo2 = 0;

int status_wifi = 0;

void setup() {
  // start serial port:
  Bridge.begin();
  Console.begin();

Blynk.begin(auth);

  //while(!Serial);    // wait for Network Serial to open
  Console.println("Emoncms client");

  cons.current(1, 60.6); 
  cons.voltage(5, -220*240/259*240/58, 1.1);  // Voltage: input pin, calibration, phase_shift 
  gen.current(2, 60.6);  
  gen.voltage(5, -220*240/259*240/58, 1.1);  // Voltage: input pin, calibration, phase_shift 
  
  testvolt.current(1, 60.6);
  testvolt.voltage(5, 234.26*240/13, 1.7);  // Voltage: input pin, calibration, phase_shift 

  
  
  // Do a first update immediately
  updateData();
  sendData();
  lastRequest = millis();
}

void loop() {
  
  Blynk.run();
  
  // get a timestamp so you can calculate reading and sending intervals:
  long now = millis();




  // if the sending interval has passed since your
  // last connection, then connect again and send data:
  if (now - lastRequest >= postingInterval) {
    updateData();
    sendData();

updateBlynk();
    
    lastRequest = now;
  }
}

void updateData() {


double corrA = 0.16; //corrección Amperios

cons.calcVI(20,2000);
gen.calcVI(20,2000); 
testvolt.calcVI(20,2000); 


    float realPower       = cons.realPower;        //extract Real Power into variable
    float apparentPower   = cons.apparentPower;    //extract Apparent Power into variable
    float powerFactor     = cons.powerFactor;      //extract Power Factor into Variable
    float supplyVoltage   = cons.Vrms;             //extract Vrms into Variable
    float Irms            = cons.Irms;             //extract Irms into Variable
    
    float realPower2       = gen.realPower;        //extract Real Power into variable
    float apparentPower2   = gen.apparentPower;    //extract Apparent Power into variable
    float powerFactor2     = gen.powerFactor;      //extract Power Factor into Variable
    float supplyVoltage2   = gen.Vrms;             //extract Vrms into Variable
    float Irms2            = gen.Irms;             //extract Irms into Variable

    double testVoltaje   = testvolt.Vrms;             //extract Vrms into Variable


double consumo = realPower;
double consumo_Irms = Irms;
double consumo_pf = powerFactor;

double generacion = realPower2;
double generacion_Irms = Irms2;
double generacion_pf = powerFactor2;

double contador = consumo - generacion;

double voltaje = supplyVoltage;



  
//  double consumo = 240.0*cons.calcIrms(1480)-54;
//  double generacion = 240.0*gen.calcIrms(1480)-54;
//  double contador = consumo - generacion;
  
  dataString = "Consumo_w:";
  dataString += consumo;
  // add generacion:
  dataString += ",Generacion_w:";
  dataString += generacion;
  // add contador:
  dataString += ",Contador_w:";
  dataString += contador;
  // add voltaje:
  dataString += ",Voltaje_V:";
  dataString += voltaje;
  // add consumo Irms:
  dataString += ",Consumo_A:";
  dataString += consumo_Irms;
  // add generacion Irms:
  dataString += ",Generacion_A:";
  dataString += generacion_Irms;
  // add consumo pf:
  dataString += ",Consumo_pf:";
  dataString += consumo_pf;
  // add generacion pf:
  dataString += ",Generacion_pf:";
  dataString += generacion_pf;
   // test:
  dataString += ",Test:";
  dataString += "1";
  
}

// this method makes a HTTP connection to the server:
void sendData() {
  // form the string for the API header parameter:
  String apiString = "a6724324477c9f5e07bb8390392cc71c";
  // apiString += APIKEY;

  // form the string for the URL parameter:
  String url = "http://emoncms.org/api/post?";
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
  Console.print("\n\nSending data CURL... ");
  Console.println(url);  
  emoncms.begin("curl");
  emoncms.addParameter("-g");
  emoncms.addParameter(url);
  emoncms.run();



  // If there's incoming data from the net connection,
  // send it out the Serial:
  while (emoncms.available()>0) {
    char c = emoncms.read();
    Console.write(c);
  }

}

//process wifi signal
void runProcess() {
  
  Process p;        // Create a process and call it "p"
  p.runShellCommand("lua /root/bin/lua/pretty-wifi-info.lua");  // use CLI commands just like 
                                                                      //inside linux console
  int i=0;
  while (p.available()>0) {
    char c = p.read();
    c2[i] = c;
    i=i+1;
  }
  String c3=c2;
  int a=c3.toInt();   //convert String to int

  if (a == 0){         //check if signal is 0%, singal strength goes from 0 to 100%
    status_wifi = 0;  //wifi signal is lost
  }
  else 
    status_wifi = 1; //wifi signal is ok
}

void updateBlynk() {


}

