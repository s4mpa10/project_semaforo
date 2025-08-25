#include "WebServerHandler.h"
#include <SPIFFS.h>

WebServerHandler::WebServerHandler(WebServer& server, RFIDManager& rfidManager) 
    : server(server), rfidManager(rfidManager) {}

void WebServerHandler::setupRoutes() {
    // Liga as rotas aos métodos da classe
    server.on("/", [this]() {
        this->handleRoot();
    });

    //Rotas arquivos(web)
    server.on("/read", [this]() {
        this->handleReadRFID();
    });

    server.on("/write", [this]() {
        this->handleWriteRFID();
    });

    //Rotas arquivos(.html)
    server.on("/ordens.html", [this]() { 
        this->handleOrdersPage();
    });

    server.on("/cadastro.html", [this]() {
        this->handleCadastroPage();
    });

    //Rotas img
    server.on("/img/raspberry.png", [this]() {
        this->handleStaticFile("/img/raspberry.png", "image/png");
    });

    server.on("/img/toyota.png", [this]() {
        this->handleStaticFile("/img/toyota.png", "image/png");
    });

    server.on("/img/IF.png", [this]() {
        this->handleStaticFile("/img/IF.png", "image/png");
    });

    server.on("/img/embaraca2.png", [this]() {
        this->handleStaticFile("/img/embaraca2.png", "image/png");
    });

    server.on("/img/semaforo_def.png", [this]() {
        this->handleStaticFile("/img/semaforo_def.png", "image/png");
    });

    //Rotas CSS
    server.on("/css/inicial.css", [this]() {
        this->handleStaticFile("/css/inicial.css", "text/css");
    });
    
    server.on("/css/cadastro.css", [this]() {
        this->handleStaticFile("/css/cadastro.css", "text/css");
    });

    server.on("/css/ordens.css", [this]() {
        this->handleStaticFile("/css/ordens.css", "text/css");
    });

    //Rotas JS
    server.on("/js/inicial.js", [this]() {
        this->handleStaticFile("/js/inicial.js", "application/javascript");
    });

    server.on("/js/cadastro.js", [this]() {
        this->handleStaticFile("/js/cadastro.js", "application/javascript");
    });

    server.on("/js/ordens.js", [this]() {
        this->handleStaticFile("/js/ordens.js", "application/javascript");
    });

}

// Implemente a função auxiliar handleStaticFile
void WebServerHandler::handleStaticFile(const char* path, const char* contentType) {
    File file = SPIFFS.open(path, "r");
    if (!file) {
        server.send(404, "text/plain", "Arquivo nao encontrado");
        return;
    }
    server.streamFile(file, contentType);
    file.close();
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

void WebServerHandler::handleOrdersPage() { 
    File file = SPIFFS.open("/ordens.html", "r");
    if (!file) {
        server.send(404, "text/plain", "Arquivo nao encontrado");
        return;
    }

    server.streamFile(file, "text/html");
    file.close();
}

void WebServerHandler::handleCadastroPage() {
    File file = SPIFFS.open("/cadastro.html", "r");
    if(!file) {
        server.send(404, "text/plain", "Arquivo nao encontrado");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
}

void WebServerHandler::handleReadRFID() {
    // A lógica de leitura do status pode ser mais complexa, mas vamos simplificar
    String rfidStatus = rfidManager.isNewCardPresentAndRead() ? "success" : "waiting";
    
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

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "text/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  return "text/plain";
}


bool WebServerHandler::handleFileRead(String path) {
  if (path.endsWith("/")) {
    path += "Inicial.HTML"; // Redireciona a raiz para Inicial.HTML
  }

  // Certifica-se de que a página HTML é sempre enviada como "text/html"
  if (path.endsWith("Inicial.HTML")) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, "text/html");
    file.close();
    return true;
  }
  
  String contentType = getContentType(path);
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}