#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

const char * ssid       = "minicurso";
const char * password   = "12345678";

const char    *host     = "api.coindesk.com";
const String  url       = "/v1/bpi/currentprice/brl.json";
const String  urlYest   = "/v1/bpi/historical/close.json?for=yesterday";

const int     httpPort  = 80;
float         valorAnterior = 0.0;
float         valorOntem    = 0.0;
float         valorAtual    = 0.0;

float         varianciaMin = 0.05;
bool          valorAlta      = 0;


LiquidCrystal_I2C lcd(0x27, 16, 2); 

byte tickerDownR[8] = {
  0b11111,
  0b11111,
  0b11110,
  0b11100,
  0b11000,
  0b10000,
  0b00000,
  0b00000
};


byte tickerDownL[8] = {
  0b11111,
  0b11111,
  0b01111,
  0b00111,
  0b00011,
  0b00001,
  0b00000,
  0b00000
};


byte tickerUpR[8] = {
  0b00000,
  0b00000,
  0b10000,
  0b11000,
  0b11100,
  0b11110,
  0b11111,
  0b11111
};


byte tickerUpL[8] = {
  0b00000,
  0b00000,
  0b00001,
  0b00011,
  0b00111,
  0b01111,
  0b11111,
  0b11111
};

void setup() {

  WiFi.begin(ssid, password);
 
  lcd.begin();  
  lcd.backlight(); 

  lcd.createChar(0,tickerDownL);
  lcd.createChar(1,tickerDownR);
  lcd.createChar(2,tickerUpL);
  lcd.createChar(3,tickerUpR);
  
  lcd.setCursor(0, 0);
  lcd.print("Ligado");
  lcd.write(0);
  lcd.write(1);
  
  Serial.begin(115200);
  
  Serial.println("Conectando ao wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(".");
  Serial.println("WiFi conectado");  
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
 
}

void loop() {
  DynamicJsonBuffer jsonBuffer;

  WiFiClient client;

  Serial.println("Conectando...");
  if (!client.connect(host, httpPort)) {
    Serial.println(String("Falha na conexao com ")+host);
    delay(1000);
    return;
  }  

  Serial.print("Requisitando URL: ");
  Serial.println(url);

  //valor atual
  client.print(String("GET ") + url +   " HTTP/1.1\r\n" + //http request
               "Host: " + host +  "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);  //delay pra esperar a respsta do servidor
  
  String respHttp;
  while(client.available()){
    String line = client.readStringUntil('\r');
    respHttp += line;
  }

  int inicioJson; 
  for (int i = 0; i < respHttp.length(); i++) {
    if (respHttp[i] == '{') {
      inicioJson = i;
      break;
    }
  }
  
  Serial.println(".");
  Serial.println("Resposta GET: ");
  Serial.println(respHttp);

  //Converte pra JSON
  String respJson     = respHttp.substring(inicioJson);
  
  JsonObject& apiTicker     = jsonBuffer.parseObject(respJson);

  if(apiTicker.success()){
    Serial.println("JSON OK");
  }else{
    Serial.println("Falha no JSON");
    valorAtual = valorAnterior;
    goto animation;
  }
  
  Serial.print("BTC: ");
  Serial.println(apiTicker["bpi"]["USD"]["rate_float"].as<String>().toFloat());
  //Serial.println(api["last_price"].as<String>());
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("US$: ");
  lcd.print(apiTicker["bpi"]["USD"]["rate_float"].as<String>().toFloat());

  lcd.setCursor(0,1);
  lcd.print("R$:  ");
  lcd.print(apiTicker["bpi"]["BRL"]["rate_float"].as<String>().toFloat());

  valorAtual = apiTicker["bpi"]["USD"]["rate_float"].as<String>().toFloat();

  animation:
  
  if (valorAnterior == 0.0) {
    valorAnterior = valorAtual;
  }

  //Queda
  if (valorAtual < (valorAnterior - varianciaMin)) {
    valorAlta = 0;
    for(int i = 0; i < 20; i++){
      lcd.setCursor(14,0);
      lcd.write(0);
      lcd.write(1);
      delay(250);
      lcd.setCursor(14,0);
      lcd.print("  ");
      lcd.setCursor(14,1);
      lcd.write(0);
      lcd.write(1);
      delay(50);
      lcd.setCursor(14,1);
      lcd.print("  ");
    }
    valorAnterior = valorAtual;
  }else if (valorAtual > (valorAnterior + varianciaMin)) {//Alta
    valorAlta = 1;
    for(int i = 0; i < 20; i++){
      lcd.setCursor(14,1);
      lcd.write(2);
      lcd.write(3);
      delay(250);
      lcd.setCursor(14,1);
      lcd.print("  ");
      lcd.setCursor(14,0);
      lcd.write(2);
      lcd.write(3);
      delay(50);
      lcd.setCursor(14,0);
      lcd.print("  ");
    }
    valorAnterior = valorAtual;
  }else{
    delay(5000);
  }

  client.stop();

}
