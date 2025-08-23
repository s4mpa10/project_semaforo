#include <WiFi.h>
#include <WebServer.h>

// Credenciais Wi-Fi
const char* ssid = "Diego";
const char* password = "pctop840";

// Instância do servidor
WebServer server(80);

// Pinos dos relés
const int releVermelho = 27;
const int releAmarelo = 26;
const int releVerde = 25;

bool estadoVermelho = false;
bool estadoAmarelo = false;
bool estadoVerde = false;

// Página HTML completa, incluindo CSS e JavaScript
const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
  <title>TaskFlow</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
      font-family: 'Segoe UI', sans-serif;
      background-color: #0d3a33;
      display: flex;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
      padding: 20px;
    }

    .wrapper {
      background-color: #12423a;
      padding: 40px 30px;
      border-radius: 20px;
      box-shadow: 0 0 20px rgba(0, 0, 0, 0.3);
      text-align: center;
      width: 700px;
    }

    .logos {
      display: flex;
      justify-content: center;
      align-items: center;
      margin-bottom: 10px;
    }

    .logos img {
      height: 60px;
      margin: 0 10px;
    }

    .wrapper h1 {
      color: white;
      font-size: 32px;
      margin-bottom: 30px;
    }

    .content {
      background-color: white;
      border-radius: 16px;
      padding: 24px;
      text-align: left;
    }

    .content h2 {
      color: #12423a;
      font-size: 22px;
      margin-bottom: 20px;
      text-align: center;
    }

    .card {
      background-color: #f4f4f4;
      border-radius: 12px;
      padding: 16px;
      margin-bottom: 16px;
      box-shadow: 0 0 8px rgba(0, 0, 0, 0.1);
    }

    .card-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    .card-header strong {
      font-size: 18px;
      color: #333;
    }

    .btn-detalhar {
      background-color: #2b57db;
      color: white;
      padding: 8px 12px;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      font-size: 14px;
    }

    .btn-detalhar:hover {
      background-color: #1f47c5;
    }

    .detalhes {
      display: none;
      margin-top: 15px;
      border-top: 1px solid #ccc;
      padding-top: 15px;
    }

    .detalhes span {
      display: block;
      margin-bottom: 8px;
      font-size: 15px;
    }

    .detalhes .boxe {
      color: #2b57db;
      font-weight: bold;
    }

    .acoes {
      display: flex;
      justify-content: space-between;
      margin-top: 10px;
      gap: 10px;
    }

    .acoes button {
      flex: 1;
      padding: 10px;
      font-size: 15px;
      border: none;
      border-radius: 6px;
      cursor: pointer;
      color: white;
    }

    .iniciar { background-color: #28a745; }
    .pausar { background-color: #ffc107; color: black; }
    .finalizar { background-color: #dc3545; }

    .iniciar:hover { background-color: #218838; }
    .pausar:hover { background-color: #e0a800; }
    .finalizar:hover { background-color: #c82333; }
  </style>
</head>
<body>
  <div class="wrapper">
    <div class="logos">
      <img src="./img/raspberry.png" alt="Pico">
      <img src="https://upload.wikimedia.org/wikipedia/en/c/cb/Raspberry_Pi_Logo.svg" alt="Raspberry Logo">
    </div>
    <h1>TaskFlow</h1>

    <div class="content">
      <h2>Ordens de Serviço</h2>

      <div class="card">
        <div class="card-header">
          <strong>Trocar Filtro de Óleo</strong>
          <button class="btn-detalhar" onclick="toggleDetalhes(this)">Detalhar</button>
        </div>
        <div class="detalhes">
          <span><strong>Descrição:</strong> Substituir filtro de óleo</span>
          <span><strong>Tempo estimado:</strong> 1 hora</span>
          <span><strong>Veículo:</strong> Palio 2012</span>
          <span><strong>Boxe de manutenção:</strong> <span class="boxe">Boxe 3</span></span>
          <span><strong>Mecânico:</strong> João</span>
          <div class="acoes">
            <button class="iniciar" onclick="toggleRele(3)">Iniciar</button>
            <button class="pausar" onclick="toggleRele(2)">Pausar</button>
            <button class="finalizar" onclick="toggleRele(1)">Finalizar</button>
          </div>
        </div>
      </div>

      <div class="card">
        <div class="card-header">
          <strong>Inspecionar Freios</strong>
          <button class="btn-detalhar" onclick="toggleDetalhes(this)">Detalhar</button>
        </div>
        <div class="detalhes">
          <span><strong>Descrição:</strong> Verificar pastilhas e discos</span>
          <span><strong>Tempo estimado:</strong> 1h30</span>
          <span><strong>Veículo:</strong> HB20 2020</span>
          <span><strong>Boxe de manutenção:</strong> <span class="boxe">Boxe 7</span></span>
          <span><strong>Mecânico:</strong> Maria</span>
          <div class="acoes">
            <button class="iniciar" onclick="toggleRele(3)">Iniciar</button>
            <button class="pausar" onclick="toggleRele(2)">Pausar</button>
            <button class="finalizar" onclick="toggleRele(1)">Finalizar</button>
          </div>
        </div>
      </div>

      <div class="card">
        <div class="card-header">
          <strong>Alinhamento</strong>
          <button class="btn-detalhar" onclick="toggleDetalhes(this)">Detalhar</button>
        </div>
        <div class="detalhes">
          <span><strong>Descrição:</strong> Verificação e ajuste da geometria das rodas</span>
          <span><strong>Tempo estimado:</strong> 2 horas</span>
          <span><strong>Veículo:</strong> Ford Ka 2015</span>
          <span><strong>Boxe de manutenção:</strong> <span class="boxe">Boxe 12</span></span>
          <span><strong>Mecânico:</strong> Pedro</span>
          <div class="acoes">
            <button class="iniciar" onclick="toggleRele(3)">Iniciar</button>
            <button class="pausar" onclick="toggleRele(2)">Pausar</button>
            <button class="finalizar" onclick="toggleRele(1)">Finalizar</button>
          </div>
        </div>
      </div>

    </div>
  </div>

  <script>
    function toggleDetalhes(botao) {
      const detalhes = botao.parentElement.nextElementSibling;
      const visivel = detalhes.style.display === 'block';
      detalhes.style.display = visivel ? 'none' : 'block';
      botao.textContent = visivel ? 'Detalhar' : 'Ver menos';
    }

    function toggleRele(num) {
      fetch('/toggle' + num)
        .then(response => {
          if (!response.ok) {
            throw new Error('Erro na requisição');
          }
          return response.text();
        })
        .then(state => {
          console.log('Relé ' + num + ' está ' + state);
        })
        .catch(error => console.error('Erro:', error));
    }
  </script>
</body>
</html>
)rawliteral";

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

void handleRoot() {
  server.send(200, "text/html", htmlContent);
}

void setup() {
  Serial.begin(115200);

  // Configura os pinos dos relés como saída
  pinMode(releVermelho, OUTPUT);
  pinMode(releAmarelo, OUTPUT);
  pinMode(releVerde, OUTPUT);

  // Desliga todos os relés no início
  digitalWrite(releVermelho, LOW);
  digitalWrite(releAmarelo, LOW);
  digitalWrite(releVerde, LOW);

  // Conecta ao Wi-Fi
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
  server.on("/", handleRoot);
  server.on("/toggle1", handleToggle1);
  server.on("/toggle2", handleToggle2);
  server.on("/toggle3", handleToggle3);

  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();
}