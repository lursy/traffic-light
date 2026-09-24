#ifndef SEMAFORO_H
#define SEMAFORO_H

#include "entities/veiculo.h"
#include "utils/queue.h"

class Semaforo {
    private:
        Queue<Veiculo> fila_;
        int proximaOrdem_ = 1;

    public:
        Veiculo registrarChegada(const Placa& placa, TipoVeiculo tipo);

        const Veiculo& primeiro() const;

        int abrirSinal(int quantidade, void (*aoLiberar)(const Veiculo&));

        void paraCadaAguardando(void (*visit)(const Veiculo&, int)) const;

        int quantidade() const;
        bool vazio() const;
};

#endif
