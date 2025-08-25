#include "SemaforoHandler.h"

SemaforoHandler::SemaforoHandler(WebServer& server) : server(server) {}

void SemaforoHandler::setupRelays() {
    pinMode(releVermelho, OUTPUT);
    pinMode(releAmarelo, OUTPUT);
    pinMode(releVerde, OUTPUT);
    desligarTodosRele();
}

void SemaforoHandler::desligarTodosRele() {
    digitalWrite(releVermelho, LOW);
    digitalWrite(releAmarelo, LOW);
    digitalWrite(releVerde, LOW);
}

void SemaforoHandler::ligarReleVerde() {
    desligarTodosRele();
    digitalWrite(releVerde, HIGH);
}

void SemaforoHandler::ligarReleAmarelo() {
    desligarTodosRele();
    digitalWrite(releAmarelo, HIGH);
}

void SemaforoHandler::ligarReleVermelho() {
    desligarTodosRele();
    digitalWrite(releVermelho, HIGH);
}

void SemaforoHandler::setupRoutes() {
    server.on("/iniciar", [this]() {
        this->handleIniciar();
    });
    server.on("/pausar", [this]() {
        this->handlePausar();
    });
    server.on("/finalizar", [this]() {
        this->handleFinalizar();
    });
}

void SemaforoHandler::handleIniciar() {
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

void SemaforoHandler::handlePausar() {
    tarefaPausada = true;
    desligarTodosRele();
    server.send(200, "text/plain", "Tarefa pausada.");
}

void SemaforoHandler::handleFinalizar() {
    desligarTodosRele();
    estadoAtual = OFF;
    tarefaPausada = false;
    server.send(200, "text/plain", "Tarefa finalizada.");
}

void SemaforoHandler::updateSemaforo() {
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