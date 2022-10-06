#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <DHT.h>

#define DHTPIN 14 
#define DHTTYPE DHT11 // DHT 11

//Variáveis Globais
const char* ssid = "SATC IOT";
const char* password = "IOT2022@";

// timer
unsigned long lastTime = 0;      //Variável de auxílio para medição/sincronização do tempo.
unsigned long timerDelay = 20000; //Timer de 20 segundos entre requisições.

//thingSpeakVars
String thingSpeakTemperature = "";
String thingSpeakHumidity = "";

//sensor
String sensorTemperature = "";
String sensorHumidity = "";
DHT dht(DHTPIN, DHTTYPE);

//urls
String thingSpeakServer = "http://api.thingspeak.com/update?api_key=T8JAK68MG6JSTF7I";

String openWeatherMapApiKey = "e4c26f52066fe1ee7e427a2799cf8dcc";
String city = "Criciuma";
String countryCode = "BR";

String jsonBuffer;


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

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Inicializa o objeto HTTP passando como argumento o objeto do cliente e
  // o servidor a ser acessado.
  http.begin(client, serverName);

  // Realiza a requisição GET e armazena a resposta
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("Resposta Código HTTP: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Código de Erro: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void sensorReader(){

  sensorHumidity = dht.readHumidity();
  sensorTemperature = dht.readTemperature();

  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(dht.readHumidity()) || isnan(dht.readTemperature())) 
  {
    Serial.println("\nFailed to read from DHT");
  } 
  else
  {
    Serial.print("\nHumidade Sensor: ");
    Serial.print(sensorHumidity);
    Serial.print(" %t");
    Serial.print("\nTemperatura Sensor: ");
    Serial.print(sensorTemperature);
    Serial.println(" *C");
  }

}


//thingSpeak
void thingSpeakRequest() {
  WiFiClient client;
  HTTPClient http;

  String serverPath;

  serverPath = thingSpeakServer + "&field1=" + thingSpeakTemperature;
  serverPath += "&field2=" + thingSpeakHumidity;
  serverPath += "&field3=" + sensorTemperature;
  serverPath += "&field4=" + sensorHumidity;

  http.begin(client, serverPath.c_str());

  // Realiza a requisição GET e armazena a resposta.
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("\nResposta Código HTTP ThingSpeak: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Código de Erro: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
}

// openWheater
void openWeatherMap() {
  String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";

  jsonBuffer = httpGETRequest(serverPath.c_str());
  JSONVar myObject = JSON.parse(jsonBuffer);

  // JSON.typeof(jsonVar) pode ser usado para obter o tipo da variável
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("\nParsing input failed!");
    return;
  }

  Serial.println("\nTemperature OpenWheather");
  Serial.println(myObject["main"]["temp"]);
  thingSpeakTemperature = JSON.stringify(myObject["main"]["temp"]);

  Serial.println("\nHumidity OpenWheather");
  Serial.println(myObject["main"]["humidity"]);
  thingSpeakHumidity = JSON.stringify(myObject["main"]["humidity"]);

}

void setup() {
  Serial.begin(115200);

  dht.begin();
  initWiFi();
}

void loop() {
  // Envia uma requisição HTTP GET após timerDelay milisegundos.
  if ((millis() - lastTime) > timerDelay) {
    // Verifica o status da conexão
    if (WiFi.status() == WL_CONNECTED) {
     
      sensorReader();

      thingSpeakRequest();

      openWeatherMap();

    }
    else {
      Serial.println("\nWiFi Desconectado");
    }
    lastTime = millis();
  }
}
