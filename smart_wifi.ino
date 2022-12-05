#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#define relayPin 15

int relayState = LOW;

String request = "";

unsigned long current;
unsigned long uuid = random(777777, 999999);

WebServer server(80);

// HANDLE POWER ON
void handlePowerOn(){

    StaticJsonBuffer<512> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    //print json result
    String str_client;
    root["result"] = true;
    root["uuid"] = uuid; 
    root.printTo(str_client);
    Serial.println(str_client);

    server.send(200, "text/plain", str_client); 
    digitalWrite(relayPin, HIGH);
}

// HANDLE POWER OFF
void handlePowerOff(){
  
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    //print json result
    String str_client;
    root["result"] = false;
    root["uuid"] = uuid;
    root.printTo(str_client);
    Serial.println(str_client);

    server.send(200, "text/plain", str_client); 
    digitalWrite(relayPin, LOW);
}

// INIT
void setup() {
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);

  WiFi.mode(WIFI_STA);
  /* start SmartConfig */
  WiFi.beginSmartConfig();

  /* Wait for SmartConfig packet from mobile */
  Serial.println("Waiting for wifi connection.");
  while (!WiFi.smartConfigDone()) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("wifi connected done.");

  /* Wait for WiFi to connect to AP */
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  //begin server local
  server.begin();
  server.on("/on", handlePowerOn);
  server.on("/off", handlePowerOff);
  
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}
 

void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
}
