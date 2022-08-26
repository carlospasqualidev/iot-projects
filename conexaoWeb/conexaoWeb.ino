/**--------------------------------------------------------
  2 Autor: Prof° Vagner Rodrigues;
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

//VARS
const char* ssid = "SATC IOT";
const char* password = "IOT2022@";

WiFiServer server(80);

unsigned long currentTime = 0, previousTime = 0;
const long timeOut = 2000;

String header = "";
String stateLedR = "Off";


#define ledR D6
#define ledG D7
#define ledB D8



void setup()
{
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

  server.begin();
}

void loop()
{
  WiFiClient client = server.available();

  if (client) {

    Serial.println("Novo cliente conectado.\n");

    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;

    while ((client.connected()) && ((currentTime - previousTime) <= timeOut)) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();


        Serial.write(c);
        header += c;

        if (c == '\n') //pra saber que acabou a requisição HTTP
        {
          if (currentLine.length() == 0) {
            /* cabeçalhos http sempre começam com um código de resposta e um tipo de conteudo para que o cliente saiba o que está por vir. */
            client.println("HTTP/1.1.200 OK");
            client.println("content-type:text/html");
            client.println("Connection: closed");
            client.println("");

            if (header.indexOf("GET /ledR/on") >= 0) {
              digitalWrite(ledR, HIGH);
              stateLedR = "On";
            } else if (header.indexOf("GET /ledR/off") >= 0 ) {

              digitalWrite(ledR, LOW);
              stateLedR = "Off";
            }

            // Display the HTML web page
            // Indica que serÃ¡ enviado cÃ³digo HTML.
            client.println("<!DOCTYPE html><html>");
            // Torna a web page responsiva em qualquer browser.
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            // Ovita solicitaÃ§Ãµes relacionadas ao favicon (nÃ£o se preocupe quanto a isso).
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");

            client.println("<body><h1>IoT II - ESP8266 web server!!</h1>");
            client.println("<p> Estado do LED Vermelho:" + stateLedR + " </p>");

            if (stateLedR == "On") {
              client.println("<p><a href=\"/ledR/off\"><button class=\"button button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/ledR/on\"><button class=\"button\">ON </button></a></p>");
            }


            client.println("</body");
            client.println("</html>");
          }
          else {
            currentLine = "";
          }
        }
        else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Cliente Desconectado.");
  }
}
