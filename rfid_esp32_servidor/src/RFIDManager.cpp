#include "RFIDManager.h"

#define DATA_BLOCK 4

RFIDManager::RFIDManager(byte ssPin, byte rstPin) : rfid(ssPin, rstPin) {
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
}

void RFIDManager::begin() {
    SPI.begin();
    rfid.PCD_Init();
    Serial.println(F("Módulo MFRC522 inicializado."));
}

bool RFIDManager::isNewCardPresentAndRead() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return false;
    }

    byte status;
    byte buffer[18];
    byte size = sizeof(buffer);

    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, DATA_BLOCK, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
        currentName = "Falha na autenticação para leitura.";
    } else {
        status = rfid.MIFARE_Read(DATA_BLOCK, buffer, &size);
        if (status != MFRC522::STATUS_OK) {
            currentName = "Falha na leitura dos dados.";
        } else {
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
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return true;
}

String RFIDManager::getUID() {
    return currentUid;
}

String RFIDManager::getName() {
    currentName.trim();
    return currentName;
}

bool RFIDManager::writeName(const String& nameToSave) {
    byte buffer[16];
    memset(buffer, ' ', sizeof(buffer));
    nameToSave.getBytes(buffer, sizeof(buffer) + 1);

    unsigned long tempoInicial = millis();
    const unsigned long tempoLimite = 10000;

    while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        if (millis() - tempoInicial > tempoLimite) {
            return false; // Timeout
        }
        delay(50);
    }
    
    byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, DATA_BLOCK, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
        return false; // Falha na autenticação
    }

    status = rfid.MIFARE_Write(DATA_BLOCK, buffer, 16);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    
    return status == MFRC522::STATUS_OK;
}

String RFIDManager::getStatusCodeText(byte status) {
  switch (status) {
    case MFRC522::STATUS_OK: return "OK";
    case MFRC522::STATUS_ERROR: return "Erro de comunicação";
    case MFRC522::STATUS_COLLISION: return "Colisão de tag detectada";
    case MFRC522::STATUS_TIMEOUT: return "Timeout na comunicação";
    case MFRC522::STATUS_MIFARE_NACK: return "NACK recebido da tag";
    default: return "Erro desconhecido";
  }
}