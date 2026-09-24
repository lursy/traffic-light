#include "entities/veiculo.h"

const char* tipoVeiculoToString(TipoVeiculo tipo){
    switch(tipo){
        case TipoVeiculo::MOTO:     return "Moto";
        case TipoVeiculo::CARRO:    return "Carro";
        case TipoVeiculo::CAMINHAO: return "Caminhão";
        case TipoVeiculo::ONIBUS:   return "Ônibus";
    }

    return "Desconhecido";
}

Veiculo::Veiculo(const Placa& placa, TipoVeiculo tipo, int ordemChegada)
    : placa_(placa), tipo_(tipo), ordemChegada_(ordemChegada) {}

const Placa& Veiculo::getPlaca() const { return this->placa_; }
TipoVeiculo Veiculo::getTipo() const { return this->tipo_; }
int Veiculo::getOrdemChegada() const { return this->ordemChegada_; }
