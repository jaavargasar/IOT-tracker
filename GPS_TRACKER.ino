

#include <SoftwareSerial.h>

#include <TinyGPS.h>


TinyGPS gps;
SoftwareSerial ss(4, 3); // tx rx
SoftwareSerial esp(10,11); // tx rx
String json = "";
String inputString="";
bool stringComplete=false;
char CR = 13;
char jsonTest[100]="{\"latitude\":4.632011,\"longitude\":-74.081635,\"date\":\"2019-3-6-2:47:43\"}\r\n";
char bufferJson[100];
void setup()
{
  Serial.begin(115200);
  esp.begin(9600);
  ss.begin(9600);
  delay(2000);
}

void loop()
{
  serialEvent();
  if(stringComplete){
    Serial.print("wifi found");
    Serial.print(inputString);
    stringComplete=false;
    inputString="";
  }
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  json="";
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      if (gps.encode(c))
        newData = true;
    }
  }

  if (newData)
  {
    
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    int year;
    byte month, day, hour, minutes, seconds, hundredths;
    unsigned long fix_age;
    gps.crack_datetime(&year, &month, &day,
    &hour, &minutes, &seconds, &hundredths, &fix_age);

    json.concat("{");
    json.concat("\"latitude\":");
    json.concat(String(flat,6));
    json.concat(",");
    json.concat("\"longitude\":");
    json.concat(String(flon,6));
    json.concat(",");
    json.concat("\"date\":");
    String date ='"'+String(year) + '-' + String(month) + '-' + String(day)+ '-' + String(hour) + ':' + String( minutes )+ ':' +  String(seconds)+ '"';
    json.concat(date);
    json.concat("}");
    json.concat('\r');
    json.concat('\n');
    
  }
  else{
      json.concat("{");
      json.concat("\"error\": \"no valid data to encode\" ");
      json.concat("}");
      json.concat('\r');
      json.concat('\n');
  }

  
  gps.stats(&chars, &sentences, &failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
  //Serial.println(json);
  
  json.toCharArray(bufferJson,json.length());
  //esp.write(json);
  //Serial.print("BUFFER");
  Serial.println(bufferJson);
  
  esp.write(bufferJson);
  //esp.write(jsonTest);

  
  
  delay(5000);
}

void serialEvent() {
  Serial.print("Serial Event");
  while (esp.available()) {
    
  Serial.println("Serial while");
    char inChar = (char)esp.read();
    if(inChar == '\n') inputString = "";
    else if(inChar != CR) inputString += inChar;
    if (inChar == CR) stringComplete = true;
  }
}
