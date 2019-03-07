/*
ESP-COAP Client
*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "coap_client.h"

//Server communication
const char CR = 13;
const char* server = "http://172.20.10.3:10010/device";
boolean stringComplete;
String inputString;

//instance for coapclient
coapClient coap;

int cur_cnt = 0;
//WiFi connection info
char* ssid = "Andres Iphone";
char* password = "andres18";

//AP connection info
const char *ssid_ap = "TRACKER";
const char *password_ap = "qwedsA!1";

//ip address and default port of coap server in which your interested in
IPAddress ip(172, 20, 10, 10);
int port = 5683;

// coap client response callback
void callback_response(coapPacket &packet, IPAddress ip, int port);

// coap client response callback
void callback_response(coapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;

  //response from coap server
  if(packet.type==3 && packet.code==0){
    Serial.println("ping ok");
  }
  Serial.println(p);
}

void setupWiFi(){
  WiFi.begin(ssid, password);
  // Connection info to WiFi network
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    //delay(500);
    yield();
  }
  //Serial.println("WiFi connected");
  //Serial.println(WiFi.localIP());  
}

void setupAP(){
  //Serial.print("Configuring access point...");
  WiFi.softAP(ssid_ap, password_ap);
  IPAddress myIP = WiFi.softAPIP();
  //Serial.print("AP IP address: ");
  //Serial.println(myIP);  
}

void sendCoapMessage(){
  bool state;
  char message[2];
  itoa(cur_cnt, message, 10);
  int msgid =coap.post(ip,port,"gps",message,strlen(message));
  state = coap.loop();
  //Serial.print("state=");
  //Serial.println(state);
  //Serial.print("msgid=");
  //Serial.println(msgid);
  cur_cnt = 1 - cur_cnt;
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if(inChar == '\n') inputString = "";
    else if(inChar != CR) inputString += inChar;
    if (inChar == CR) stringComplete = true;
  }
}

void setup() {
  Serial.begin(9600);
  stringComplete = false;
  inputString = "";
  setupWiFi();
  setupAP();
  // client response callback.
  // this endpoint is single callback.
  coap.response(callback_response);
  // start coap client
  coap.start();
}

void loop() {
  serialEvent();
  if( stringComplete ){
    Serial.print("inputString: ");
    Serial.println(inputString);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println( "Connection is OK" );
      if(inputString.indexOf("error") < 0){
        HTTPClient http;
        http.begin(server);
        http.addHeader("Content-Type", "application/json");
        //int httpCode = http.POST("irradiance_measure[time]=2017-11-01%2004%3A00%3A00&irradiance_measure[measure]=0.4224&irradiance_measure[device_id]=1");
        //{"latitude":4.632011,"longitude":-74.081635,"date":"2019-3-6-2:47:43"}
        int httpCode = http.POST(inputString);
        Serial.print("httpCode: ");
        Serial.println(httpCode);
        if( httpCode > 0 ) http.writeToStream(&Serial);
        else Serial.println(http.errorToString(httpCode).c_str());
        Serial.println();
        http.end();
      }
      sendCoapMessage();
    }
    else{
      Serial.println( "No connection" );  
    }
    inputString = "";
    stringComplete = false;
  }
}
