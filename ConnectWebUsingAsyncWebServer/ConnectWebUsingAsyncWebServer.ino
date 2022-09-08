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

//LIBS
#include <ESP8266WiFi.h>

#include <ESPAsyncTCP.h>

#include <ESPAsyncWebServer.h>

//DEFINES
#define ledR D6
#define ledG D7
#define ledB D8

//FUNCTIONS
String processor(const String &
                 var);
String outputState(int output);

//VARS
const char * ssid = "SATC IOT";
const char * password = "IOT2022@";

const char * PARAM_INPUT_1 = "output";
const char * PARAM_INPUT_2 = "state";

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML><html>
<head>
  <title>IoT II - ESP8266 Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
  
</head>

<body>
  <h2>IoT II - ESP8266 Web Server Async</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ 
  xhr.open("GET", "/update?output="+element.id+"&state=1", true); 
  }
  else { 
  xhr.open("GET", "/update?output="+element.id+"&state=0", true); 
  }
  xhr.send();
}
</script>
</body>
</html>

)rawliteral";

void setup() {
  pinMode(ledR, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(ledG, OUTPUT);

  Serial.begin(115200);
  delay(2000);
  Serial.print("Conectando na rede...");

  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\n WIFI Conectado. Endereço de IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request -> send_P(200, "text/html", index_html, processor);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {

    String inputMessage1, inputMessage2;

    //GET ipESP/update?output=<inputMessage1>&state=<inputMessage2>
    if ((request -> hasParam(PARAM_INPUT_1)) && (request -> hasParam(PARAM_INPUT_2))) {
      inputMessage1 = request -> getParam(PARAM_INPUT_1) -> value();
      inputMessage2 = request -> getParam(PARAM_INPUT_2) -> value();
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
    } else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }

    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - set to: ");
    Serial.print(inputMessage2);
    request -> send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {

}

String processor(const String& var){
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons +="<h4> LED R: <h4> <label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=" + String (ledR) + " "+ outputState(ledR) +"><span class=\"slider\"></span></label>";
    buttons +="<h4> LED G: <h4> <label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=" + String (ledG) + " "+ outputState(ledG) +"><span class=\"slider\"></span></label>";
    buttons +="<h4> LED B: <h4> <label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=" + String (ledB) + " "+ outputState(ledB) +"><span class=\"slider\"></span></label>";
   
    return buttons;
  }
  return String();
}

  String outputState(int output){
    if(digitalRead(output)){
      return "checked";
      }else{
        return "";
    }
  }
