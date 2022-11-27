/**--------------------------------------------------------
  2 Autor: Carlos Pasquali, Gustavo Alves;
  3 Hardware: WeMos D1 ESP8266
  4 Framework: Arduino IDE 1.8.19
  5 Curso: Engenharia de Computação
  6 Disciplina: IoT II
  7 -------------------------------------------------------- */


//DEFINES
#define trig D3
#define echo D2
#define ledR D6
#define ledG D7
#define ledB D8

//LIBS
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
const char* ssid = "SATC IOT";
const char* password = "IOT2022@";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create an Event Source on /events
AsyncEventSource events("/events");

// VARS
unsigned long lastTime = 0;  
unsigned long timerDelay = 1000;
int distance;

int duration, cm;

// FUNCTIONS


String processor(const String& var){
 

  if(var == "DISTANCE"){
    return String(distance);
  }
  
  return String();
}


void getSensorReadings(){
  digitalWrite(trig, LOW);
  delayMicroseconds(5);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  duration = pulseIn (echo, HIGH);
  distance = duration * 0.034 / 2;

  Serial.println(distance);
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

void initComponents(){

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);

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
  initComponents();
  handleServer();
}

void siloVolume(){

  if(distance < 140){
     digitalWrite(ledR, LOW); 
     digitalWrite(ledB, LOW); 

     digitalWrite(ledG, HIGH); 
  }

  if(distance >= 140 && distance <= 220 ){
     digitalWrite(ledB, LOW); 

     digitalWrite(ledR, HIGH); 
     digitalWrite(ledG, HIGH); 
  }

  if(distance > 220  ){
     digitalWrite(ledB, LOW); 
     digitalWrite(ledG, LOW); 

     digitalWrite(ledR, HIGH); 
  }
}


void runSystemLoop(){
 if ((millis() - lastTime) > timerDelay) {
    
    getSensorReadings();
    siloVolume();

    // Send Events to the Web Server with the Sensor Readings
    events.send(String(distance).c_str(),"distance",millis());
 

    
    lastTime = millis();
  }
}



void loop() {
  runSystemLoop();
}
