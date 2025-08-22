#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H

#include <MFRC522.h>
#include <SPI.h>
#include <Arduino.h>

class RFIDManager {
public:
    // Construtor
    RFIDManager(byte ssPin, byte rstPin);

    // Métodos públicos para a interface da classe
    void begin();
    bool isNewCardPresentAndRead();
    String getUID();
    String getName();
    bool writeName(const String& nameToSave);

private:
    // Atributos privados para encapsulamento
    MFRC522 rfid;
    MFRC522::MIFARE_Key key;
    String currentUid = "";
    String currentName = "";

    // Métodos auxiliares privados
    String getStatusCodeText(byte status);
};

#endif