#include <stdio.h>

#include "entities/veiculo.h"
#include "vo/placa.vo.h"


Veiculo::Veiculo(const char* code, TipoVeiculo tipo){
    this->placa_ = new Placa();
    this->placa_->setCode(code);
    this->tipo_ = tipo;
}

Veiculo::Veiculo(const Veiculo & other){
    this->tipo_ = other.tipo_;
    this->placa_ = new Placa();
    this->placa_->setCode(other.placa_->getCode());
}

Placa* Veiculo::getPlaca(){ return this->placa_; }
TipoVeiculo Veiculo::getTipo(){ return this->tipo_; }