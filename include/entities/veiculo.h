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

// Veículo aguardando no semáforo. Todos os membros são valores (sem ponteiros),
// então as cópias feitas pela fila são independentes e nada precisa ser
// liberado manualmente.
class Veiculo {
    private:
        Placa placa_;
        TipoVeiculo tipo_;
        int ordemChegada_;      // gerada pelo sistema (Semaforo), começa em 1

    public:
        Veiculo(const Placa& placa, TipoVeiculo tipo, int ordemChegada);

        const Placa& getPlaca() const;
        TipoVeiculo getTipo() const;
        int getOrdemChegada() const;
};

#endif
