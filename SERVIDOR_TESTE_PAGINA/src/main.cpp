#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

// Credenciais Wi-Fi
const char* ssid = "Moto g20";
const char* password = "@Dm1n202!";

// Instância do servidor
WebServer server(80);

// Pinos dos relés
const int releVermelho = 27;
const int releAmarelo = 26;
const int releVerde = 25;

bool estadoVermelho = false;
bool estadoAmarelo = false;
bool estadoVerde = false;

// ===================================
// FUNÇÕES AUXILIARES
// Mantenha todas elas antes do setup()
// ===================================

// Função para obter o tipo MIME do arquivo
String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "text/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  return "text/plain";
}

// Função auxiliar para carregar arquivos de forma mais genérica
bool handleFileRead(String path) {
  if (path.endsWith("/")) {
    path += "Inicial.HTML"; // Redireciona a raiz para Inicial.HTML
  }

  String contentType = getContentType(path);
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    size_t fileSize = file.size();
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

// Função para servir arquivos estáticos
void handleStaticFile(const char* path, const char* contentType) {
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "404: Not Found");
  }
}

// Handlers
void handleToggle1() {
  estadoVermelho = !estadoVermelho;
  digitalWrite(releVermelho, estadoVermelho ? HIGH : LOW);
  server.send(200, "text/plain", estadoVermelho ? "ON" : "OFF");
}

void handleToggle2() {
  estadoAmarelo = !estadoAmarelo;
  digitalWrite(releAmarelo, estadoAmarelo ? HIGH : LOW);
  server.send(200, "text/plain", estadoAmarelo ? "ON" : "OFF");
}

void handleToggle3() {
  estadoVerde = !estadoVerde;
  digitalWrite(releVerde, estadoVerde ? HIGH : LOW);
  server.send(200, "text/plain", estadoVerde ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);

  pinMode(releVermelho, OUTPUT);
  pinMode(releAmarelo, OUTPUT);
  pinMode(releVerde, OUTPUT);

  digitalWrite(releVermelho, LOW);
  digitalWrite(releAmarelo, LOW);
  digitalWrite(releVerde, LOW);

  if (!SPIFFS.begin(true)) {
    Serial.println("Ocorreu um erro ao montar o SPIFFS.");
    return;
  }
  Serial.println("SPIFFS montado com sucesso.");

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Rotas do servidor
  server.on("/", []() {
    handleStaticFile("/Inicial.HTML", "text/html");
  });

  server.on("/ordens.html", []() {
    handleStaticFile("/ordens.html", "text/html");
  });

  server.on("/cadastro.html", []() {
    handleStaticFile("/cadastro.html", "text/html");
  });

  // Rotas para arquivos de estilo e script
  server.on("/style_inicial.css", []() {
    handleStaticFile("/style_inicial.css", "text/css");
  });
  server.on("/style_ordens.css", []() {
    handleStaticFile("/style_ordens.css", "text/css");
  });
  server.on("/style_cadastro.css", []() {
    handleStaticFile("/style_cadastro.css", "text/css");
  });
  
  server.on("/script_ordens.js", []() {
    handleStaticFile("/script_ordens.js", "text/javascript");
  });
  server.on("/script_cadastro.js", []() {
    handleStaticFile("/script_cadastro.js", "text/javascript");
  });

  // Rotas para as imagens
  server.on("/img/raspberry.png", []() { handleStaticFile("/img/raspberry.png", "image/png"); });
  server.on("/img/toyota.png", []() { handleStaticFile("/img/toyota.png", "image/png"); });
  server.on("/img/IF.png", []() { handleStaticFile("/img/IF.png", "image/png"); });
  server.on("/img/embaraca2.png", []() { handleStaticFile("/img/embaraca2.png", "image/png"); });
  server.on("/img/semaforo_def.png", []() { handleStaticFile("/img/semaforo_def.png", "image/png"); });

  server.on("/toggle1", handleToggle1);
  server.on("/toggle2", handleToggle2);
  server.on("/toggle3", handleToggle3);

  // A rota 404 também precisa da função handleFileRead, que agora está acima
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "404: Not Found");
    }
  });

  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();
}