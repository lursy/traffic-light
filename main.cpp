#include <iostream>
#include <stdio.h>

#include "entities/veiculo.h"
#include "utils/queue.h"

int main(){
    char* code = new char[50];
    Veiculo* carro = new Veiculo("test-12", TipoVeiculo::B);
    Veiculo* moto = new Veiculo("test-23", TipoVeiculo::A);
    Veiculo* caminhao = new Veiculo("test-45", TipoVeiculo::C);
    Queue semaforo = Queue<Veiculo>(3);

    Veiculo moto2 = *moto;
    semaforo.enqueue(moto);
    semaforo.enqueue(&moto2);
    
    printf("ADDRESS: [%p] - [%p]\n", moto, &moto2);

    
    // semaforo.enqueue(carro);
    // semaforo.enqueue(caminhao);
    // semaforo.dequeue();
    // semaforo.enqueue(moto);
    // std::cin.getline(code, 50);
    // Veiculo carro = Veiculo(code, TipoVeiculo::B);

    // printf("%s, %ld\n", carro.getPlaca().getCode(), sizeof(*code));

    // delete[] code;
    return 0;
}