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

EnergyMonitor cons;
EnergyMonitor gen;

// set up net client info:
const unsigned long postingInterval = 1000;  //delay between updates to emoncms.com
unsigned long lastRequest = 0;      // when you last made a request
String dataString = "";

void setup() {
  // start serial port:
  Bridge.begin();
  Console.begin();
 

  //while(!Console);    // wait for Network Console to open
  Console.println("Emoncms client");

  cons.current(1, 60.6);  
  gen.current(2, 60.6);  
  
  
  // Do a first update immediately
  updateData();
  sendDataRemoto();
    sendDataLocal();
  lastRequest = millis();
}

void loop() {
  // get a timestamp so you can calculate reading and sending intervals:
  long now = millis();




  // if the sending interval has passed since your
  // last connection, then connect again and send data:
  if (now - lastRequest >= postingInterval) {
    updateData();
    sendDataRemoto();
    sendDataLocal();
    lastRequest = now;
  }
}

void updateData() {
  // convert the readings to a String to send it:
  double consumo = 240.0*cons.calcIrms(1480)-54;
  double generacion = 240.0*gen.calcIrms(1480)-54;
  double contador = consumo - generacion;
  dataString = "Consumo_w:";
  dataString += consumo;
  // add generacion:
  dataString += ",Generacion_w:";
  dataString += generacion;
  // add contador:
  dataString += ",Contador_w:";
  dataString += contador;
}

// this method makes a HTTP connection to the server:
void sendDataRemoto() {
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
  // send it out the Console:
  while (emoncms.available()>0) {
    char c = emoncms.read();
    Console.write(c);
  }

}

void sendDataLocal() {
  // form the string for the API header parameter:
  String apiString = "e22ff0cae49b72a2901811989be7eba0";
  // apiString += APIKEY;

  // form the string for the URL parameter:
  String url = "http://rpihugo.ddns.net/emoncms/input/post?";
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
  while (emoncms.available()>0) {
    char c = emoncms.read();
    Console.write(c);
  }

}


