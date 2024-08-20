#include <AccelStepper.h>

// Definição dos pinos do driver ULN2003
#define IN1 2
#define IN2 4
#define IN3 5
#define IN4 19

// Configuração do motor de passo
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);

void setup() {
  // Defina a velocidade máxima e a velocidade inicial do motor (em passos por segundo)
  stepper.setMaxSpeed(1000);
  stepper.setSpeed(1000);

  // Inicializa a comunicação serial para debug
  Serial.begin(115200);
}

void loop() {
  // Move o motor um passo para a direita
  
  stepper.setSpeed(1000);
  for(int i=0; i<=2000; i++){ //Uma volta completa a 1000 pulsos por segundo
      stepper.runSpeed();
    delay(5); // Espera 0.005 segundos
    Serial.println(i);
    }

  // Move o motor parado
  stepper.setSpeed(0);
 
  stepper.runSpeed();
  stepper.stop();
  delay(1000); // Espera 1 segundos
}
