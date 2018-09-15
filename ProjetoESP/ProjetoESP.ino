#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

#define LED1 D1
#define LED2 D2
#define LED3 D3
#define LED4 D5
#define LED5 D6

const char *ssid = "ESP8266 Gustavo";
const char *password = "12345678";

ESP8266WebServer myServer(80);
IPAddress espIP(192, 168, 1, 1);

void handleRoot() { 
  File dataFile = SPIFFS.open("/room.html", "r");
  
  myServer.streamFile(dataFile, "text/html");
  dataFile.close();
}

void handleLED(){

  //Serial.println("Recebi um request");
  if(myServer.hasArg("led1")){
    if(myServer.arg("led1") == "1"){
      digitalWrite(LED1,HIGH);
    }else{
      digitalWrite(LED1,LOW);
    }
  }

  if(myServer.hasArg("led2")){
    if(myServer.arg("led2") == "1"){
      digitalWrite(LED2,HIGH);
    }else{
      digitalWrite(LED2,LOW);
    }
  }

  if(myServer.hasArg("led3")){
    if(myServer.arg("led3") == "1"){
      digitalWrite(LED3,HIGH);
    }else{
      digitalWrite(LED3,LOW);
    }
  }

  if(myServer.hasArg("led4")){
    if(myServer.arg("led4") == "1"){
      digitalWrite(LED4,HIGH);
    }else{
      digitalWrite(LED4,LOW);
    }
  }

  if(myServer.hasArg("led5")){
    if(myServer.arg("led5") == "1"){
      digitalWrite(LED5,HIGH);
    }else{
      digitalWrite(LED5,LOW);
    }
  }

}

void setup() {
  
  delay(5000);
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configurando o ponto de acesso...");

  //sistema de arquivo SPIFFS
  SPIFFS.begin();
  Serial.println("Sistema de arquivos SPIFFS inicializado");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(espIP, espIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  
  Serial.print("Endereço IP: ");
  Serial.println(espIP);

  myServer.on("/", handleRoot);
  myServer.on("/mudarLED",handleLED);
  myServer.onNotFound(handleRoot);
  
  myServer.begin();

  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  pinMode(LED5,OUTPUT);

  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(LED3,LOW);
  digitalWrite(LED4,LOW);
  digitalWrite(LED5,LOW);
  
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  myServer.handleClient();
}
