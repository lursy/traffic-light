#include "vo/placa.vo.h"

#ifndef VEICULO_H
#define VEICULO_H

enum class TipoVeiculo {
    A,
    B,
    C,
};

class Veiculo {
    private:
        Placa *placa_;
        TipoVeiculo tipo_;
    
    public:
        Veiculo(const char *placa, TipoVeiculo tipo);
        Veiculo(const Veiculo & other);
        Placa* getPlaca();
        TipoVeiculo getTipo();
};

#endif