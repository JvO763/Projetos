#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AccelStepper.h>

// Definição dos pinos do driver ULN2003
#define IN1 2
#define IN2 4
#define IN3 5
#define IN4 19

// Configuração do motor de passo
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);

AsyncWebServer server(80);

const int output26 = 26;
const int output27 = 27;
const int output2 = 2;

String inputUser;
int inputPeso;
int inputPassado = 0;
int angulo;

const char* ssid = "JoaoNet";
const char* password = "zubuzulu";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Exemplo de HTML com Botões</title>
</head>
<body>
    <h2>Informe seu usuário:</h2>
    <input type="text" id="inputUsuario">
    <br><br>
    <div id="pesoAtualDiv" style="display: none;">
        <h3 id="pesoAtualText">Peso atual: <span id="pesoAtual">--</span> kg</h3>
        <br>
    </div>
    <button onclick="treinar()">Treinar!</button>
    <button onclick="mudarPeso()">Mudar Peso</button>
    <br><br>
    <script>
        // Objeto para armazenar o peso de cada usuário
        var pesosUsuarios = {};

        function treinar() {
            var usuario = document.getElementById("inputUsuario").value;
            if (usuario.trim() === "") {
                alert("Por favor, insira um nome de usuário válido.");
                return;
            }
            // Verifica se o usuário já existe no objeto
            if (!pesosUsuarios.hasOwnProperty(usuario)) {
                pesosUsuarios[usuario] = 0; // Define o peso inicial como 0
            }
            // Exibe o peso atual do usuário
            document.getElementById("pesoAtual").textContent = pesosUsuarios[usuario];
            document.getElementById("pesoAtualDiv").style.display = "block";
            alert("Usuário '" + usuario + "' está treinando!");

            // Envia o peso atual para a ESP32
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/set?User=" + usuario + "&peso=" + pesosUsuarios[usuario], true);
            xhttp.send();
        }

        function mudarPeso() {
            var usuario = document.getElementById("inputUsuario").value;
            if (usuario.trim() === "") {
                alert("Por favor, insira um nome de usuário válido.");
                return;
            }
            var novoPeso = prompt("Insira o novo peso:");
            if (novoPeso !== null && !isNaN(novoPeso)) {
                pesosUsuarios[usuario] = parseFloat(novoPeso); // Atualiza o peso do usuário
                // Exibe o novo peso atualizado
                document.getElementById("pesoAtual").textContent = pesosUsuarios[usuario] + " kg";
                alert("Peso alterado para " + pesosUsuarios[usuario] + " kg.");

                // Envia o novo peso para a ESP32
//                var xhttp = new XMLHttpRequest();
//                xhttp.open("GET", "/set?User=" + usuario + "&peso=" + pesosUsuarios[usuario], true);
//                xhttp.send();
            } else {
                alert("Por favor, insira um valor numérico válido para o peso.");
            }
        }
    </script>
</body>
</html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  // disable watch dog timer 0
  disableCore0WDT();

  // maybe disable watch dog timer 1 if needed
  disableCore1WDT();
    // Defina a velocidade máxima e a velocidade inicial do motor (em passos por segundo)
  stepper.setMaxSpeed(1000);
  stepper.setSpeed(1000);

  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  pinMode(output2, OUTPUT);

  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);
  digitalWrite(output2, LOW);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/set", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->hasParam("User")) {
      inputUser = request->getParam("User")->value();
    }
    else {
      inputUser = "No user sent";
    }
    Serial.println("User: " + inputUser);

    if (request->hasParam("peso")) {
      inputPeso = request->getParam("peso")->value().toInt();
    }
    else {
      inputPeso = 0;
    }
    Serial.println("Peso: " + String(inputPeso));  

    request->send(200, "text/plain", "OK");
  });

  server.onNotFound(notFound);
  server.begin();
}

void loop() {
      // Lógica para girar motor com base no peso
      
    if (inputPeso <= 10 && inputPassado != inputPeso) {
       moveMotor(1000, 1000, 5); // Velocidade, Pulsos, Delay entre pulsos
    } else if (inputPeso <= 20 && inputPassado != inputPeso) {
       moveMotor(1000, 2000, 5); // Velocidade, Pulsos, Delay entre pulsos
    } else if (inputPassado != inputPeso) {
       moveMotor(1000, 3000, 5); // Velocidade, Pulsos, Delay entre pulsos
    }
    inputPassado = inputPeso;
}

// Função para mover o motor de passo para a direita
void moveMotor(int velocidade, int pulsos, int delayEntrePulsos) {
  int pulsos_Efetivos = pulsos - angulo;
  if(pulsos_Efetivos>=0){
    stepper.setSpeed(velocidade);
    for(int i = 0; i <= pulsos_Efetivos; i++) {
    stepper.runSpeed();
    delay(delayEntrePulsos);
 
  }}
  else{
    for(int i = 0; i <= -pulsos_Efetivos; i++) {
      stepper.setSpeed(-velocidade);
    stepper.runSpeed();
    delay(delayEntrePulsos);
  }
  };
      
    stepper.setSpeed(0);
    stepper.runSpeed();
    stepper.stop();
    delay(1000);
  
    angulo = pulsos;
  }

  
