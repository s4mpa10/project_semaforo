#ifndef WEBSERVER_HANDLER_H
#define WEBSERVER_HANDLER_H

#include <WebServer.h>
#include "RFIDManager.h" 
#include "SemaforoHandler.h"

class WebServerHandler {
public:
    WebServerHandler(WebServer& server, RFIDManager& rfidManager, SemaforoHandler& semaforoHandler);
    void setupRoutes();
    bool handleFileRead(String path);

private:
    WebServer& server;
    RFIDManager& rfidManager;
    SemaforoHandler& semaforoHandler; 
    
    // Métodos para manipulação das requisições
    void handleRoot();
    void handleReadRFID();
    void handleWriteRFID();
    void handleOrdersPage();
    void handleStaticFile(const char* path, const char* contentType);
    void handleCadastroPage();

};

#endif