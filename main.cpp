#include "services/semaforo.h"
#include "ui/menu.h"

// Simulação de semáforo de uma via com fila dinâmica (FIFO).
// A memória dos veículos restantes é liberada automaticamente pelo destrutor
// da fila quando `semaforo` sai de escopo.
int main(){
    Semaforo semaforo;
    Menu menu(semaforo);

    menu.executar();

    return 0;
}
