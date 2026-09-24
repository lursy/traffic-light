#include <stdexcept>

#include "services/semaforo.h"

// O(1)
Veiculo Semaforo::registrarChegada(const Placa& placa, TipoVeiculo tipo){
    Veiculo veiculo(placa, tipo, this->proximaOrdem_);

    this->fila_.enqueue(veiculo);
    this->proximaOrdem_++;

    return veiculo;
}

// O(1)
const Veiculo& Semaforo::primeiro() const { return this->fila_.front(); }

// O(min(k, n))
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

// O(n)
void Semaforo::paraCadaAguardando(void (*visit)(const Veiculo&, int)) const {
    this->fila_.forEach(visit);
}

// O(1)
int Semaforo::quantidade() const { return this->fila_.size(); }

// O(1)
bool Semaforo::vazio() const { return this->fila_.isEmpty(); }
