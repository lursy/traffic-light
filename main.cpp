#include "services/semaforo.h"
#include "ui/menu.h"

// O(m + n)
int main(){
    Semaforo semaforo;
    Menu menu(semaforo);

    menu.executar();

    return 0;
}
