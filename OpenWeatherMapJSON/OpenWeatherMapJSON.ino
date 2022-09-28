#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

//Variáveis Globais
const char* ssid = "SATC IOT";
const char* password = "IOT2022@";
unsigned long lastTime = 0;			//Variável de auxílio para medição/sincronização do tempo. 
unsigned long timerDelay = 5000;	//Timer de 5 segundos entre requisições.
String openWeatherMapApiKey = "e4c26f52066fe1ee7e427a2799cf8dcc";
String city = "Criciuma";
String countryCode = "BR";
String jsonBuffer;

void setup() {
  Serial.begin(115200); 

  delay(1000);
  Serial.print("Conectando na rede ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
  delay(500);
    Serial.print(".");
  }
  Serial.print("\nWiFi conectado. Endereço de IP: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer setado para requisições a cada 5 segundos.");
}

void loop() {
  // Envia uma requisição HTTP GET após timerDelay milisegundos.
  if ((millis() - lastTime) > timerDelay) {
    // Verifica o status da conexão
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      // Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) pode ser usado para obter o tipo da variável
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      // Serial.print("JSON object = ");
      // Serial.println(myObject);
      Serial.print("Temperatura: ");
      Serial.println(myObject["main"]["temp"]);
      Serial.print("Pressão: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Umidade: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Velocidade do Vento: ");
      Serial.println(myObject["wind"]["speed"]);
    }
    else {
      Serial.println("WiFi Desconectado");
    }
    lastTime = millis();
  }
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
  
  if (httpResponseCode>0) {
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