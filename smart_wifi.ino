#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#define relayPin 0

int relayState = LOW;

String request = "";

unsigned long current;
unsigned long uuid = random(777777, 999999);

WiFiServer server(80);


void setup() {
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);

  WiFi.mode(WIFI_STA);
  /* start SmartConfig */
  WiFi.beginSmartConfig();

  /* Wait for SmartConfig packet from mobile */
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("SmartConfig done.");

  /* Wait for WiFi to connect to AP */
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  //begin server local
  server.begin();

  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  request = "";
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  WiFiClient client = server.available();

  //jika terhubung dg wifi
  if (client) {
    while (!client.available()) {
      delay(1);
    }
    Serial.print("Request Received:");
    request = client.readStringUntil('\r\n');
    Serial.println(request);
    client.flush();

    //process the request
    if (request.indexOf("/power=true") != -1) {
      relayState = HIGH;
      digitalWrite(relayPin, relayState);
    } else if (request.indexOf("/power=false") != -1) {
      relayState = LOW;
      digitalWrite(relayPin, relayState);
    }
  }


  //jika ada request
  if (client) {
    boolean result = false;

    //Kondisi LED ESP8266-01 (HIGH=mati,LOW=hidup)
    if (relayState) {
      result = false;
    } else {
      result = true;
    }

    //print json result
    String str_client;
    root["result"] = result;
    root["uuid"] = uuid;
    root.printTo(Serial);

    Serial.println();
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json; charset=utf-8");
    client.println("");
    root.printTo(str_client);
    client.println(str_client);
  }
}