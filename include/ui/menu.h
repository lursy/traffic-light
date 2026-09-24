#ifndef MENU_H
#define MENU_H

#include <string>

#include "services/semaforo.h"

// Interface textual do programa. Só faz entrada/saída: as regras ficam no
// Semaforo e a estrutura de dados na Queue.
class Menu {
    private:
        Semaforo& semaforo_;

        void registrarChegada();
        void consultarPrimeiro();
        void abrirSinal();
        void exibirAguardando();
        void exibirQuantidade();

        // Leitura de uma linha do teclado. Devolve false no fim da entrada
        // (Ctrl+D / Ctrl+Z), o que encerra o programa.
        bool lerLinha(const std::string& prompt, std::string& linha);

        // Converte `texto` (sem espaços nas pontas) em inteiro. Devolve false se
        // não for um número inteiro válido.
        static bool paraInteiro(const std::string& texto, int& valor);

        static void imprimirVeiculo(const Veiculo& veiculo);

    public:
        explicit Menu(Semaforo& semaforo);

        void executar();
};

#endif
