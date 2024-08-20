#include <PID_v1_bc.h> // Inclua a biblioteca PID
#include <Servo.h> // Inclua a biblioteca Servo para controle de servo-motor

// Defina os pinos para o sensor e o atuador (saída de controle)
const int sensorPin = A0; // Pino analógico para leitura do sensor
const int actuatorPin = 9; // Pino PWM para controle do atuador (servo-motor)

// Defina os parâmetros do controlador PID
double Setpoint = 100; // Defina o valor desejado (setpoint)
double Input, Output; // Variáveis para leitura do sensor e saída do controlador
double Kp = 0.5529, Ki = 0.2734, Kd = 0.2726; // Ganho do controlador PID, coloquei o do discreto, mas depende da frequência de leitura do sensor pra eu considerar contínuo ou discreto

// Crie uma instância do objeto PID
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// Crie uma instância do objeto Servo para controlar o servo-motor
Servo myServo;

void setup() {
  // Inicialize o sensor e o atuador
  pinMode(sensorPin, INPUT);
  myServo.attach(actuatorPin);

  // Inicialize o controlador PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(100); // Defina o tempo de amostragem em milissegundos (100 ms)
  myPID.SetOutputLimits(0, 180); // Defina os limites de saída do controlador (0 a 180 graus para o servo)

  // Inicialize a comunicação serial
  Serial.begin(9600);
}

void loop() {
  // Leia o valor do sensor
  Input = analogRead(sensorPin);

  // Calcule o controle PID
  myPID.Compute();

  // Ajuste a saída do controlador para o atuador (servo-motor)
  myServo.write(Output);

  // Imprima os valores
  Serial.print("Sensor: ");
  Serial.print(Input);
  Serial.print(" | Kp: ");
  Serial.print(Kp);
  Serial.print(" | Ki: ");
  Serial.print(Ki);
  Serial.print(" | Kd: ");
  Serial.println(Kd);
  Serial.print(" | Setpoint: ");
  Serial.print(Setpoint);
  Serial.print(" | PWM: ");
  Serial.println(Output); // No caso de servo-motores, o valor de saída é diretamente o ângulo

  // Espere um tempo antes da próxima iteração
  delay(100); // Intervalo de amostragem de 100 ms
}
