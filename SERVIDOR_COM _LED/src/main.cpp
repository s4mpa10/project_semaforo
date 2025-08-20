#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <MFRC522.h>

// Substitua com as suas credenciais de rede
const char* ssid = "S23 Ultra de Universitario";
const char* password = "senha123";

// Crie uma instância do servidor
WebServer server(80);

// Pinagem do MFRC522 para ESP32
#define SS_PIN 5  // (SDA/CS) Pino Slave Select padrão do SPI no ESP32
#define RST_PIN 22 // Pino de Reset. Pode ser qualquer GPIO livre.

// Crie uma instância do objeto MFRC522
MFRC522 rfid(SS_PIN, RST_PIN);

// Crie uma chave de acesso padrão para os cartões MIFARE
MFRC522::MIFARE_Key key;

// Define o bloco e o setor que usaremos para gravar e ler o nome
#define BLOCO_DADOS 4

// Variáveis globais para armazenar o status e o nome da tag
String rfidStatus = "Aguardando uma tag...";
String currentUid = "";
String currentName = "";

// Protótipos das funções
String getStatusCodeText(byte status);

// HTML content for the web page
const char* htmlContent = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 Servidor RFID</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; }
    h1 { color: #0F3376; padding: 2vh; }
    p { font-size: 1.5rem; }
    input { padding: 8px; font-size: 1rem; }
    button { padding: 10px 24px; font-size: 1rem; margin: 5px; }
  </style>
</head>
<body>
  <h1>ESP32 Servidor RFID</h1>
  <p id="status">Status: Aguardando tag...</p>
  <p id="uid_display">UID: N/A</p>
  <p id="name_display">Nome: N/A</p>
  <button onclick="readTag()">Ler RFID</button>
  <br>
  <hr>
  <h2>Gravar Nome na Tag</h2>
  <p>Digite o nome (máx 16 caracteres):</p>
  <input type="text" id="nameInput" maxlength="16">
  <br>
  <button onclick="writeTag()">Gravar RFID</button>

  <script>
    function readTag() {
      document.getElementById('status').innerText = 'Lendo tag...';
      fetch('/read').then(response => response.json()).then(data => {
        document.getElementById('status').innerText = 'Status: ' + data.status;
        document.getElementById('uid_display').innerText = 'UID: ' + data.uid;
        document.getElementById('name_display').innerText = 'Nome: ' + data.name;
      });
    }

    function writeTag() {
      var name = document.getElementById('nameInput').value;
      if (name.length > 0) {
        document.getElementById('status').innerText = 'Iniciando gravação...';
        fetch('/write?name=' + encodeURIComponent(name)).then(response => response.text()).then(text => {
          document.getElementById('status').innerText = text;
        });
      } else {
        alert("Por favor, digite um nome para gravar.");
      }
    }

    // Auto-atualização de status a cada 2 segundos
    setInterval(readTag, 2000);
  </script>
</body>
</html>
)rawliteral";


// Função para exibir a página web
void handleRoot() {
  server.send(200, "text/html", htmlContent);
}

// Nova função para ler a tag RFID
void handleReadRFID() {
  String response = "{";
  response += "\"status\":\"" + rfidStatus + "\",";
  response += "\"uid\":\"" + currentUid + "\",";
  response += "\"name\":\"" + currentName + "\"";
  response += "}";
  server.send(200, "application/json", response);
}

// Nova função para gravar a tag RFID
void handleWriteRFID() {
  if (server.hasArg("name")) {
    String nameToSave = server.arg("name");
    
    // Prepara o buffer para gravação
    byte buffer[16];
    memset(buffer, ' ', sizeof(buffer));
    nameToSave.getBytes(buffer, sizeof(buffer) + 1);

    // Espera por uma tag para gravação
    server.send(200, "text/plain", "Aproxime a tag para gravar...");

    // Implementação de timeout de 10 segundos para a gravação
    unsigned long tempoInicial = millis();
    const unsigned long tempoLimite = 10000;

    // Aguarda uma tag ser apresentada
    while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
      server.handleClient(); // Permite que o servidor continue respondendo outras requisições
      if (millis() - tempoInicial > tempoLimite) {
        server.send(200, "text/plain", "Timeout! Nenhuma tag detectada para gravação.");
        return;
      }
      delay(50);
    }

    // Autentica para escrita
    byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, BLOCO_DADOS, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
      server.send(200, "text/plain", "Falha na autenticação para escrita: " + getStatusCodeText(status));
      return;
    }

    // Grava os dados no bloco
    status = rfid.MIFARE_Write(BLOCO_DADOS, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
      server.send(200, "text/plain", "Falha ao gravar na tag: " + getStatusCodeText(status));
    } else {
      server.send(200, "text/plain", "Dados gravados com sucesso!");
    }

    // Finaliza a comunicação
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

  } else {
    server.send(400, "text/plain", "Erro: Nenhum dado de nome fornecido.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Conecta ao Wi-Fi
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

  // Inicia o barramento SPI
  SPI.begin();
  // Inicia o módulo MFRC522
  rfid.PCD_Init();
  
  // Preenche a chave com os valores padrão (0xFF)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("Sistema RFID e Servidor Web prontos."));
  
  // Rota para a página web
  server.on("/", handleRoot);

  // Rota para ler a tag RFID
  server.on("/read", handleReadRFID);

  // Rota para gravar a tag RFID
  server.on("/write", handleWriteRFID);

  // Inicia o servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void loop() {
  server.handleClient();
  
  // Lógica de leitura de tag para atualizar as variáveis globais
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    byte status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // 1. Autentica o bloco para leitura
    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, BLOCO_DADOS, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
      rfidStatus = "Falha na autenticação para leitura.";
    } else {
      // 2. Tenta ler os dados do bloco
      status = rfid.MIFARE_Read(BLOCO_DADOS, buffer, &size);
      if (status != MFRC522::STATUS_OK) {
        rfidStatus = "Falha na leitura dos dados.";
      } else {
        // 3. Atualiza as variáveis globais
        rfidStatus = "Tag detectada e lida com sucesso!";
        
        currentUid = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
          char temp[3];
          sprintf(temp, "%02X", rfid.uid.uidByte[i]);
          currentUid += temp;
        }

        currentName = "";
        for (byte i = 0; i < 16; i++) {
          char c = (char)buffer[i];
          if (isprint(c)) {
            currentName += c;
          }
        }
      }
    }
    
    // Coloca a tag em modo de espera
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

// Retorna uma string descritiva para um código de status do MFRC522
String getStatusCodeText(byte status) {
  switch (status) {
    case MFRC522::STATUS_OK: return "OK";
    case MFRC522::STATUS_ERROR: return "Erro de comunicação";
    case MFRC522::STATUS_COLLISION: return "Colisão de tag detectada";
    case MFRC522::STATUS_TIMEOUT: return "Timeout na comunicação";
    case MFRC522::STATUS_MIFARE_NACK: return "NACK recebido da tag";
    default: return "Erro desconhecido";
  }
}