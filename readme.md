# queue-traffic — Simulação de Semáforo com Fila

Trabalho de ECOI2204 (Algoritmos e Estrutura de Dados, UNIFEI) — Tema 2.

Simulação em terminal de uma via única controlada por semáforo: os veículos
entram no fim de uma **fila dinâmica encadeada, implementada manualmente**
(sem contêineres da STL) e são liberados em ordem FIFO quando o sinal abre.

## Como executar

```bash
make run                      # compila e executa
make debug && ./app           # versão com AddressSanitizer (verifica vazamentos)
docker compose run --rm app   # via Docker
```

## Funcionalidades

| Opção | O que faz |
|---|---|
| 1 | Registra a chegada de um veículo (placa + tipo) no fim da fila; a ordem de chegada é gerada pelo sistema |
| 2 | Mostra o primeiro da fila sem removê-lo |
| 3 | Abre o sinal: pergunta quantos veículos passam e libera na ordem de chegada |
| 4 | Lista todos os veículos aguardando |
| 5 | Mostra a quantidade de veículos na fila |
| 0 | Encerra o programa |

Entradas inválidas (opção inexistente, placa fora do formato, quantidade ≤ 0) e
operações sobre a fila vazia mostram uma mensagem e voltam ao menu.

## Como a fila funciona

Cada veículo fica em um nó alocado com `new`, que aponta para o próximo. A fila
guarda o primeiro nó (`head_`), o último (`tail_`) e a quantidade (`length_`).

```
 head_                               tail_
   ↓                                   ↓
 [#1 ABC1234] → [#2 BRA2E19] → [#3 XYZ9876] → nullptr
```

- **Entrada (`enqueue`)**: o novo nó é ligado depois do `tail_` e vira o novo `tail_`.
- **Saída (`dequeue`)**: o `head_` avança para o segundo nó e o primeiro é apagado com `delete`.
- **Fim do programa**: o destrutor apaga os nós que sobraram.

## Principais decisões

- **Lista encadeada com `head_` e `tail_`.** O enunciado pede fila dinâmica, sem
  capacidade fixa. Em um array o fim pode ser calculado por `início + tamanho`,
  mas numa lista os nós ficam espalhados na memória; sem o `tail_`, achar o fim
  exigiria percorrer a fila inteira (O(n) por chegada). Com ele, é O(1).
- **Contador `length_`.** Atualizado em cada entrada e saída, então a quantidade
  é O(1) em vez de contar os nós.
- **A fila guarda cópias.** `enqueue` copia o veículo para dentro do nó. Assim,
  apagar o objeto original não deixa um ponteiro inválido dentro da fila.
- **Regra dos três.** Como a `Queue` gerencia memória, ela define destrutor,
  construtor de cópia e `operator=` (cópia profunda), evitando vazamentos e
  `delete` duplicado.
- **Padrão `include/` + `src/`.** A `Queue` é um template, mas a implementação
  fica em `src/utils/queue.cpp` com instanciação explícita para `Veiculo`,
  no mesmo padrão das outras classes.
- **Validação na `Placa`.** O construtor normaliza (`abc-1234` → `ABC1234`) e
  rejeita formatos inválidos, então toda placa que existe no programa é válida.
- **Camadas separadas.** `Menu` só faz entrada/saída, `Semaforo` tem as regras
  (ordem de chegada, abrir sinal) e `Queue` é só a estrutura de dados.

## Estrutura

```
include/utils/queue.h          fila dinâmica Queue<T> (lista encadeada)
src/utils/queue.cpp            implementação da fila (instanciada para Veiculo)
include/vo/placa.vo.h          placa validada (ABC1234 / ABC1D23)
include/entities/veiculo.h     veículo: placa, tipo, ordem de chegada
include/services/semaforo.h    regras do semáforo (única fila do programa)
include/ui/menu.h              menu textual
```

## Complexidade

Notação usada nos comentários `// O(...)` acima de cada função:

- **n**: quantidade de veículos na fila;
- **k**: quantidade de veículos pedida ao abrir o sinal;
- **m**: quantidade de caracteres digitados na entrada (linha do teclado ou placa).

### Resumo

| Função | Complexidade |
|---|---|
| `Queue::enqueue`, `Queue::dequeue`, `Queue::front` | O(1) |
| `Queue::size`, `Queue::isEmpty`, `Queue::swap`, construtor de `Node` | O(1) |
| `Queue::forEach`, `Queue::clear`, `~Queue` | O(n) |
| Construtor de cópia de `Queue`, `Queue::operator=` | O(n) |
| `Semaforo::registrarChegada`, `primeiro`, `quantidade`, `vazio` | O(1) |
| `Semaforo::abrirSinal` | O(min(k, n)) |
| `Semaforo::paraCadaAguardando` | O(n) |
| Construtor de `Placa`, `Placa::normalize` | O(m) |
| `Placa::isValid`, `Placa::getCode` | O(1) |
| `Veiculo` (construtor, getters) e `tipoVeiculoToString` | O(1) |
| `Menu::registrarChegada`, `lerLinha`, `paraInteiro` | O(m) |
| `Menu::abrirSinal` | O(m + min(k, n)) |
| `Menu::exibirAguardando` | O(n) |
| `Menu::consultarPrimeiro`, `exibirQuantidade`, `imprimirVeiculo`, construtor de `Menu` | O(1) |
| `Menu::executar`, `main` | O(m + n) por opção escolhida no menu |

As operações essenciais da fila (inserir, remover, consultar o início e o tamanho) são **O(1)**. A operação mais cara do programa é **O(n)**: exibir a fila ou liberá-la inteira. A memória usada é **Θ(n)**, com um nó por veículo aguardando.

### Fila (`src/utils/queue.cpp`)

**`enqueue`: O(1).** O ponteiro `tail_` dá acesso direto ao fim, sem percorrer a lista. O `if/else` só decide se o novo nó também é o primeiro (fila vazia).

```cpp
// O(1)
template <typename T>
void Queue<T>::enqueue(const T& obj){
    Node* node = new Node(obj);

    if(this->tail_ == nullptr){
        this->head_ = node;
    } else {
        this->tail_->next = node;
    }

    this->tail_ = node;
    this->length_++;
}
```

**`dequeue`: O(1).** Sempre remove o nó apontado por `head_`. Os dois `if` são O(1): um trata a fila vazia e o outro zera `tail_` quando o último nó sai.

```cpp
// O(1)
template <typename T>
T Queue<T>::dequeue(){
    if(this->head_ == nullptr){
        throw std::out_of_range("There are no elements in this queue.");
    }

    Node* node = this->head_;
    T value = node->value;

    this->head_ = node->next;
    if(this->head_ == nullptr){
        this->tail_ = nullptr;
    }

    delete node;
    this->length_--;

    return value;
}
```

**`front`: O(1).** Devolve o valor do nó `head_` por referência, sem remover.

```cpp
// O(1)
template <typename T>
const T& Queue<T>::front() const {
    if(this->head_ == nullptr){
        throw std::out_of_range("There are no elements in this queue.");
    }

    return this->head_->value;
}
```

**`isEmpty` e `size`: O(1).** O contador `length_` é atualizado em cada `enqueue`/`dequeue`. Sem ele, contar exigiria percorrer a fila em O(n).

```cpp
// O(1)
template <typename T>
bool Queue<T>::isEmpty() const { return this->head_ == nullptr; }
```

```cpp
// O(1)
template <typename T>
int Queue<T>::size() const { return this->length_; }
```

**`forEach`: O(n).** O `for` visita os n nós, do início ao fim, uma vez cada.

```cpp
// O(n)
template <typename T>
void Queue<T>::forEach(void (*visit)(const T&, int)) const {
    int position = 1;

    for(Node* node = this->head_; node != nullptr; node = node->next){
        visit(node->value, position++);
    }
}
```

**`clear` e destrutor: O(n).** O `while` executa um `delete` por nó.

```cpp
// O(n)
template <typename T>
void Queue<T>::clear(){
    while(this->head_ != nullptr){
        Node* node = this->head_;
        this->head_ = node->next;
        delete node;
    }

    this->tail_ = nullptr;
    this->length_ = 0;
}
```

```cpp
// O(n)
template <typename T>
Queue<T>::~Queue(){
    this->clear();
}
```

**Construtor de cópia e `operator=`: O(n).** A cópia faz um `enqueue` O(1) para cada um dos n nós da origem. O `operator=` copia e troca (copy-and-swap), e a fila antiga é liberada pelo destrutor da cópia temporária. Nesse caso, n é a soma dos nós das duas filas.

```cpp
// O(n)
template <typename T>
Queue<T>::Queue(const Queue<T>& other){
    try {
        for(Node* node = other.head_; node != nullptr; node = node->next){
            this->enqueue(node->value);
        }
    } catch(...) {
        this->clear();
        throw;
    }
}
```

```cpp
// O(n)
template <typename T>
Queue<T>& Queue<T>::operator=(const Queue<T>& other){
    if(this != &other){
        Queue<T> copy(other);
        this->swap(copy);
    }

    return *this;
}
```

### Semáforo (`src/services/semaforo.cpp`)

**`registrarChegada`: O(1).** Cria o veículo (tamanho fixo) e faz um `enqueue` O(1).

```cpp
// O(1)
Veiculo Semaforo::registrarChegada(const Placa& placa, TipoVeiculo tipo){
    Veiculo veiculo(placa, tipo, this->proximaOrdem_);

    this->fila_.enqueue(veiculo);
    this->proximaOrdem_++;

    return veiculo;
}
```

**`abrirSinal`: O(min(k, n)).** O `while` para quando k veículos saíram **ou** quando a fila esvazia, e cada iteração é um `dequeue` O(1).
- **Melhor caso O(1):** k = 1, ou quantidade inválida.
- **Pior caso O(n):** k ≥ n, liberando a fila inteira.

Como cada veículo sai no máximo uma vez, a soma de todas as aberturas de sinal da execução é limitada pelo total de veículos registrados.

```cpp
// O(min(k, n))
int Semaforo::abrirSinal(int quantidade, void (*aoLiberar)(const Veiculo&)){
    if(quantidade <= 0){
        throw std::invalid_argument("A quantidade deve ser maior que zero.");
    }

    int liberados = 0;

    while(liberados < quantidade && !this->fila_.isEmpty()){
        aoLiberar(this->fila_.dequeue());
        liberados++;
    }

    return liberados;
}
```

### Placa (`src/vo/placa.cpp`)

**`normalize`: O(m).** O `for` percorre os m caracteres digitados, e o `if` dentro dele é O(1).

```cpp
// O(m)
std::string Placa::normalize(const std::string& code){
    std::string normalized;

    for(char c : code){
        if(c == '-' || c == ' ') continue;
        normalized += (char) std::toupper((unsigned char) c);
    }

    return normalized;
}
```

**`isValid`: O(1).** O `for` tem 3 iterações fixas e o restante são 4 testes, independente da entrada, porque o tamanho já foi verificado como 7.

```cpp
// O(1)
bool Placa::isValid(const std::string& code){
    if((int) code.size() != LENGTH) return false;

    for(int i = 0; i < 3; i++){
        if(!std::isalpha((unsigned char) code[i])) return false;
    }

    return std::isdigit((unsigned char) code[3])
        && std::isalnum((unsigned char) code[4])
        && std::isdigit((unsigned char) code[5])
        && std::isdigit((unsigned char) code[6]);
}
```

### Menu (`src/ui/menu.cpp`)

**`exibirAguardando`: O(n).** O custo vem do `forEach` da fila. O `if` inicial trata a fila vazia em O(1).

```cpp
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
```

Relatório técnico (descrição da solução, representação da estrutura, roteiro de
testes e análise de complexidade): [docs/relatorio.md](docs/relatorio.md).
