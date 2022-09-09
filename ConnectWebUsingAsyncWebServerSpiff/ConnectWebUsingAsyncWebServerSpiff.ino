/**--------------------------------------------------------
  2 Autor: Carlos Pasquali;
  3 Hardware: WeMos D1 ESP8266
  4 Framework: Arduino IDE 1.8.19
  5 Curso: Engenharia de Computação
  6 Disciplina: IoT II
  7 -------------------------------------------------------- */
/* Relação entre pinos da WeMos D1 R2 e GPIOs do ESP8266
  9 Pinos-WeMos Função Pino-ESP-8266
  10 TX TXD TXD/GPIO1
  11 RX RXD RXD/GPIO3
  12 D0 IO GPIO16
  13 D1 IO, SCL GPIO5
  14 D2 IO, SDA GPIO4
  15 D3 IO, 10k PU GPIO0
  16 D4 IO, 10k PU, LED GPIO2 //Utilizado na gravação. Sug: Usar como INPUT.
  17 D5 IO, SCK GPIO14 //Utilizado na gravação. Sug: Usar como INPUT.
  18 D6 IO, MISO GPIO12
  19 D7 IO, MOSI GPIO13
  20 D8 IO, 10k PD, SS GPIO15
  21 A0 Inp. AN 3,3Vmax A0
  22 */

//DEFINES
#define trig D3
#define echo D2
#define ledR D6
#define ledG D7
#define ledB D8

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "Rosemary";
const char* password = "996529424";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// VARS
String ledRState;
String ledGState;
String ledBState;

// FUNCTIONS

String processor(const String& var){
  
  Serial.println(var);
  if(var == "STATERED"){
    if(digitalRead(ledR)){
      ledRState = "ON";
    }
    else{
      ledRState = "OFF";
    }
    return ledRState;

  } else if(var == "STATEGREEN"){
    if(digitalRead(ledG)){
      ledGState = "ON";
    }
    else{
      ledGState = "OFF";
    }
    return ledGState;
  } else if(var == "STATEBLUE"){
    if(digitalRead(ledB)){
      ledBState = "ON";
    }
    else{
      ledBState = "OFF";
    }
    return ledBState;
  }
}

void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.print("Server is running in: ");
    Serial.println(WiFi.localIP());
}

void importFiles(){
    // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  Serial.println("import files complete!");

}

void handleServer(){
  // Route to set GPIO to HIGH
  server.on("/ledRon", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledR, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/ledRoff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledR, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

 server.on("/ledGon", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledG, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/ledGoff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledG, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

   server.on("/ledBon", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledB, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/ledBoff", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledB, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });


  // Start server
  server.begin();
}

void setup() {
  Serial.begin(115200);
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);


  initWiFi();
  importFiles();
  handleServer();
}

void loop() {

}
