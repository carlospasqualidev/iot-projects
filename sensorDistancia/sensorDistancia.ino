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

#define btn1 D4
#define btn2 D5

#define ledR D6
#define ledG D7
#define ledB D8

#define trig D3
#define echo D2

int distance;
bool btnState1 = false;
bool beforeBtnState = false;

void setup()
{  
  pinMode(ledR, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(ledG, OUTPUT);
  
  pinMode(btn1, INPUT_PULLUP);
  
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);


  Serial.begin (9600);
}

void loop()
{      
  Serial.println(btnState1);
 
    sensor();

    if(distance > 50){ 
      digitalWrite(ledB, LOW);
      digitalWrite(ledR, LOW);
      
      digitalWrite(ledG, HIGH);
    }
    else if(distance > 30 && distance < 49){ 
       digitalWrite(ledB, LOW);
 
      digitalWrite(ledG, HIGH);
      digitalWrite(ledR, HIGH);
    }
    else if(distance > 10 && distance < 29){ 
        digitalWrite(ledR, HIGH);
    }
    else{
      digitalWrite(ledB, LOW);
      digitalWrite(ledG, LOW);
      
      digitalWrite(ledR, HIGH);
      delay(200);
      digitalWrite(ledR, LOW);
      delay(200);
    }
  
}

void sensor() {
  
  digitalWrite(trig, LOW);
  delayMicroseconds(5);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  distance = pulseIn (echo, HIGH)/58;
}
