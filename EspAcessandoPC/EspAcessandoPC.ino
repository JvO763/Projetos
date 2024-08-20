#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "JoaoNet";
const char* password = "zubuzulu";

WebServer server(80);

void handleRoot() {
  if (server.hasArg("peso")) {
    String peso = server.arg("peso");
    int pesoValue = peso.toInt();
    Serial.print("Peso recebido: ");
    Serial.println(pesoValue);
  }
  server.send(200, "text/plain", "Peso atualizado");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  server.on("/esp/painel.php", handleRoot);
  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();
}
