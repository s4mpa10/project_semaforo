#include <Arduino.h>
#include "config.h"
#include "RFIDManager.h"

// Cria uma instância da classe RFIDManager
RFIDManager rfidManager(SS_PIN, RST_PIN);

void setup() {
    Serial.begin(9600);
    while (!Serial);

    rfidManager.setup();
    // pinMode(PIN_GREEN, OUTPUT);
    // digitalWrite(PIN_GREEN, LOW);

    Serial.println(F("Sistema RFID pronto."));
    Serial.println(F("Aproxime uma tag para leitura."));
    Serial.println(F("Digite '1' e pressione Enter para entrar no modo de gravação."));
    Serial.println(F("-----------------------------------"));
}

void loop() {
    if (Serial.available() > 0) {
        char opcao = Serial.read();
        while(Serial.available()) { Serial.read(); }

        if (opcao == '1') {
            rfidManager.gravacaoDaTag();
            Serial.println(F("\nSistema RFID pronto."));
            Serial.println(F("Aproxime uma tag para leitura."));
            Serial.println(F("Digite '1' e pressione Enter para entrar no modo de gravação."));
            Serial.println(F("-----------------------------------"));
        }
    }

    if (rfidManager.isNewCardPresentAndRead()) {
        rfidManager.leituraDaTag();
        digitalWrite(PIN_GREEN, HIGH);
        Serial.println(F("Lâmpada verde ligado!"));
        delay(2000);
        // digitalWrite(PIN_GREEN, LOW);
        Serial.println(F("Lâmpada verde desligada!"));

        rfidManager.haltAndStop();
    }
    
    delay(50);
}