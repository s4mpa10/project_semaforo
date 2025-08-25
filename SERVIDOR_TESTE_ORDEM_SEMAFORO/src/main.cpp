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

// Estados do semáforo
enum SemaforoEstado {
  OFF,
  VERDE,
  AMARELO,
  VERMELHO,
  PISCA_ALERTA
};

SemaforoEstado estadoAtual = OFF;
unsigned long tempoInicioTarefa = 0;
bool tarefaPausada = false;

// ===================================
// FUNÇÕES AUXILIARES
// ===================================

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "text/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  return "text/plain";
}

bool handleFileRead(String path) {
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

void handleStaticFile(const char* path, const char* contentType) {
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "404: Not Found");
  }
}

// Funções de controle dos relés
void desligarTodosRele() {
  digitalWrite(releVermelho, LOW);
  digitalWrite(releAmarelo, LOW);
  digitalWrite(releVerde, LOW);
}

void ligarReleVerde() {
  desligarTodosRele();
  digitalWrite(releVerde, HIGH);
}

void ligarReleAmarelo() {
  desligarTodosRele();
  digitalWrite(releAmarelo, HIGH);
}

void ligarReleVermelho() {
  desligarTodosRele();
  digitalWrite(releVermelho, HIGH);
}

// ===================================
// HANDLERS DO SERVIDOR
// ===================================

void handleIniciar() {
  if (!tarefaPausada) {
    tempoInicioTarefa = millis();
    estadoAtual = VERDE;
    ligarReleVerde();
  } else {
    tempoInicioTarefa = millis() - (60000 - (tempoInicioTarefa + 60000 - millis()));
    tarefaPausada = false;
  }
  server.send(200, "text/plain", "Tarefa iniciada!");
}

void handlePausar() {
  tarefaPausada = true;
  desligarTodosRele();
  server.send(200, "text/plain", "Tarefa pausada.");
}

void handleFinalizar() {
  desligarTodosRele();
  estadoAtual = OFF;
  tarefaPausada = false;
  server.send(200, "text/plain", "Tarefa finalizada.");
}

void setup() {
  Serial.begin(115200);

  pinMode(releVermelho, OUTPUT);
  pinMode(releAmarelo, OUTPUT);
  pinMode(releVerde, OUTPUT);

  desligarTodosRele();

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

  server.on("/", []() {
    handleFileRead("/Inicial.HTML");
  });
  server.on("/iniciar", handleIniciar);
  server.on("/pausar", handlePausar);
  server.on("/finalizar", handleFinalizar);

  server.on("/style_inicial.css", []() { handleStaticFile("/style_inicial.css", "text/css"); });
  server.on("/style_ordens.css", []() { handleStaticFile("/style_ordens.css", "text/css"); });
  server.on("/style_cadastro.css", []() { handleStaticFile("/style_cadastro.css", "text/css"); });
  server.on("/script_ordens.js", []() { handleStaticFile("/script_ordens.js", "text/javascript"); });
  server.on("/script_cadastro.js", []() { handleStaticFile("/script_cadastro.js", "text/javascript"); });
  server.on("/img/raspberry.png", []() { handleStaticFile("/img/raspberry.png", "image/png"); });
  server.on("/img/toyota.png", []() { handleStaticFile("/img/toyota.png", "image/png"); });
  server.on("/img/IF.png", []() { handleStaticFile("/img/IF.png", "image/png"); });
  server.on("/img/embaraca2.png", []() { handleStaticFile("/img/embaraca2.png", "image/png"); });
  server.on("/img/semaforo_def.png", []() { handleStaticFile("/img/semaforo_def.png", "image/png"); });

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

  if (!tarefaPausada) {
    unsigned long tempoDecorrido = millis() - tempoInicioTarefa;

    switch (estadoAtual) {
      case VERDE:
        if (tempoDecorrido >= 50000 && tempoDecorrido < 60000) {
          estadoAtual = AMARELO;
          ligarReleAmarelo();
        } else if (tempoDecorrido >= 60000) {
          estadoAtual = VERMELHO;
          ligarReleVermelho();
        }
        break;

      case AMARELO:
        if (tempoDecorrido >= 60000) {
          estadoAtual = VERMELHO;
          ligarReleVermelho();
        }
        break;

      case VERMELHO:
        if (tempoDecorrido >= 65000) {
          estadoAtual = PISCA_ALERTA;
          desligarTodosRele();
        }
        break;

      case PISCA_ALERTA:
        if (millis() % 500 < 250) {
          digitalWrite(releVerde, HIGH);
          digitalWrite(releVermelho, LOW);
        } else {
          digitalWrite(releVerde, LOW);
          digitalWrite(releVermelho, HIGH);
        }
        digitalWrite(releAmarelo, LOW);
        break;

      case OFF:
        break;
    }
  }
}