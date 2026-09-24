# Relatório — Tema 2: Simulação de Semáforo com Fila

**UNIFEI — ECOI2204 Algoritmos e Estrutura de Dados**
**Professor:** Dr. André Ribeiro de Brito

| Aluno | Matrícula |
|---|---|
| _Nome 1_ | _000000_ |
| _Nome 2_ | _000000_ |
| _Nome 3_ | _000000_ |

---

## 1. Descrição da solução

O programa simula um semáforo em uma via única. Os carros que chegam entram no fim de uma fila e, quando o sinal abre, o usuário diz quantos podem passar. Eles saem sempre pela frente da fila, então quem chegou primeiro passa primeiro (FIFO).

Para isso implementamos uma **fila dinâmica encadeada** do zero, sem usar nada pronto da STL. Dividimos o código em algumas classes, cada uma com uma função:

| Classe | Arquivos | O que faz |
|---|---|---|
| `Queue` | `utils/queue.h` e `.cpp` | A fila em si: nós ligados por ponteiros |
| `Placa` | `vo/placa.vo.h` e `.cpp` | Guarda a placa e confere se ela é válida (`ABC1234` ou `ABC1D23`) |
| `Veiculo` | `entities/veiculo.h` e `.cpp` | Placa, tipo (moto, carro, caminhão, ônibus) e ordem de chegada |
| `Semaforo` | `services/semaforo.h` e `.cpp` | Tem a fila e o contador da ordem de chegada |
| `Menu` | `ui/menu.h` e `.cpp` | Mostra o menu, lê o teclado e imprime os resultados |

O menu tem as opções pedidas no enunciado: registrar chegada, consultar o primeiro, abrir o sinal, listar a fila e mostrar a quantidade. Quando o usuário digita algo inválido (uma letra no lugar de um número, uma placa errada ou uma quantidade negativa), o programa avisa e volta para o menu, sem travar.

Algumas decisões que tomamos:

- **A fila guarda um ponteiro para o início e outro para o fim.** Assim, colocar um carro no fim não exige percorrer a fila inteira.
- **A fila tem um contador de quantidade** que é atualizado a cada entrada e saída, então não precisamos contar os nós toda vez.
- **A ordem de chegada é gerada pelo próprio sistema.** Um contador no `Semaforo` começa em 1 e só aumenta quando o carro realmente entra na fila.
- **A fila guarda uma cópia do veículo.** Assim, nada que aconteça com o objeto original afeta o que está na fila.

## 2. Representação da estrutura

Cada carro fica dentro de um **nó**, que guarda os dados do veículo e um ponteiro para o próximo nó. A fila guarda o endereço do primeiro nó (`head_`), o do último (`tail_`) e a quantidade de nós.

```cpp
struct Node {
    Veiculo value;   // dados do carro
    Node* next;      // próximo da fila
};
```

Uma fila com três carros fica assim:

```
 head_                               tail_
   ↓                                   ↓
 [#1 ABC1234] → [#2 BRA2E19] → [#3 XYZ9876] → nullptr
```

**Quando um carro chega (`enqueue`):** criamos um nó novo com `new`, ligamos o último nó a ele e ele passa a ser o `tail_`. Se a fila estava vazia, ele também vira o `head_`.

```
 [#1] → [#2] → [#3] → [#4 novo] → nullptr
```

**Quando o sinal abre (`dequeue`):** guardamos o primeiro nó, andamos o `head_` para o segundo e apagamos o primeiro com `delete`. Se era o último carro, o `tail_` também volta a ser `nullptr`.

```
 [#2] → [#3] → [#4] → nullptr
```

Todo nó criado com `new` é apagado com `delete`, seja quando o carro sai da fila, seja no fim do programa, quando o destrutor da fila apaga os que sobraram.

## 3. Testes executados

Fizemos os testes manualmente pelo menu, na sequência abaixo, começando com a fila vazia.

| # | Teste | O que fizemos | Resultado esperado | Obtido |
|---|---|---|---|---|
| 1 | Abrir o sinal com a fila vazia | Opção 3 logo ao iniciar | "não há veículos para liberar" | ☐ |
| 2 | Registrar vários veículos e conferir a ordem | Registramos `ABC1234`, `BRA2E19`, `XYZ9876`, `DEF5678` e usamos a opção 4 | Lista na ordem #1, #2, #3, #4 | ☐ |
| 3 | Consultar o primeiro sem alterar a fila | Opção 2 duas vezes, depois opção 5 | As duas vezes mostra `ABC1234`, e a quantidade continua 4 | ☐ |
| 4 | Liberar menos veículos do que existem | Opção 3 com quantidade `2` | Saem #1 e #2, e sobram 2 na fila | ☐ |
| 5 | Tentar liberar mais veículos do que existem | Opção 3 com quantidade `5` | Saem #3 e #4, e aparece o aviso de que não há mais veículos | ☐ |
| 6 | Entradas inválidas | Opção `9`; placa `AB12345`; registramos um carro e abrimos o sinal com quantidade `-1` | Mensagem de erro em cada caso e volta ao menu | ☐ |

_(Colar aqui os prints ou a saída do terminal de cada teste.)_

## 4. Análise de complexidade

Nas contas abaixo, **n** é o número de carros na fila e **k** é quantos carros o usuário pediu para passar.

**Registrar chegada (`enqueue`).** A operação fundamental é criar o nó e ligar os ponteiros. Como temos o ponteiro `tail_`, vamos direto ao fim da fila. O código só tem um `if` (fila vazia ou não), sem nenhum laço, então o custo é o mesmo com 1 ou com 1000 carros: **O(1)** em todos os casos.

**Liberar um carro (`dequeue`) e consultar o primeiro (`front`).** As duas mexem só no primeiro nó, que o `head_` já aponta. Só há `if`s simples, sem laço: **O(1)** em todos os casos.

**Quantidade (`size`).** Só devolve o contador: **O(1)**.

**Abrir o sinal (`abrirSinal`).** A operação fundamental é o `dequeue` dentro do laço:

```cpp
while(liberados < quantidade && !this->fila_.isEmpty()){   // roda min(k, n) vezes
    aoLiberar(this->fila_.dequeue());                      // O(1)
    liberados++;
}
```

O laço para quando saem k carros ou quando a fila acaba, o que acontecer primeiro.
- **Melhor caso:** k = 1, só um carro sai: **O(1)**.
- **Caso médio:** saem alguns carros: **O(k)**.
- **Pior caso:** k é maior ou igual ao tamanho da fila e todos saem: **O(n)**.

**Listar a fila (`forEach`).** Para mostrar todos os carros, o laço precisa passar por cada nó:

```cpp
for(Node* node = this->head_; node != nullptr; node = node->next){   // n vezes
    visit(node->value, position++);                                  // O(1)
}
```

É **O(n)** em todos os casos, porque sempre percorre a fila inteira.

**Apagar a fila (`clear`, chamado pelo destrutor).** Também é um laço com um `delete` por nó: **O(n)**.

| Operação | Melhor caso | Caso médio | Pior caso | Justificativa |
|---|---|---|---|---|
| Registrar chegada | O(1) | O(1) | O(1) | Vai direto ao fim pelo `tail_`, sem laço |
| Consultar primeiro | O(1) | O(1) | O(1) | Acessa direto o `head_` |
| Liberar um carro | O(1) | O(1) | O(1) | Remove sempre o primeiro nó |
| Quantidade | O(1) | O(1) | O(1) | Contador atualizado a cada entrada/saída |
| Abrir sinal (k carros) | O(1) | O(k) | O(n) | Laço roda até k vezes ou até a fila esvaziar |
| Listar a fila | O(n) | O(n) | O(n) | Laço passa por todos os nós |
| Apagar a fila | O(n) | O(n) | O(n) | Um `delete` por nó |

**Complexidade final:** as operações principais da fila (inserir, remover e consultar) são **O(1)**. As mais caras do programa são listar a fila e liberar todos os carros de uma vez, que são **O(n)**. A memória usada também cresce com n, porque é um nó por carro esperando.

## 5. Como compilar e executar

```bash
make run
```

Ou, sem `make`:

```bash
g++ -std=c++20 -Iinclude src/utils/queue.cpp src/vo/placa.cpp src/entities/veiculo.cpp \
    src/services/semaforo.cpp src/ui/menu.cpp main.cpp -o app
./app
```

Também deixamos o `Dockerfile` (entrega opcional): `docker compose run --rm app`.
