#include <stdexcept>

#include "services/semaforo.h"

Veiculo Semaforo::registrarChegada(const Placa& placa, TipoVeiculo tipo){
    Veiculo veiculo(placa, tipo, this->proximaOrdem_);

    this->fila_.enqueue(veiculo);
    this->proximaOrdem_++;          // só avança se o enqueue deu certo

    return veiculo;
}

const Veiculo& Semaforo::primeiro() const { return this->fila_.front(); }

int Semaforo::abrirSinal(int quantidade, void (*aoLiberar)(const Veiculo&)){
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

void Semaforo::paraCadaAguardando(void (*visit)(const Veiculo&, int)) const {
    this->fila_.forEach(visit);
}

int Semaforo::quantidade() const { return this->fila_.size(); }

bool Semaforo::vazio() const { return this->fila_.isEmpty(); }
