#include <Arduino.h>

// Definição dos pinos GPIOs que são utilizados para conectar com as entradas dos relés
const int RELAY_PIN_VERDE = 27; //VERDE
const int RELAY_PIN_AMARELO = 26; // VERMELHO 
const int RELAY_PIN_VERMELHO = 25; // AMARELO

// Define a lógica de ativação do relé.
// Neste exemplo, o relé é ativado com LOW (LOW_LEVEL_TRIGGER).
// Se o seu jumper estiver configurado para HIGH, mude para HIGH_LEVEL_TRIGGER.
// #define LOW_LEVEL_TRIGGER
#define HIGH_LEVEL_TRIGGER

void setup() {
  Serial.begin(9600);
  Serial.println("Inicializando...");

  // Configura os pinos dos relés como saída
  pinMode(RELAY_PIN_VERDE, OUTPUT);
  pinMode(RELAY_PIN_AMARELO, OUTPUT);
  pinMode(RELAY_PIN_VERMELHO, OUTPUT);

  // Inicializa o relé no estado desligado
  // O estado inicial depende da lógica de acionamento
  #ifdef HIGH_LEVEL_TRIGGER
    digitalWrite(RELAY_PIN_VERDE, LOW); // LOW ativa o relé, então HIGH o mantém desligado
    digitalWrite(RELAY_PIN_AMARELO, LOW); 
    digitalWrite(RELAY_PIN_VERMELHO, LOW); 
    Serial.println("Relé configurado para acionamento por LOW. Desligado no início.");
  #endif

  // #ifdef LOW_LEVEL_TRIGGER
  //   digitalWrite(RELAY_PIN, LOW); // HIGH ativa o relé, então LOW o mantém desligado
  //   Serial.println("Relé configurado para acionamento por HIGH. Desligado no início.");
  // #endif
}

void loop() {
  // A cada 5 segundos, o relé liga e desliga, acendendo e apagando a lâmpada.

  // Acende a lâmpada (liga o relé)
  // #ifdef LOW_LEVEL_TRIGGER
  //   digitalWrite(RELAY_PIN_VERDE, HIGH);
  // #endif
  #ifdef HIGH_LEVEL_TRIGGER
    digitalWrite(RELAY_PIN_VERDE, HIGH);
    digitalWrite(RELAY_PIN_AMARELO, HIGH);
    digitalWrite(RELAY_PIN_VERMELHO, HIGH);
  #endif

  Serial.println("Lâmpada Verde Acessa!");
  delay(5000); // Espera 5 segundos

  // Apaga a lâmpada (desliga o relé)
  // #ifdef LOW_LEVEL_TRIGGER
  //   digitalWrite(RELAY_PIN_VERDE, LOW);
  // #endif
  #ifdef HIGH_LEVEL_TRIGGER
    digitalWrite(RELAY_PIN_VERDE, LOW);
  #endif

  Serial.println("Lâmpada Verde Apagada!");
  delay(5000); // Espera 5 segundos

    Serial.println("Lâmpada Amarela Acessa!");
  delay(5000); // Espera 5 segundos

  #ifdef HIGH_LEVEL_TRIGGER
    digitalWrite(RELAY_PIN_AMARELO, LOW);
  #endif

  Serial.println("Lâmpada Amarela Apagada!");
  delay(5000); // Espera 5 segundos

    Serial.println("Lâmpada Vermelha Acessa!");
  delay(5000); // Espera 5 segundos

  #ifdef HIGH_LEVEL_TRIGGER
    digitalWrite(RELAY_PIN_VERMELHO, LOW);
  #endif

  Serial.println("Lâmpada Vermelha Apagada!");
  delay(5000); // Espera 5 segundos
}