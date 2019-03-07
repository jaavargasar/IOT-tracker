#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <coap_server.h>

#define LED_BUILTIN 1
#define THRESHOLD -70
#define BUZZER_PIN 3

char* ssid = "Andres Iphone";
char* password = "andres18";
String ssid_pa = "TRACKER";
String last = "1";

IPAddress wifiIp(172, 20, 10, 10);
IPAddress wifiNet(255, 255, 255, 240);
IPAddress wifiGW(172, 20, 10, 1);

// CoAP server endpoint url callback
void callback_gps(coapPacket &packet, IPAddress ip, int port, int obs);
coapServer coap;

// CoAP server endpoint URL
void callback_gps(coapPacket *packet, IPAddress ip, int port,int obs) {
  // send response
  char p[packet->payloadlen + 1];
  memcpy(p, packet->payload, packet->payloadlen);
  p[packet->payloadlen] = NULL;
  String message(p);
  //Serial.print("Message=[");
  //Serial.print(p);
  //Serial.println("]\n");
  if(!message.equals("") && !message.equals(last))
  {
    scanForAP();
  }
  
  char val[2];
  message.toCharArray(val, message.length());
  if(obs==1)
    coap.sendResponse(val);
  else
    coap.sendResponse(ip,port,val);
  last = message;
}


void setupWiFi(){
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(wifiIp, wifiGW, wifiNet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

void scanForAP() {
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    if(WiFi.SSID(i) == ssid_pa && WiFi.RSSI(i) > THRESHOLD){
      //Serial.print(" (");
      //Serial.print(WiFi.RSSI(i));
      //Serial.println(")");
      digitalWrite(LED_BUILTIN, HIGH);
      noTone(BUZZER_PIN);
      return;
    }
  }
  tone(BUZZER_PIN, 100);
  
  digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
  //Serial.begin(115200);
  pinMode(BUZZER_PIN, FUNCTION_3);
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);
  setupWiFi();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  coap.server(callback_gps, "gps");
  coap.start();
}

void loop() {
  coap.loop();
  delay(5000);
}
