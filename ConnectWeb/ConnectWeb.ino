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
  
//VARS
const char* ssid = "SATC IOT";
const char* password = "IOT2022@";

WiFiServer server(80);

unsigned long currentTime = 0, previousTime = 0;
const long timeOut = 2000;

String header = "";
char stateLed = 'O';


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

            if (header.indexOf("GET /ActiveLedIs/R") >= 0) {

              digitalWrite(ledG, LOW);
              digitalWrite(ledB, LOW);
              digitalWrite(ledR, HIGH);
              stateLed = 'R';
            } else if (header.indexOf("GET /ActiveLedIs/G") >= 0 ) {

              digitalWrite(ledG, HIGH);
              digitalWrite(ledB, LOW);
              digitalWrite(ledR, LOW);
              stateLed = 'G';
            } else if (header.indexOf("GET /ActiveLedIs/B") >= 0 ) {

              digitalWrite(ledG, LOW);
              digitalWrite(ledB, HIGH);
              digitalWrite(ledR, LOW);
              stateLed = 'B';
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


            //STYLES
            client.println("<style>");

            client.println("html {font-family: Helvetica;margin: 0;padding: 0;box-sizing: border-box;}");

            client.println("* {margin: 0;padding: 0;box-sizing: border-box;color: #9ba8b9;}");

            client.println(".hr {width: 100%; border-color: #9ba8b9;}");

            client.println(".background {display: flex;flex-direction: column;align-items: center;padding: 24px;width: 100%; height: 100vh;background-color: #273341;}");

            client.println(".container {display: flex;gap: 24px;flex-direction: column;justify-content: center;align-items: center;padding: 24px;border-radius: 10px;width: 30%;background-color: #4b5767;}");

            client.println(".button { border-radius: 10px;padding: 8px 12px;outline: none;border: none;cursor: pointer; color: white;}");

            client.println(".rgbController { display: flex;gap: 8px;}");

            client.println(".ledIndicatorContainer {display: flex;align-items: center;justify-content: center;flex-direction: column;gap: 8px;}");

            client.println(".ledIndicator {width: 24px;height: 24px;background-color: #2b2b2b;border-radius: 100%;}");

            client.println("</style>");

            client.println("</head>");

            //BODY
            client.println("<body>");

            client.println("<div class=\"background\">");
            client.println("<div class=\"container\">");

            client.println("<h1>IoT II - ESP8266 Web Server</h1>");
            client.println("<hr class=\"hr\" />");

            client.println("<div class=\"ledIndicatorContainer\">");

            client.println("<p>Led status:</p>");

            switch (stateLed) {

              case 'R':
                client.println("<div class=\"ledIndicator\" style=\"background-color: red\"></div>");
                break;
              case 'G':
                client.println("<div class=\"ledIndicator\" style=\"background-color: green\"></div>");
                break;
              case 'B':
                client.println("<div class=\"ledIndicator\" style=\"background-color: blue\"></div>");
                break;
              default :
                client.println("<div class=\"ledIndicator\" style=\"background-color: #2b2b2b\"></div>");
                break;
            }



            client.println("</div>");
            client.println("<hr class=\"hr\"></hr>");


            client.println("<div class=\"rgbController\">");

            client.println("<p> <a href=\"/ActiveLedIs/R\"><button class=\"button\" style=\"background-color: red\">R</button></a></p>");
            client.println("<p> <a href=\"/ActiveLedIs/G\"><button class=\"button\" style=\"background-color: green\">G</button></a></p>");
            client.println("<p> <a href=\"/ActiveLedIs/B\"><button class=\"button\" style=\"background-color: blue\">B</button></a></p>");

            client.println("</div>");
            client.println("</div>");
            client.println("</div>");

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
