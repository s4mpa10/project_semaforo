#include "RFIDManager.h"
#include <Arduino.h>
#include "config.h"

#define BLOCO_DADOS 4

// Construtor
RFIDManager::RFIDManager(byte ssPin, byte rstPin) : _rfid(ssPin, rstPin) {
    // Inicializa a chave com valores padrão
    for (byte i = 0; i < 6; i++) {
        _key.keyByte[i] = 0xFF;
    }
}

// Inicializa o módulo
void RFIDManager::setup() {
    SPI.begin();
    _rfid.PCD_Init();
    Serial.println(F("Módulo RFID MFRC522 inicializado."));
}

// Verifica se um novo cartão está presente
bool RFIDManager::isNewCardPresentAndRead() {
    return _rfid.PICC_IsNewCardPresent() && _rfid.PICC_ReadCardSerial();
}

// Coloca a tag em modo de espera e encerra a criptografia
void RFIDManager::haltAndStop() {
    _rfid.PICC_HaltA();
    _rfid.PCD_StopCrypto1();
}

// Implementação da função de leitura da tag
void RFIDManager::leituraDaTag() {
    byte status;
    byte buffer[18];
    byte size = sizeof(buffer);

    status = _rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, BLOCO_DADOS, &_key, &(_rfid.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Falha na autenticação para leitura: "));
        Serial.println(getStatusCodeText(status));
        return;
    }

    status = _rfid.MIFARE_Read(BLOCO_DADOS, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Falha na leitura dos dados: "));
        Serial.println(getStatusCodeText(status));
        return;
    }

    Serial.print(F("UID:"));
    printHex(_rfid.uid.uidByte, _rfid.uid.size);
    Serial.println();
    
    Serial.print(F("Funcionário: "));
    for (byte i = 0; i < 18; i++) {
        char c = (char)buffer[i];
        if (isprint(c)) {
            Serial.print(c);
        }
    }
    Serial.println();
    Serial.println(F("Digite '1' e pressione Enter para entrar no modo de gravação."));
    Serial.println(F("-----------------------------------"));
}

// Implementação da função de gravação da tag
void RFIDManager::gravacaoDaTag() {
    byte status;
    byte buffer[18];
    String nome;

    Serial.println();
    Serial.println(F("--- MODO DE GRAVAÇÃO ---"));
    Serial.println(F("Digite o nome a ser gravado (max 18 caracteres) e pressione Enter:"));
    
    // Aguarda o usuário digitar
    while (Serial.available() == 0) {
        delay(100);
    }
    
    nome = Serial.readStringUntil('\n');
    nome.trim();

    Serial.println(F("Aproxime a tag para gravar..."));

    // Aguarda por uma tag com um timeout
    unsigned long startTime = millis();
    bool tagFound = false;
    while (!tagFound && (millis() - startTime < 10000)) { // 10 segundos de espera
        if (_rfid.PICC_IsNewCardPresent() && _rfid.PICC_ReadCardSerial()) {
            tagFound = true;
            Serial.print(F("Tag detectada! UID:"));
            printHex(_rfid.uid.uidByte, _rfid.uid.size);
            Serial.println();
        }
        delay(50);
    }
    
    if (!tagFound) {
        Serial.println(F("Nenhuma tag detectada."));
        return;
    }
    
    Serial.print(F("Tentando cadastrar o nome: '"));
    Serial.print(nome);
    Serial.println(F("'"));

    for (byte i = 0; i < 16; i++) {
        buffer[i] = ' '; 
    }
    
    for (byte i = 0; i < nome.length() && i < 16; i++) {
        buffer[i] = nome.charAt(i);
    }

    Serial.println(F("Iniciando autenticacao para escrita..."));
    status = _rfid.PCD_Authenticate(0x60, BLOCO_DADOS, &_key, &(_rfid.uid));
    Serial.print(F("Status autenticacao (HEX): 0x"));
    Serial.println(status, HEX);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Falha na autenticacao para cadastro: "));
        Serial.println(getStatusCodeText(status));
        haltAndStop();
        return;
    }
    Serial.println(F("Autenticacao OK."));

    Serial.println(F("Iniciando gravacao do bloco..."));
    status = _rfid.MIFARE_Write(BLOCO_DADOS, buffer, 16);
    Serial.print(F("Status gravacao (HEX): 0x"));
    Serial.println(status, HEX);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Falha no cadastro da tag: "));
        Serial.println(getStatusCodeText(status));
    } else {
        Serial.println(F("Cadastro realizado com sucesso!"));
        Serial.print(F("UID do Funcionario: "));
        printHex(_rfid.uid.uidByte, _rfid.uid.size);
        Serial.println();
        Serial.print(F("Nome Cadastrado: "));
        Serial.println(nome);
        Serial.println(F("-----------------------------------"));
    }

    haltAndStop();
}


// Funções auxiliares (privadas)
void RFIDManager::printHex(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

String RFIDManager::getStatusCodeText(byte status) {
    switch (status) {
        case MFRC522::STATUS_OK: return F("OK");
        case MFRC522::STATUS_ERROR: return F("Erro de comunicação");
        case MFRC522::STATUS_COLLISION: return F("Colisão de tag detectada");
        case MFRC522::STATUS_TIMEOUT: return F("Timeout na comunicação");
        case MFRC522::STATUS_MIFARE_NACK: return F("NACK recebido da tag");
        default: return F("Erro desconhecido");
    }
}