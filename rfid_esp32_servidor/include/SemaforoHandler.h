#ifndef SEMAFORO_HANDLER_H
#define SEMAFORO_HANDLER_H

#include <Arduino.h>
#include <WebServer.h>

enum SemaforoEstado {
    OFF,
    VERDE,
    AMARELO,
    VERMELHO,
    PISCA_ALERTA
};

class SemaforoHandler {
public:
    SemaforoHandler(WebServer& server);
    void setupRelays();
    void updateSemaforo();

    // Handlers para as rotas do servidor
    void handleIniciar();
    void handlePausar();
    void handleFinalizar();

private:
    WebServer& server;
    const int releVermelho = 26;
    const int releAmarelo = 25;
    const int releVerde = 27;

    SemaforoEstado estadoAtual = OFF;
    unsigned long tempoInicioTarefa = 0;
    bool tarefaPausada = true;

    // Métodos para controle dos relés
    void desligarTodosRele();
    void ligarReleVerde();
    void ligarReleAmarelo();
    void ligarReleVermelho();

};

#endif