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
    <h1>Aviário 722</h1>
    <p>
        Temperatura:<span id="temp"></span><br />
        Amônia:<span id="ar"></span>
    </p>
    <script>
        const api_url = "sensores";
        
        setInterval(async function() {

            const response = await fetch(api_url);
            const data = await response.json();
            const { temperatura , amonia} = data;
            
            document.getElementById("temp").textContent = temperatura;
            document.getElementById("ar").textContent = amonia;
            }, 2000);  
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
  server.on("/sensores", get_sensores);
  
  server.begin();
  Serial.println("HTTP server started");
}
void loop(void) {
  server.handleClient();
  MDNS.update();
  
}


void get_sensores(){
   
    const size_t CAPACITY = JSON_OBJECT_SIZE(4);
    
        StaticJsonDocument<CAPACITY> doc;

        JsonObject object = doc.to<JsonObject>();
        object["temperatura"] = amoniaAr;
        object["amonia"] = temperaturaAtual;
      
        serializeJson(doc, jsonOutput);
        server.send(200, "application/json",jsonOutput);
        amoniaAr++;
        temperaturaAtual += 10; 

}
