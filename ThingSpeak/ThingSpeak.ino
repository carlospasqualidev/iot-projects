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

#define trig D3
#define echo D2

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <h2>IoT II - ESP8266 Web Server Async</h2>
  <p id="dist"> %DISTANCE%</p>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('distance', function(e) {
  console.log("distance", e.data);
  document.getElementById("dist").innerHTML = e.data;
 }, false);
 
 
}
</script>
</body>
</html>)rawliteral";

// Replace with your network credentials
const char * ssid = "Rosemary";
const char * password = "996529424";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create an Event Source on /events
AsyncEventSource events("/events");

// VARS
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;
int distance;

// FUNCTIONS
void getSensorReadings(){
  digitalWrite(trig, LOW);
  delayMicroseconds(5);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  distance = pulseIn (echo, HIGH)/58;
  Serial.println(distance);
}

String processor(const String& var){
  getSensorReadings();

  if(var == "DISTANCE"){
    return String(distance);
  }
  
  return String();
}

void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

void handleServer(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 1000);
  });
  server.addHandler(&events);
  server.begin();
}

void setup() {
  Serial.begin(115200);
  initWiFi();

   pinMode(trig, OUTPUT);
   pinMode(echo, INPUT);

  handleServer();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    getSensorReadings();
      // Send Events to the Web Server with the Sensor Readings
    events.send(String(distance).c_str(),"distance",millis());
    
    lastTime = millis();
  }
}
