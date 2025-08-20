#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <string>
#include <config.h>

// Cria uma classe para gerenciar as operações do RFID
class RFIDManager {
public:
    // Construtor
    RFIDManager(byte ssPin, byte rstPin);

    // Funções públicas
    void setup();
    bool isNewCardPresentAndRead();
    void leituraDaTag();
    void gravacaoDaTag();
    void haltAndStop();

private:
    // Instância do objeto MFRC522
    MFRC522 _rfid;
    // Chave de acesso padrão
    MFRC522::MIFARE_Key _key;

    // Funções privadas (auxiliares)
    void printHex(byte *buffer, byte bufferSize);
    String getStatusCodeText(byte status);
};

#endif // RFID_MANAGER_H