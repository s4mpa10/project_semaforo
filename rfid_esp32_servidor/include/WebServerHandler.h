#ifndef WEBSERVER_HANDLER_H
#define WEBSERVER_HANDLER_H

#include <WebServer.h>
#include "RFIDManager.h" // Inclui a classe RFIDManager

class WebServerHandler {
public:
    WebServerHandler(WebServer& server, RFIDManager& rfidManager);
    void setupRoutes();
    bool handleFileRead(String path);

private:
    WebServer& server;
    RFIDManager& rfidManager;
    
    // Métodos para manipulação das requisições
    void handleRoot();
    void handleReadRFID();
    void handleWriteRFID();
    void handleOrdersPage();
    void handleStaticFile(const char* path, const char* contentType);
    void handleCadastroPage();

};

#endif