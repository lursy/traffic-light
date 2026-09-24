#ifndef MENU_H
#define MENU_H

#include <string>

#include "services/semaforo.h"

class Menu {
    private:
        Semaforo& semaforo_;

        void registrarChegada();
        void consultarPrimeiro();
        void abrirSinal();
        void exibirAguardando();
        void exibirQuantidade();

        bool lerLinha(const std::string& prompt, std::string& linha);

        static bool paraInteiro(const std::string& texto, int& valor);

        static void imprimirVeiculo(const Veiculo& veiculo);

    public:
        explicit Menu(Semaforo& semaforo);

        void executar();
};

#endif
