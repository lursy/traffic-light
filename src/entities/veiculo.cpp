#include "entities/veiculo.h"

// O(1)
const char* tipoVeiculoToString(TipoVeiculo tipo){
    switch(tipo){
        case TipoVeiculo::MOTO:     return "Moto";
        case TipoVeiculo::CARRO:    return "Carro";
        case TipoVeiculo::CAMINHAO: return "Caminhão";
        case TipoVeiculo::ONIBUS:   return "Ônibus";
    }

    return "Desconhecido";
}

// O(1)
Veiculo::Veiculo(const Placa& placa, TipoVeiculo tipo, int ordemChegada)
    : placa_(placa), tipo_(tipo), ordemChegada_(ordemChegada) {}

// O(1)
const Placa& Veiculo::getPlaca() const { return this->placa_; }
// O(1)
TipoVeiculo Veiculo::getTipo() const { return this->tipo_; }
// O(1)
int Veiculo::getOrdemChegada() const { return this->ordemChegada_; }
