#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "RFIDManager.h"
#include "WebServerHandler.h"

// Substitua com as suas credenciais de rede
const char* ssid = "Sampaio";
const char* password = "03113012S";

// Pinagem do MFRC522
#define SS_PIN 5
#define RST_PIN 22

// Instâncias das classes
WebServer server(80);
RFIDManager rfidManager(SS_PIN, RST_PIN);
WebServerHandler webServerHandler(server, rfidManager);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Iniciando setup...");

    // Inicializa o sistema de arquivos SPIFFS no início
    if (!SPIFFS.begin(true)) {
        Serial.println("Ocorreu um erro ao montar o SPIFFS. Verifique o upload do sistema de arquivos.");
        return;
    }

    WiFi.begin(ssid, password);
    Serial.print("Conectando a ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi conectado.");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());

    rfidManager.begin();
    webServerHandler.setupRoutes();

    server.begin();
    Serial.println("Servidor HTTP iniciado.");
}

void loop() {
    server.handleClient();
    // A lógica de leitura em loop foi movida para o WebServerHandler
    // para ser acionada via requisição. Se precisar de uma leitura
    // contínua, você pode chamar o rfidManager.isNewCardPresentAndRead()
    // aqui também.
}