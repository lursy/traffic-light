#include "services/semaforo.h"

Veiculo Semaforo::registrarChegada(const Placa& placa, TipoVeiculo tipo){
    Veiculo veiculo(placa, tipo, this->proximaOrdem_);

    this->fila_.enqueue(veiculo);
    this->proximaOrdem_++;          // só avança se o enqueue deu certo

    return veiculo;
}

const Veiculo& Semaforo::primeiro() const { return this->fila_.front(); }

int Semaforo::quantidade() const { return this->fila_.size(); }

bool Semaforo::vazio() const { return this->fila_.isEmpty(); }
