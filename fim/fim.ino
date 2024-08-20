#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Stepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// ULN2003 Motor Driver Pins
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17
int ativa = 0;
int peso = 0;
int pesoAnterior = 0;
int diferenca = 0;
bool boolValue;
String payloadPeso;
// initialize the stepper library
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

const char* ssid = "pao";
const char* password = "zubuzulu";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // set the speed at 5 rpm
  myStepper.setSpeed(15);

  //while (!Serial); // Aguarda até que a porta serial esteja pronta
  //Serial.println("Digite se deseja ativar a seleção de peso (1/0) e o valor do peso separados por espaço:");
}

void loop() {
  // Requisição para ler peso.json
  HTTPClient http;
  String urlPeso = "http://192.168.182.227/esp/peso.json";

  http.begin(urlPeso);
  int httpCodePeso = http.GET();

  if (httpCodePeso > 0) {
    if (httpCodePeso == HTTP_CODE_OK) {
      payloadPeso = http.getString();
      DynamicJsonDocument docPeso(1024);
      DeserializationError errorPeso = deserializeJson(docPeso, payloadPeso);

      if (!errorPeso) {
        Serial.print("Peso recebido do servidor: ");
        Serial.println(payloadPeso);
      } else {
        Serial.print("Erro ao deserializar JSON de peso: ");
        Serial.println(errorPeso.c_str());
      }
    }
  } else {
    Serial.print("Falha na requisição HTTP de peso com código de erro: ");
    Serial.println(httpCodePeso);
  }

  http.end();

  // Requisição para ler bool.json
  String urlBool = "http://192.168.182.227/esp/bool.json";

  http.begin(urlBool);
  int httpCodeBool = http.GET();

  if (httpCodeBool > 0) {
    if (httpCodeBool == HTTP_CODE_OK) {
      String payloadBool = http.getString();
      
      // Convertendo o payload para um valor booleano
      boolValue = payloadBool.toInt(); // Converte para 0 ou 1

      Serial.print("Valor bool recebido do servidor: ");
      Serial.println(boolValue ? "true" : "false");
    }
  } else {
    Serial.print("Falha na requisição HTTP de bool com código de erro: ");
    Serial.println(httpCodeBool);
  }

    if (boolValue == true) { // Verifica se o booleano é true

      peso = payloadPeso.toInt();
      if (pesoAnterior <= peso){
        diferenca = peso-pesoAnterior;
        Serial.println("Aumentando o peso em: ");
        Serial.println(diferenca);
        myStepper.step(15*diferenca*stepsPerRevolution);
        pesoAnterior = peso;
        Serial.println("Peso selecionado!");
      } else{
        diferenca = pesoAnterior-peso;
        Serial.println("Diminuindo o peso em: ");
        Serial.println(diferenca);
        myStepper.step(-15*diferenca*stepsPerRevolution);
        pesoAnterior = peso;
        Serial.println("Peso selecionado!");
    }
    ativa = 0;
  

  }


  
  http.end();
  delay(5000);  // Intervalo de 5 segundos entre as requisições
}
