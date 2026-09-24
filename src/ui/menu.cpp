#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "ui/menu.h"

// O(1)
Menu::Menu(Semaforo& semaforo) : semaforo_(semaforo) {}

// O(m + n)
void Menu::executar(){
    std::string linha;

    while(true){
        std::cout << "\n========= SEMÁFORO - VIA ÚNICA =========\n"
                  << " 1. Registrar chegada de veículo\n"
                  << " 2. Consultar primeiro da fila\n"
                  << " 3. Abrir sinal\n"
                  << " 4. Exibir veículos aguardando\n"
                  << " 5. Exibir quantidade de veículos na fila\n"
                  << " 0. Sair\n"
                  << "========================================\n";

        if(!this->lerLinha("Opção: ", linha)) break;

        int opcao;
        if(!paraInteiro(linha, opcao)){
            std::cout << "Opção inválida: digite um número de 0 a 5.\n";
            continue;
        }

        switch(opcao){
            case 1: this->registrarChegada();  break;
            case 2: this->consultarPrimeiro(); break;
            case 3: this->abrirSinal();        break;
            case 4: this->exibirAguardando();  break;
            case 5: this->exibirQuantidade();  break;
            case 0:
                std::cout << "Encerrando. " << this->semaforo_.quantidade()
                          << " veículo(s) ainda aguardavam na fila.\n";
                return;
            default:
                std::cout << "Opção inválida: digite um número de 0 a 5.\n";
        }
    }

    std::cout << "\nFim da entrada. Encerrando.\n";
}

// O(m)
void Menu::registrarChegada(){
    std::string linha;

    if(!this->lerLinha("Placa (ex.: ABC1234 ou ABC1D23): ", linha)) return;

    try {
        Placa placa(linha);

        std::cout << "Tipo: 1-Moto  2-Carro  3-Caminhão  4-Ônibus\n";
        if(!this->lerLinha("Tipo: ", linha)) return;

        int tipo;
        if(!paraInteiro(linha, tipo) || tipo < 1 || tipo > 4){
            std::cout << "Tipo inválido: digite um número de 1 a 4. Veículo não registrado.\n";
            return;
        }

        Veiculo veiculo = this->semaforo_.registrarChegada(placa, (TipoVeiculo) (tipo - 1));

        std::cout << "Veículo registrado no fim da fila: ";
        imprimirVeiculo(veiculo);
        std::cout << "Posição na fila: " << this->semaforo_.quantidade() << "\n";
    } catch(const std::invalid_argument& e) {
        std::cout << e.what() << " Veículo não registrado.\n";
    }
}

// O(1)
void Menu::consultarPrimeiro(){
    if(this->semaforo_.vazio()){
        std::cout << "A fila está vazia: não há veículo para consultar.\n";
        return;
    }

    std::cout << "Primeiro da fila (próximo a passar): ";
    imprimirVeiculo(this->semaforo_.primeiro());
}

// O(m + min(k, n))
void Menu::abrirSinal(){
    if(this->semaforo_.vazio()){
        std::cout << "Sinal aberto, mas não há veículos para liberar.\n";
        return;
    }

    std::string linha;
    if(!this->lerLinha("Quantos veículos poderão passar? ", linha)) return;

    int quantidade;
    if(!paraInteiro(linha, quantidade) || quantidade <= 0){
        std::cout << "Quantidade inválida: digite um número inteiro maior que zero.\n";
        return;
    }

    std::cout << "Sinal VERDE.\n";

    int liberados = this->semaforo_.abrirSinal(quantidade, [](const Veiculo& veiculo){
        std::cout << "  Liberado: ";
        imprimirVeiculo(veiculo);
    });

    if(liberados < quantidade){
        std::cout << "Foram solicitados " << quantidade << ", mas só havia " << liberados
                  << " veículo(s) na fila. Não há mais veículos para liberar.\n";
    }

    std::cout << "Sinal VERMELHO. " << this->semaforo_.quantidade()
              << " veículo(s) aguardando.\n";
}

// O(n)
void Menu::exibirAguardando(){
    if(this->semaforo_.vazio()){
        std::cout << "Não há veículos aguardando.\n";
        return;
    }

    std::cout << std::left
              << std::setw(6) << "Pos." << std::setw(8) << "Ordem"
              << std::setw(10) << "Placa" << "Tipo\n";

    this->semaforo_.paraCadaAguardando([](const Veiculo& veiculo, int posicao){
        std::cout << std::left
                  << std::setw(6) << posicao
                  << std::setw(8) << ("#" + std::to_string(veiculo.getOrdemChegada()))
                  << std::setw(10) << veiculo.getPlaca().getCode()
                  << tipoVeiculoToString(veiculo.getTipo()) << "\n";
    });
}

// O(1)
void Menu::exibirQuantidade(){
    std::cout << "Veículos na fila: " << this->semaforo_.quantidade() << "\n";
}

// O(m)
bool Menu::lerLinha(const std::string& prompt, std::string& linha){
    std::cout << prompt;

    if(!std::getline(std::cin, linha)){
        return false;
    }

    return true;
}

// O(m)
bool Menu::paraInteiro(const std::string& texto, int& valor){
    std::size_t inicio = texto.find_first_not_of(" \t\r");
    std::size_t fim = texto.find_last_not_of(" \t\r");
    if(inicio == std::string::npos) return false;

    std::string numero = texto.substr(inicio, fim - inicio + 1);

    try {
        std::size_t lidos;
        valor = std::stoi(numero, &lidos);
        return lidos == numero.size();
    } catch(const std::exception&) {
        return false;
    }
}

// O(1)
void Menu::imprimirVeiculo(const Veiculo& veiculo){
    std::cout << "#" << veiculo.getOrdemChegada() << " "
              << veiculo.getPlaca().getCode() << " ("
              << tipoVeiculoToString(veiculo.getTipo()) << ")\n";
}
