#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//Variáveis Globais
const char* ssid = "SATC IOT";
const char* password = "IOT2022@";
unsigned long lastTime = 0;			//Variável de auxílio para medição/sincronização do tempo. 
unsigned long timerDelay = 10000;	//Timer de 5 segundos entre requisições. 

//Seu nome de domínio com caminho de URL ou endereço IP com caminho
String serverName = "http://api.thingspeak.com/update?api_key=REPLACE_WITH_YOUR_API_KEY";

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
 
  Serial.println("Timer setado para requisições a cada 10 segundos.");
  
  // A semente aleatória é um número usado para inicializar um gerador de números pseudoaleatórios
  randomSeed(analogRead(0));
}

void loop() {
  // Envia uma requisição HTTP GET após timerDelay milisegundos.
  if ((millis() - lastTime) > timerDelay) {
    // Verifica o status da conexão
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
    
      String serverPath = serverName + "&field1=" + String(random(40));
      
      http.begin(client, serverPath.c_str());
      
	  // Realiza a requisição GET e armazena a resposta.
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("Resposta Código HTTP: ");
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
    else {
      Serial.println("WiFi Desconectado");
    }
    lastTime = millis();
  }
}