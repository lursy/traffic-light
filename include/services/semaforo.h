#ifndef SEMAFORO_H
#define SEMAFORO_H

#include "entities/veiculo.h"
#include "utils/queue.h"

// Semáforo de uma única via: mantém a fila de veículos aguardando e gera a
// ordem de chegada. Toda a política FIFO é garantida pela Queue.
class Semaforo {
    private:
        Queue<Veiculo> fila_;
        int proximaOrdem_ = 1;

    public:
        // Registra a chegada no fim da fila e devolve o veículo com a ordem gerada.
        Veiculo registrarChegada(const Placa& placa, TipoVeiculo tipo);

        // Primeiro da fila, sem removê-lo. Lança std::out_of_range se vazia.
        const Veiculo& primeiro() const;

        // Abre o sinal liberando até `quantidade` veículos, na ordem de chegada.
        // Chama aoLiberar(veiculo) para cada liberado e devolve quantos saíram
        // (menos que `quantidade` se a fila esvaziar antes).
        // Lança std::invalid_argument se quantidade <= 0.
        int abrirSinal(int quantidade, void (*aoLiberar)(const Veiculo&));

        // Percorre os veículos aguardando: visit(veiculo, posicao).
        void paraCadaAguardando(void (*visit)(const Veiculo&, int)) const;

        int quantidade() const;
        bool vazio() const;
};

#endif
