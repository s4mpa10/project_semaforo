#ifndef WEBSERVER_HANDLER_H
#define WEBSERVER_HANDLER_H

#include <WebServer.h>
#include "RFIDManager.h" // Inclui a classe RFIDManager

class WebServerHandler {
public:
    WebServerHandler(WebServer& server, RFIDManager& rfidManager);
    void setupRoutes();

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

    // HTML da página web (pode ser movido para um arquivo .h separado)
    // const char* htmlContent = R"rawliteral(
    //   <!DOCTYPE HTML><html lang="pt-br">
    //   <head>
    //     <title>ESP32 Servidor RFID</title>
    //     <style>
    //       body { font-family: Arial, sans-serif; text-align: center; }
    //       h1 { color: #0F3376; padding: 2vh; }
    //       p { font-size: 1.5rem; }
    //       input { padding: 8px; font-size: 1rem; }
    //       button { padding: 10px 24px; font-size: 1rem; margin: 5px; }
    //     </style>
    //   </head>
    //   <body>
    //     <h1>ESP32 Servidor RFID</h1>
    //     <p id="status">Status: Aguardando tag...</p>
    //     <p id="uid_display">UID: N/A</p>
    //     <p id="name_display">Nome: N/A</p>
    //     <button onclick="readTag()">Ler RFID</button>
    //     <br>
    //     <hr>
    //     <h2>Gravar Nome na Tag</h2>
    //     <p>Digite o nome (max 16 caracteres):</p>
    //     <input type="text" id="nameInput" maxlength="16">
    //     <br>
    //     <button onclick="writeTag()">Gravar RFID</button>

    //     <script>
        //   function readTag() {
        //     document.getElementById('status').innerText = 'Lendo tag...';
        //     fetch('/read').then(response => response.json()).then(data => {
        //       document.getElementById('status').innerText = 'Status: ' + data.status;
        //       document.getElementById('uid_display').innerText = 'UID: ' + data.uid;
        //       document.getElementById('name_display').innerText = 'Nome: ' + data.name;
        //     });
        //   }

    //       function writeTag() {
    //         var name = document.getElementById('nameInput').value;
    //         if (name.length > 0) {
    //           document.getElementById('status').innerText = 'Iniciando gravação...';
    //           fetch('/write?name=' + encodeURIComponent(name)).then(response => response.text()).then(text => {
    //             document.getElementById('status').innerText = text;
    //           });
    //         } else {
    //           alert("Por favor, digite um nome para gravar.");
    //         }
    //       }

    //       // Auto-atualização de status a cada 2 segundos
    //       setInterval(readTag, 2000);
    //     </script>
    //   </body>
    //   </html>
    // )rawliteral";
};

#endif