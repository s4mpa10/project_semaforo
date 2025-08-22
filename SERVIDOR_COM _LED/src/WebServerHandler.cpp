#include "WebServerHandler.h"
#include <SPIFFS.h>

WebServerHandler::WebServerHandler(WebServer& server, RFIDManager& rfidManager) 
    : server(server), rfidManager(rfidManager) {}

void WebServerHandler::setupRoutes() {
    // Liga as rotas aos métodos da classe
    server.on("/", [this]() {
        this->handleRoot();
    });

    server.on("/read", [this]() {
        this->handleReadRFID();
    });

    server.on("/write", [this]() {
        this->handleWriteRFID();
    });
    
    server.on("/ordens.html", [this]() { 
        this->handleOrdersPage();
    });
}

void WebServerHandler::handleRoot() {
    File file = SPIFFS.open("/inicial.html", "r");
    if (!file) {
        server.send(404, "text/plain", "Arquivo nao encontrado");
        return;
    }

    server.streamFile(file, "text/html");
    file.close();
}

void WebServerHandler::handleOrdersPage() { // Adicione a implementação
    File file = SPIFFS.open("/ordens.html", "r");
    if (!file) {
        server.send(404, "text/plain", "Arquivo nao encontrado");
        return;
    }

    server.streamFile(file, "text/html");
    file.close();
}

void WebServerHandler::handleReadRFID() {
    // A lógica de leitura do status pode ser mais complexa, mas vamos simplificar
    String rfidStatus = rfidManager.isNewCardPresentAndRead() ? "success" : "Aguardando tag...";
    
    String response = "{";
    response += "\"status\":\"" + rfidStatus + "\",";
    response += "\"uid\":\"" + rfidManager.getUID() + "\",";
    response += "\"name\":\"" + rfidManager.getName() + "\"";
    response += "}";
    server.send(200, "application/json", response);
}

void WebServerHandler::handleWriteRFID() {
    if (server.hasArg("name")) {
        String nameToSave = server.arg("name");
        
        // Esta abordagem é simplificada, pois o writeTag já tem o loop de espera.
        // A resposta inicial "Aproxime a tag" seria enviada antes, mas no
        // formato de classes, o fluxo de comunicação é mais complexo.
        if (rfidManager.writeName(nameToSave)) {
            server.send(200, "text/plain", "Dados gravados com sucesso!");
        } else {
            server.send(200, "text/plain", "Falha na gravação ou timeout.");
        }
    } else {
        server.send(400, "text/plain", "Erro: Nenhum dado de nome fornecido.");
    }
}

