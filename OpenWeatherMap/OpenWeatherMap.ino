#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>
 //Variáveis Globais
const char * ssid = "SATC IOT";
const char * password = "IOT2022@";
unsigned long lastTime = 0; //Variável de auxílio para medição/sincronização do tempo.
unsigned long timerDelay = 5000; //Timer de 5 segundos entre requisições.
//Seu nome de domínio com caminho de URL ou endereço IP
String serverName =
  "http://api.openweathermap.org/data/2.5/weather?q=Criciuma,BR&APPID=e4c26f52066fe1ee7e427a2799cf8dcc";

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.print("Conectando na rede ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
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
    //Verifica o status da conexão
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client; //Cria o objeto do cliente na classe WiFiClient.
      HTTPClient http; //Cria o objeto http na classe HTTPClient.
      // Inicializa o objeto HTTP passando como argumento o objeto do cliente e
      // o servidor a ser acessado.
      http.begin(client, serverName.c_str());
      // Realiza a requisição GET e armazena a resposta.
      int httpResponseCode = http.GET();

      //Caso tenha recebido alguma resposta válida (maior que 0)
      if (httpResponseCode > 0) {
        Serial.print("Resposta Código HTTP: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      } else {
        Serial.print("Código de Erro: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    } else {
      Serial.println("WiFi Desconectado");
    }
    lastTime = millis();
  }
}
