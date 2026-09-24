#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <stdexcept>

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
        template <typename Callback>
        int abrirSinal(int quantidade, Callback aoLiberar);

        // Percorre os veículos aguardando: visit(veiculo, posicao).
        template <typename Visitor>
        void paraCadaAguardando(Visitor visit) const;

        int quantidade() const;
        bool vazio() const;
};

template <typename Callback>
int Semaforo::abrirSinal(int quantidade, Callback aoLiberar){
    if(quantidade <= 0){
        throw std::invalid_argument("A quantidade deve ser maior que zero.");
    }

    int liberados = 0;

    while(liberados < quantidade && !this->fila_.isEmpty()){
        aoLiberar(this->fila_.dequeue());
        liberados++;
    }

    return liberados;
}

template <typename Visitor>
void Semaforo::paraCadaAguardando(Visitor visit) const {
    this->fila_.forEach(visit);
}

#endif
