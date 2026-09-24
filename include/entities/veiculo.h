#ifndef VEICULO_H
#define VEICULO_H

#include "vo/placa.vo.h"

enum class TipoVeiculo {
    MOTO,
    CARRO,
    CAMINHAO,
    ONIBUS,
};

const char* tipoVeiculoToString(TipoVeiculo tipo);

class Veiculo {
    private:
        Placa placa_;
        TipoVeiculo tipo_;
        int ordemChegada_;

    public:
        Veiculo(const Placa& placa, TipoVeiculo tipo, int ordemChegada);

        const Placa& getPlaca() const;
        TipoVeiculo getTipo() const;
        int getOrdemChegada() const;
};

#endif
