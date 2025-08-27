#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "RFIDManager.h"
#include "WebServerHandler.h"
#include "SemaforoHandler.h"

// Substitua com as suas credenciais de rede
const char* ssid = "Sampaio";
const char* password = "03113012S";

// Pinagem do MFRC522
#define SS_PIN 5
#define RST_PIN 22

// Instâncias das classes
WebServer server(80);
RFIDManager rfidManager(SS_PIN, RST_PIN);
SemaforoHandler semaforoHandler(server);
WebServerHandler webServerHandler(server, rfidManager,semaforoHandler);

void setup() {
    Serial.begin(115200);

    // Inicializa o sistema de arquivos SPIFFS no início
    if (!SPIFFS.begin(true)) {
        Serial.println("Ocorreu um erro ao montar o SPIFFS. Verifique o upload do sistema de arquivos.");
        return;
    }
    Serial.println("SPIFFS montado com sucesso.");

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

    // Inicializa os relés e o RFID
    rfidManager.begin();
    webServerHandler.setupRoutes();
    semaforoHandler.setupRelays();


    server.onNotFound([]() {
        if (!webServerHandler.handleFileRead(server.uri())) {
        server.send(404, "text/plain", "404: Not Found");
        }
    });

    server.begin();
    Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();
  semaforoHandler.updateSemaforo();
}






