#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "Bruno Migliorini"
#define STAPSK  "edicula88"
#endif

char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Minha Localização</title>
</head>
<body>
    <h1>Where is my SSi</h1>
    <p>
        latitude:<span id="lat"></span><br />
        longitude:<span id="lon"></span>
    </p>
    <script>
        const api_url = "sensores";
        async function getISS(){
            const response = await fetch(api_url);
            const data = await response.json();
            const { temperatura , amonia} = data;
            
            document.getElementById("lat").textContent = temperatura;
            document.getElementById("lon").textContent = amonia;
        }
        setInterval(getISS(), 1000);    
    </script>
</body>
</html>
)=====";

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

char jsonOutput[128];

const int led = 13;

int amoniaAr = 1;
int temperaturaAtual=20; 

String  ledState = "OFF";

void handleRoot(){ 
  if ( server.hasArg("LED") ) {
    handleSubmit();
  } else {
    server.send ( 200, "text/html", getPage() );
  }  
}
void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}


void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/",[](){server.send_P(200,"text/html", webpage);});
  server.on("/sensores", [](), get_sensores);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}
void loop(void) {
  server.handleClient();
  MDNS.update();
  
}



void handleSubmit() {

  String LEDValue;
  LEDValue = server.arg("LED");
  Serial.println("Set GPIO "); 
  Serial.print(LEDValue);
  
  if ( LEDValue == "ON" ) {
    digitalWrite(led, HIGH);
    ledState = "On";
    server.send ( 200, "text/html", getPage() );
  }
  else if( LEDValue == "OFF" ) 
  {
    digitalWrite(led, LOW);
    ledState = "Off";
    server.send ( 200, "text/html", getPage() );
  } else 
  {
    Serial.println("Error Led Value");
  }
}

void get_sensores(){
   
    const size_t CAPACITY = JSON_OBJECT_SIZE(4);
    
        StaticJsonDocument<CAPACITY> doc;

        JsonObject object = doc.to<JsonObject>();
        object["temperatura"] = 5;
        object["amonia"] = 15;
      
        serializeJson(doc, jsonOutput);
        server.send(200, "application/json",jsonOutput);
  });
  

}
