# Relatório Técnico — Tema 2: Simulação de Semáforo com Fila

**Universidade Federal de Itajubá — Instituto de Ciências Tecnológicas**
**Disciplina:** ECOI2204 – Algoritmos e Estrutura de Dados
**Professor:** Dr. André Ribeiro de Brito

| Aluno | Matrícula |
|---|---|
| _Nome 1_ | _000000_ |
| _Nome 2_ | _000000_ |
| _Nome 3_ | _000000_ |

---

## Sumário

1. [Objetivo](#1-objetivo)
2. [Descrição da solução](#2-descrição-da-solução)
3. [Representação da estrutura](#3-representação-da-estrutura)
4. [Atendimento aos requisitos](#4-atendimento-aos-requisitos)
5. [Compilação e execução](#5-compilação-e-execução)
6. [Testes executados](#6-testes-executados)
7. [Análise de complexidade](#7-análise-de-complexidade)
8. [Gerenciamento de memória](#8-gerenciamento-de-memória)

---

## 1. Objetivo

O programa simula uma via única controlada por um semáforo, em modo texto. Os veículos que chegam entram no fim de uma fila. Quando o sinal abre, o usuário informa quantos veículos podem passar, e eles saem pelo início da fila, respeitando rigorosamente a política **FIFO** (*First In, First Out*): o primeiro a chegar é o primeiro a atravessar.

Toda a solução usa **uma única estrutura de dados**: uma **fila dinâmica simples**, implementada manualmente como lista encadeada, sem nenhum contêiner da STL.

---

## 2. Descrição da solução

### 2.1 Organização em camadas

O código foi dividido em camadas. Cada camada tem uma única responsabilidade, e as dependências vão sempre de cima para baixo:

```
 main.cpp
    │
    ▼
 ui/Menu ─────────────── entrada e saída (menu textual, leitura e validação do teclado)
    │
    ▼
 services/Semaforo ───── regras do semáforo (ordem de chegada, abrir sinal)
    │
    ├──► utils/Queue<T> ─ ESTRUTURA DE DADOS: fila dinâmica encadeada (genérica)
    │
    ▼
 entities/Veiculo ────── dados do veículo (placa, tipo, ordem de chegada)
    │
    ▼
 vo/Placa ────────────── value object: placa validada (ABC1234 ou ABC1D23)
```

| Arquivo | Responsabilidade |
|---|---|
| `include/utils/queue.h` | Classe template `Queue<T>`: a fila dinâmica. Por ser template, a implementação inteira fica no header. |
| `include/vo/placa.vo.h`, `src/vo/placa.cpp` | Classe `Placa`: normaliza a entrada (remove hífen e espaços, converte para maiúsculas) e valida o formato. |
| `include/entities/veiculo.h`, `src/entities/veiculo.cpp` | Classe `Veiculo` e enum `TipoVeiculo` (Moto, Carro, Caminhão, Ônibus). |
| `include/services/semaforo.h`, `src/services/semaforo.cpp` | Classe `Semaforo`: contém **a única fila** do programa e o contador de ordem de chegada. |
| `include/ui/menu.h`, `src/ui/menu.cpp` | Classe `Menu`: exibe o menu, lê e valida as entradas e mostra os resultados. |
| `main.cpp` | Cria o `Semaforo` e o `Menu` e inicia o laço do menu. |
| `Makefile`, `Dockerfile`, `docker-compose.yml` | Compilação local e em contêiner (entrega opcional 4). |

### 2.2 Principais decisões de implementação

**Fila genérica (`Queue<T>`).** A estrutura não sabe nada sobre veículos. Ela funciona para qualquer tipo `T`, o que separa a estrutura de dados (avaliada no trabalho) da regra de negócio do semáforo.

**A fila armazena cópias (semântica de valor).** `enqueue(const T&)` copia o objeto para dentro de um nó novo, e `dequeue()` devolve uma cópia e libera o nó. Assim a fila é dona dos próprios dados, e nenhum objeto externo pode ser apagado enquanto ainda está na fila, o que deixaria um ponteiro inválido lá dentro.

**Ponteiros para o início (`head_`) e para o fim (`tail_`).** Com o ponteiro para o fim, a inserção é O(1), sem precisar percorrer a lista até o último nó.

**Contador `length_`.** A quantidade de elementos é atualizada a cada `enqueue`/`dequeue`, então consultar o tamanho custa O(1) em vez de O(n).

**Ordem de chegada gerada pelo sistema.** O `Semaforo` tem um contador `proximaOrdem_` que começa em 1 e só avança depois que o veículo entra na fila com sucesso. Um registro rejeitado (placa ou tipo inválido) não consome número. O contador nunca é reiniciado, então a ordem continua crescente mesmo depois de liberações.

**Placa como *value object*.** A validação fica no construtor de `Placa`, então todo objeto `Placa` existente é, por construção, uma placa válida. Formatos aceitos:

| Formato | Exemplo | Regra (posições 0–6) |
|---|---|---|
| Antigo | `ABC1234` ou `ABC-1234` | 3 letras, 4 dígitos |
| Mercosul | `ABC1D23` | 3 letras, dígito, letra, 2 dígitos |

A entrada é normalizada antes da validação: `abc-1234` é aceita e armazenada como `ABC1234`.

**Tratamento de erros em duas camadas.** A `Queue` e o `Semaforo` lançam exceções (`std::out_of_range`, `std::invalid_argument`) quando recebem uma operação inválida, e nunca devolvem lixo. O `Menu` verifica as condições antes de chamar essas classes (por exemplo, se a fila está vazia) e mostra mensagens claras ao usuário. Entradas não numéricas, fora do intervalo ou com lixo no final (`3abc`) são rejeitadas, e o programa volta ao menu. O fim da entrada (Ctrl+D/Ctrl+Z) encerra o programa de forma limpa.

**Bibliotecas utilizadas.** `iostream`, `iomanip`, `string`, `stdexcept`, `cctype` (`toupper`, `isalpha`…) e `utility` (`std::swap`). Nenhuma delas é um contêiner nem substitui a fila implementada.

---

## 3. Representação da estrutura

### 3.1 Nó e fila

```cpp
template <typename T>
class Queue {
    private:
        struct Node {
            T value;        // cópia do veículo
            Node* next;     // próximo da fila (nullptr no último)
        };

        Node* head_ = nullptr;   // início: próximo a sair
        Node* tail_ = nullptr;   // fim: onde entram os novos
        int length_ = 0;         // quantidade de nós
    ...
};
```

Cada nó é alocado com `new` quando o veículo chega e liberado com `delete` quando ele sai.

### 3.2 Estados da fila

**Fila vazia:** `head_` e `tail_` valem `nullptr` e `length_ = 0`.

```
 head_ ──► nullptr
 tail_ ──► nullptr          length_ = 0
```

**Depois de registrar #1 ABC1234, #2 BRA2E19 e #3 XYZ9876:**

```
 head_                                   tail_
   │                                       │
   ▼                                       ▼
 ┌──────────────┬───┐   ┌──────────────┬───┐   ┌──────────────┬───┐
 │ #1 ABC1234   │ ●─┼──►│ #2 BRA2E19   │ ●─┼──►│ #3 XYZ9876   │ ╳ │   length_ = 3
 │ Carro        │   │   │ Moto         │   │   │ Caminhão     │   │
 └──────────────┴───┘   └──────────────┴───┘   └──────────────┴───┘
      value      next
```

### 3.3 Inserção no fim (`enqueue`): chegada de #4 DEF5678

```
 1) novo = new Node(#4)                     ┌──────────────┬───┐
                                            │ #4 DEF5678   │ ╳ │
                                            └──────────────┴───┘
 2) tail_->next = novo
 3) tail_ = novo;  length_++

 head_                                                          tail_
   ▼                                                              ▼
 [#1] ──► [#2] ──► [#3] ──► [#4] ──► nullptr                  length_ = 4
```

Caso especial: se a fila estava vazia (`tail_ == nullptr`), o novo nó passa a ser também o `head_`.

### 3.4 Remoção do início (`dequeue`): o sinal abre para 1 veículo

```
 1) no = head_                (guarda o nó #1)
 2) valor = no->value         (copia o veículo para devolver)
 3) head_ = no->next          (o #2 passa a ser o primeiro)
 4) delete no;  length_--

 head_                                    tail_
   ▼                                        ▼
 [#2] ──► [#3] ──► [#4] ──► nullptr                  length_ = 3
```

Caso especial: se o nó removido era o único, `head_` vira `nullptr`, e `tail_` também precisa ser zerado. Caso contrário, `tail_` ficaria apontando para memória liberada.

---

## 4. Atendimento aos requisitos

| # | Funcionalidade obrigatória | Menu | Implementação |
|---|---|---|---|
| 1 | Registrar chegada no final da fila | 1 | `Menu::registrarChegada` → `Semaforo::registrarChegada` → `Queue::enqueue` |
| 2 | Consultar o início sem remover | 2 | `Menu::consultarPrimeiro` → `Semaforo::primeiro` → `Queue::front` |
| 3 | Abrir o sinal e solicitar quantos passarão | 3 | `Menu::abrirSinal` pergunta a quantidade |
| 4 | Liberar na ordem de chegada | 3 | `Semaforo::abrirSinal` → `Queue::dequeue` repetido |
| 5 | Exibir todos que aguardam | 4 | `Menu::exibirAguardando` → `Queue::forEach` |
| 6 | Exibir a quantidade na fila | 5 | `Menu::exibirQuantidade` → `Queue::size` |
| 7 | Informar quando não houver veículos para liberar | 3 | Mensagem para fila vazia e para pedidos maiores que a fila |

| Requisito / restrição | Como foi atendido |
|---|---|
| C++ | C++20, compilado com `g++ -Wall -Wextra -pedantic` sem avisos |
| Estrutura implementada manualmente | `Queue<T>` encadeada, com `new`/`delete` explícitos |
| Uma única via e uma única fila | O `Semaforo` tem um único membro `Queue<Veiculo> fila_` |
| Sem contêineres da STL | Nenhum `vector`, `list`, `queue`, `deque` etc. (ver §2.2) |
| Dados em memória durante a execução | A fila vive enquanto o objeto `semaforo` do `main` existir |
| Todo `new` liberado com `delete` | Ver §8 |
| Menu textual e tratamento de operações inválidas ou sobre fila vazia | Ver §2.2 e testes 6.6 a 6.9 |
| Dados do veículo: placa, tipo e ordem de chegada | Classe `Veiculo`; a ordem é gerada pelo `Semaforo` |

---

## 5. Compilação e execução

**Com `make`** (Linux, WSL ou MinGW):

```bash
make          # compila ./app
make run      # compila e executa
make debug    # compila com AddressSanitizer/UBSan (detecta vazamentos de memória)
make clean
```

**Sem `make`:**

```bash
g++ -std=c++20 -Wall -Wextra -Iinclude src/vo/placa.cpp src/entities/veiculo.cpp \
    src/services/semaforo.cpp src/ui/menu.cpp main.cpp -o app
./app
```

**Com Docker** (entrega opcional):

```bash
docker compose run --rm app
```

---

## 6. Testes executados

> **Roteiro para execução manual.** Execute cada teste pelo menu, compare com o resultado esperado e registre o resultado obtido (texto ou print da tela). Os testes 6.1 a 6.5 são os **testes mínimos** do enunciado.

### 6.1 Registrar vários veículos e conferir a ordem exibida

| Passo | Opção | Entrada |
|---|---|---|
| 1 | 1 | `ABC1234`, tipo `2` (Carro) |
| 2 | 1 | `BRA2E19`, tipo `1` (Moto) |
| 3 | 1 | `xyz-9876`, tipo `3` (Caminhão) |
| 4 | 1 | `DEF5678`, tipo `4` (Ônibus) |
| 5 | 4 | — |

**Esperado:** cada registro mostra a ordem de chegada (#1 a #4) e a posição na fila. A opção 4 lista os veículos na mesma ordem em que foram registrados. A placa `xyz-9876` aparece normalizada como `XYZ9876`.

```
Pos.  Ordem   Placa     Tipo
1     #1      ABC1234   Carro
2     #2      BRA2E19   Moto
3     #3      XYZ9876   Caminhão
4     #4      DEF5678   Ônibus
```

**Resultado obtido:** ☐ conforme esperado — _(anexar saída/print)_

### 6.2 Consultar o primeiro veículo sem alterar a fila

Continuando do teste 6.1:

| Passo | Opção | Entrada |
|---|---|---|
| 1 | 2 | — |
| 2 | 2 | — (consultar de novo) |
| 3 | 5 | — |

**Esperado:** as duas consultas mostram `Primeiro da fila (próximo a passar): #1 ABC1234 (Carro)`, e a opção 5 continua mostrando `Veículos na fila: 4`. A consulta não remove nada.

**Resultado obtido:** ☐ conforme esperado — _(anexar saída/print)_

### 6.3 Abrir o sinal para liberar menos veículos do que existem

Continuando do teste 6.2 (4 veículos na fila):

| Passo | Opção | Entrada |
|---|---|---|
| 1 | 3 | quantidade `2` |
| 2 | 4 | — |

**Esperado:**

```
Sinal VERDE.
  Liberado: #1 ABC1234 (Carro)
  Liberado: #2 BRA2E19 (Moto)
Sinal VERMELHO. 2 veículo(s) aguardando.
```

A opção 4 mostra apenas #3 e #4, agora nas posições 1 e 2.

**Resultado obtido:** ☐ conforme esperado — _(anexar saída/print)_

### 6.4 Tentar liberar mais veículos do que existem

Continuando do teste 6.3 (2 veículos na fila):

| Passo | Opção | Entrada |
|---|---|---|
| 1 | 3 | quantidade `5` |

**Esperado:** os 2 veículos restantes são liberados em ordem, e o programa avisa que não há mais veículos:

```
Sinal VERDE.
  Liberado: #3 XYZ9876 (Caminhão)
  Liberado: #4 DEF5678 (Ônibus)
Foram solicitados 5, mas só havia 2 veículo(s) na fila. Não há mais veículos para liberar.
Sinal VERMELHO. 0 veículo(s) aguardando.
```

**Resultado obtido:** ☐ conforme esperado — _(anexar saída/print)_

### 6.5 Abrir o sinal com a fila vazia

Com a fila vazia (logo ao iniciar o programa ou depois do teste 6.4):

| Passo | Opção | Entrada |
|---|---|---|
| 1 | 3 | — |
| 2 | 2 | — |
| 3 | 4 | — |

**Esperado:** as mensagens `Sinal aberto, mas não há veículos para liberar.`, `A fila está vazia: não há veículo para consultar.` e `Não há veículos aguardando.`, sem erro e sem encerrar o programa.

**Resultado obtido:** ☐ conforme esperado — _(anexar saída/print)_

### 6.6 Opções de menu inválidas

| Entrada na opção | Esperado |
|---|---|
| `9` | `Opção inválida: digite um número de 0 a 5.` |
| `abc` | idem |
| `3abc` | idem |
| (linha vazia) | idem |

**Resultado obtido:** ☐ conforme esperado — _(anexar saída/print)_

### 6.7 Placa inválida

| Entrada na placa | Esperado |
|---|---|
| `AB12345` | `Placa inválida: use o formato ABC1234 ou ABC1D23. Veículo não registrado.` |
| `ABCD123` | idem |
| `ABC123` | idem (6 caracteres) |

Depois de uma placa inválida, registre uma placa válida: ela recebe o **próximo** número de ordem, porque o registro inválido não consome numeração.

**Resultado obtido:** ☐ conforme esperado — _(anexar saída/print)_

### 6.8 Tipo de veículo inválido

Opção 1, placa `AAA1111`, tipo `9` (depois repetir com `0` e `x`).

**Esperado:** `Tipo inválido: digite um número de 1 a 4. Veículo não registrado.` A quantidade na fila (opção 5) não muda.

**Resultado obtido:** ☐ conforme esperado — _(anexar saída/print)_

### 6.9 Quantidade inválida ao abrir o sinal

Com pelo menos 1 veículo na fila, opção 3, com as quantidades `0`, `-1`, `x` e `99999999999`.

**Esperado:** `Quantidade inválida: digite um número inteiro maior que zero.` em todos os casos. Nenhum veículo é liberado.

**Resultado obtido:** ☐ conforme esperado — _(anexar saída/print)_

### 6.10 Liberação de memória

Compile com `make debug`, registre alguns veículos, libere parte deles e saia pela opção 0 **com veículos ainda na fila**. Repita encerrando com Ctrl+D (Linux/WSL) em vez da opção 0.

**Esperado:** o programa termina sem nenhum relatório do AddressSanitizer (`ERROR: LeakSanitizer` ou `ERROR: AddressSanitizer`). Isso comprova que todos os nós foram liberados, inclusive os que ainda estavam na fila no encerramento.

**Resultado obtido:** ☐ conforme esperado — _(anexar saída/print)_

---

## 7. Análise de complexidade

**Notação:**
- **n**: quantidade de veículos na fila no momento da operação.
- **k**: quantidade de veículos que o usuário pede para liberar ao abrir o sinal.
- **m**: quantidade de caracteres digitados para a placa.

### 7.1 `enqueue` — registrar chegada (`include/utils/queue.h:96`)

```cpp
void Queue<T>::enqueue(const T& obj){
    Node* node = new Node(obj);          // O(1): 1 alocação + cópia de um Veiculo (tamanho fixo)

    if(this->tail_ == nullptr){          // condição: O(1)
        this->head_ = node;              //   ramo "fila vazia": O(1)
    } else {
        this->tail_->next = node;        //   ramo "fila não vazia": O(1)
    }

    this->tail_ = node;                  // O(1)
    this->length_++;                     // O(1)
}
```

- **Operação fundamental:** alocação do nó e encadeamento de ponteiros.
- **Estruturas de controle:** um `if/else` com os dois ramos O(1) e nenhuma repetição.
- **Melhor, médio e pior caso:** O(1). O custo não depende de n, porque o ponteiro `tail_` dá acesso direto ao fim. Sem ele, seria preciso percorrer a lista inteira até o último nó, e a inserção passaria a ser O(n).

No fluxo completo de registro (`Semaforo::registrarChegada`), a placa também é validada:

| Trecho | Estrutura | Complexidade |
|---|---|---|
| `Placa::normalize` (`src/vo/placa.cpp:15`) | `for` sobre os caracteres digitados, com `if` O(1) dentro | O(m) |
| `Placa::isValid` (`src/vo/placa.cpp:27`) | `if` de tamanho + `for` de 3 iterações fixas + 4 testes | O(1) |
| `Semaforo::registrarChegada` (`src/services/semaforo.cpp:3`) | sequência simples + `enqueue` | O(1) |

**Registrar chegada: O(m) em relação à entrada e O(1) em relação ao tamanho da fila.** Como uma placa válida tem 7 caracteres, na prática o custo é constante.

### 7.2 `dequeue` — liberar um veículo (`include/utils/queue.h:111`)

```cpp
T Queue<T>::dequeue(){
    if(this->head_ == nullptr){                  // condição: O(1)
        throw std::out_of_range(...);            //   fila vazia: O(1)
    }

    Node* node = this->head_;                    // O(1)
    T value = node->value;                       // O(1): cópia de tamanho fixo
    this->head_ = node->next;                    // O(1)
    if(this->head_ == nullptr){                  // condição: O(1)
        this->tail_ = nullptr;                   //   era o último: O(1)
    }
    delete node;                                 // O(1)
    this->length_--;                             // O(1)
    return value;
}
```

- **Operação fundamental:** desencadeamento e liberação do nó do início.
- **Estruturas de controle:** dois `if` O(1) e nenhuma repetição.
- **Melhor, médio e pior caso:** O(1). O elemento removido é sempre o do início, que `head_` acessa diretamente.

### 7.3 `front` / consultar primeiro (`include/utils/queue.h:132`)

Um `if` (fila vazia?) e o retorno de `head_->value` por referência, sem cópia.
**Melhor, médio e pior caso: O(1).**

### 7.4 `size` / quantidade e `isEmpty` (`include/utils/queue.h:141-144`)

Leitura direta de `length_` e comparação `head_ == nullptr`.
**Melhor, médio e pior caso: O(1).** Sem o contador `length_`, contar os veículos exigiria percorrer a fila, com custo O(n).

### 7.5 Abrir o sinal (`include/services/semaforo.h:39`)

```cpp
int Semaforo::abrirSinal(int quantidade, Callback aoLiberar){
    if(quantidade <= 0){                                       // condição: O(1)
        throw std::invalid_argument(...);
    }

    int liberados = 0;
    while(liberados < quantidade && !this->fila_.isEmpty()){   // executa min(k, n) vezes
        aoLiberar(this->fila_.dequeue());                      //   O(1) por iteração (7.2) + impressão O(1)
        liberados++;
    }

    return liberados;
}
```

- **Operação fundamental:** `dequeue` dentro do `while`.
- **Estruturas de controle:** um `if` O(1) e um `while` que para quando k veículos saíram **ou** quando a fila esvazia, o que acontecer primeiro. O laço roda **min(k, n)** vezes, cada iteração O(1), então o total é **O(min(k, n))**.
- **Melhor caso: O(1).** Acontece com a fila vazia (o `Menu` avisa e nem pergunta a quantidade), com uma quantidade inválida ou com k = 1.
- **Caso médio: O(min(k, n)).** Por exemplo, liberar 2 de 4 veículos (teste 6.3) faz 2 iterações.
- **Pior caso: O(n).** Acontece quando k ≥ n e a fila inteira é liberada (teste 6.4).

**Observação (análise amortizada):** cada veículo entra na fila uma vez e sai no máximo uma vez. Por isso, em toda a execução do programa, a soma das iterações de todas as aberturas de sinal é no máximo o total de veículos registrados. Em média, cada veículo custa O(1) para ser liberado.

### 7.6 Exibir veículos aguardando — `forEach` (`include/utils/queue.h:161`)

```cpp
void Queue<T>::forEach(Visitor visit) const {
    int position = 1;
    for(Node* node = this->head_; node != nullptr; node = node->next){   // n iterações
        visit(node->value, position++);                                  //   impressão de 1 linha: O(1)
    }
}
```

- **Operação fundamental:** visitar (imprimir) um nó.
- **Estruturas de controle:** um `for` que percorre a lista do início até `nullptr`, sempre n iterações, com corpo O(1).
- **Melhor, médio e pior caso: Θ(n).** Para exibir todos os veículos é obrigatório visitar cada um, então não existe atalho. O único caso trivial é a fila vazia (n = 0), que o `Menu` trata em O(1).

### 7.7 Liberar a fila inteira — `clear` / destrutor (`include/utils/queue.h:148`)

```cpp
void Queue<T>::clear(){
    while(this->head_ != nullptr){       // n iterações
        Node* node = this->head_;        //   O(1)
        this->head_ = node->next;        //   O(1)
        delete node;                     //   O(1)
    }
    this->tail_ = nullptr;               // O(1)
    this->length_ = 0;                   // O(1)
}
```

É executado automaticamente no encerramento, pelo destrutor da fila.
**Melhor, médio e pior caso: Θ(n)**, porque cada nó precisa de um `delete`.

### 7.8 Cópia da fila — construtor de cópia e `operator=` (`include/utils/queue.h:58` e `:78`)

O construtor de cópia percorre a fila de origem com um `for` (n iterações) e faz um `enqueue` O(1) para cada nó: **Θ(n)**. O `operator=` faz essa cópia e depois libera a fila antiga com o destrutor da cópia temporária: **Θ(n + n_antiga)**. O programa do semáforo não copia filas; essas operações existem para a classe ficar correta (regra dos três).

### 7.9 Resumo

| Operação | Melhor caso | Caso médio | Pior caso | Justificativa |
|---|---|---|---|---|
| Registrar chegada (`enqueue`) | O(1) | O(1) | O(1) | Acesso direto ao fim pelo ponteiro `tail_`; só `if/else` O(1), sem laço |
| Validar placa | O(m) | O(m) | O(m) | Laço sobre os m caracteres digitados; para placa válida m = 7, na prática O(1) |
| Consultar primeiro (`front`) | O(1) | O(1) | O(1) | Acesso direto ao início pelo ponteiro `head_` |
| Liberar 1 veículo (`dequeue`) | O(1) | O(1) | O(1) | Remove sempre o nó do início; só `if`s O(1) |
| Abrir sinal (liberar k) | O(1) | O(min(k, n)) | O(n) | O `while` roda min(k, n) vezes com corpo O(1); o pior caso é k ≥ n |
| Exibir aguardando (`forEach`) | Θ(n) | Θ(n) | Θ(n) | O `for` precisa visitar todos os n nós |
| Quantidade (`size`) | O(1) | O(1) | O(1) | Contador `length_` mantido a cada inserção/remoção |
| Fila vazia? (`isEmpty`) | O(1) | O(1) | O(1) | Compara `head_` com `nullptr` |
| Encerrar (`clear`/destrutor) | Θ(n) | Θ(n) | Θ(n) | O `while` faz um `delete` por nó |
| Copiar fila | Θ(n) | Θ(n) | Θ(n) | O `for` faz um `enqueue` O(1) por nó |

### 7.10 Complexidade final

- **Operações essenciais da fila** (inserir, remover, consultar o início, tamanho): todas **O(1)**, independentemente de quantos veículos estejam aguardando. É o que se espera de uma fila bem implementada.
- **Operações do menu:** a mais cara é **O(n)**, seja exibir todos os veículos ou abrir o sinal para a fila inteira. Nenhuma operação do programa passa de **O(n)**.
- **Execução completa:** com R registros e L listagens, o custo total é O(R + L·n_max). As liberações somam no máximo O(R) no programa inteiro (análise amortizada, §7.5).
- **Memória: Θ(n).** Há um nó por veículo aguardando (os dados do veículo mais um ponteiro), além de uma quantidade constante de variáveis de controle (`head_`, `tail_`, `length_`, `proximaOrdem_`). Veículos liberados deixam de ocupar memória no mesmo momento.

---

## 8. Gerenciamento de memória

A única alocação dinâmica feita manualmente no programa é a dos nós da fila:

| Onde ocorre o `new` | Onde ocorre o `delete` correspondente |
|---|---|
| `Queue::enqueue`: um nó por veículo que chega | `Queue::dequeue`: quando o veículo é liberado pelo sinal |
| | `Queue::clear`, chamado pelo destrutor `~Queue`: nós que ainda estavam na fila quando o programa termina |

Decisões que garantem que não há vazamentos nem liberações duplicadas:

- **`Veiculo` e `Placa` não usam ponteiros.** Os campos são valores (`std::string`, `enum`, `int`), então não há nada para liberar manualmente e as cópias são independentes (regra do zero).
- **Regra dos três na `Queue`.** A classe gerencia memória, então define destrutor, construtor de cópia e `operator=`. Os dois últimos fazem **cópia profunda** (nós novos), o que evita que duas filas compartilhem nós e os liberem duas vezes.
- **`operator=` com copy-and-swap.** A fila antiga só é liberada depois que a cópia termina com sucesso, e a autoatribuição (`a = a`) é tratada.
- **O destrutor roda em qualquer encerramento.** O objeto `semaforo` do `main` é automático, então seu destrutor (e o da fila) roda tanto ao sair pela opção 0 quanto no fim da entrada.

A verificação prática está descrita no teste 6.10 (`make debug`, com AddressSanitizer/LeakSanitizer).
