# Relatório — Tema 2: Simulação de Semáforo com Fila

**UNIFEI — ECOI2204 Algoritmos e Estrutura de Dados**
**Professor:** Dr. André Ribeiro de Brito

| Aluno | Matrícula |
|---|---|
| Matheus Felipe da Cruz Ferreira | 2026015610 |

---

## 1. Descrição da solução

O programa simula um semáforo em uma via única. Os veículos que chegam entram no fim de uma fila e, quando o sinal abre, o usuário informa quantos podem passar. Eles saem sempre pelo início da fila, respeitando a política FIFO.

Implementei uma **fila dinâmica encadeada** do zero, sem estruturas prontas da STL. O código está dividido em classes:

| Classe | O que faz |
|---|---|
| `Queue` | A fila: nós ligados por ponteiros |
| `Placa` | Guarda a placa e valida o formato (`ABC1234` ou `ABC1D23`) |
| `Veiculo` | Placa, tipo (moto, carro, caminhão, ônibus) e ordem de chegada |
| `Semaforo` | Contém a fila e gera a ordem de chegada |
| `Menu` | Menu textual: lê o teclado e mostra os resultados |

O menu oferece: registrar chegada, consultar o primeiro, abrir o sinal, listar os veículos aguardando e mostrar a quantidade. Entradas inválidas e operações sobre a fila vazia geram uma mensagem, e o programa volta ao menu.

## 2. Representação da estrutura

Cada veículo fica em um **nó** com os dados e um ponteiro para o próximo. A fila guarda um ponteiro para o primeiro nó (`head_`), um para o último (`tail_`) e a quantidade de nós (`length_`).

```cpp
struct Node {
    Veiculo value;
    Node* next;
};
```

```
 head_                               tail_
   ↓                                   ↓
 [#1 ABC1234] → [#2 BRA2E19] → [#3 XYZ9876] → nullptr
```

- **Chegada (`enqueue`):** cria um nó com `new`, liga o último nó a ele e atualiza o `tail_`. Se a fila estava vazia, o nó também vira o `head_`.
- **Saída (`dequeue`):** avança o `head_` para o segundo nó e apaga o primeiro com `delete`. Se a fila ficou vazia, o `tail_` volta a ser `nullptr`.
- **Fim do programa:** o destrutor da fila apaga com `delete` os nós que ainda restarem.

## 3. Testes executados

Testes feitos pelo menu, nesta sequência, começando com a fila vazia.

| # | Teste | Entrada | Resultado obtido | Status |
|---|---|---|---|---|
| 1 | Abrir o sinal com a fila vazia | Opção 3 | `Sinal aberto, mas não há veículos para liberar.` | ✅ |
| 2 | Registrar vários veículos e conferir a ordem | `ABC1234` (carro), `BRA2E19` (moto), `XYZ9876` (caminhão), `DEF5678` (ônibus); depois opção 4 | Ordens #1 a #4 geradas, e a listagem mostra os veículos na ordem de chegada | ✅ |
| 3 | Consultar o primeiro sem alterar a fila | Opção 2 duas vezes, depois opção 5 | As duas vezes: `#1 ABC1234 (Carro)`, e depois `Veículos na fila: 4` | ✅ |
| 4 | Liberar menos veículos do que existem | Opção 3, quantidade `2` | Liberados #1 e #2, `2 veículo(s) aguardando` | ✅ |
| 5 | Liberar mais veículos do que existem | Opção 3, quantidade `5` | Liberados #3 e #4, com o aviso `Foram solicitados 5, mas só havia 2 veículo(s) na fila. Não há mais veículos para liberar.` | ✅ |
| 6 | Entradas inválidas | Opção `9`; placa `AB12345`; quantidade `-1` | `Opção inválida`, `Placa inválida` e `Quantidade inválida`, sempre voltando ao menu | ✅ |

## 4. Análise de complexidade

**n** = número de veículos na fila; **k** = quantidade de veículos pedida ao abrir o sinal.

### Inserir (`enqueue`)

- **Operação fundamental:** criação do nó e ligação dos ponteiros.
- **Casos:** o `tail_` dá acesso direto ao fim, então o custo é o mesmo para qualquer tamanho de fila. Melhor, médio e pior caso: **O(1)**.

```cpp
Node* node = new Node(obj);        // O(1)
if(this->tail_ == nullptr){        // condição: O(1)
    this->head_ = node;
} else {
    this->tail_->next = node;
}
this->tail_ = node;                // O(1)
this->length_++;                   // O(1)
```

### Remover (`dequeue`) e consultar o primeiro (`front`)

- **Operação fundamental:** acesso ao nó do início (`head_`).
- **Casos:** sempre atuam sobre o primeiro nó, e só há condições simples, sem repetição. Melhor, médio e pior caso: **O(1)**.

```cpp
if(this->head_ == nullptr){ ... }   // condição: O(1)
Node* node = this->head_;
this->head_ = node->next;
if(this->head_ == nullptr){         // condição: O(1)
    this->tail_ = nullptr;
}
delete node;                        // O(1)
```

### Quantidade (`size`)

- **Operação fundamental:** leitura do contador `length_`, atualizado a cada inserção e remoção.
- **Casos:** melhor, médio e pior caso: **O(1)**.

### Abrir o sinal (`abrirSinal`)

- **Operação fundamental:** `dequeue` dentro do laço.
- **Casos:** o laço para quando saem k veículos ou quando a fila esvazia.
  - **Melhor caso:** k = 1, **O(1)**.
  - **Caso médio:** k < n, **O(k)**.
  - **Pior caso:** k ≥ n, a fila inteira é liberada, **O(n)**.

```cpp
if(quantidade <= 0){ ... }                                 // condição: O(1)
while(liberados < quantidade && !this->fila_.isEmpty()){   // repetição: min(k, n) vezes
    aoLiberar(this->fila_.dequeue());                      // O(1)
    liberados++;
}
```

### Listar a fila (`forEach`)

- **Operação fundamental:** visita de um nó.
- **Casos:** é preciso passar por todos os nós. Melhor, médio e pior caso: **O(n)**.

```cpp
for(Node* node = this->head_; node != nullptr; node = node->next){   // repetição: n vezes
    visit(node->value, position++);                                  // O(1)
}
```

### Esvaziar a fila (`clear`, usado pelo destrutor)

- **Operação fundamental:** `delete` de um nó.
- **Casos:** apaga todos os nós. Melhor, médio e pior caso: **O(n)**.

```cpp
while(this->head_ != nullptr){      // repetição: n vezes
    Node* node = this->head_;
    this->head_ = node->next;
    delete node;                    // O(1)
}
```

### Resumo

| Operação | Melhor caso | Caso médio | Pior caso | Justificativa |
|---|---|---|---|---|
| Inserir (`enqueue`) | O(1) | O(1) | O(1) | Acesso direto ao fim pelo `tail_`, sem laço |
| Remover (`dequeue`) | O(1) | O(1) | O(1) | Remove sempre o primeiro nó, sem laço |
| Consultar primeiro (`front`) | O(1) | O(1) | O(1) | Acesso direto ao `head_` |
| Quantidade (`size`) | O(1) | O(1) | O(1) | Contador mantido a cada inserção/remoção |
| Abrir sinal | O(1) | O(k) | O(n) | Laço executa min(k, n) vezes |
| Listar a fila (`forEach`) | O(n) | O(n) | O(n) | Laço percorre todos os nós |
| Esvaziar a fila (`clear`) | O(n) | O(n) | O(n) | Um `delete` por nó |

### Complexidade final

As operações básicas da fila (inserir, remover, consultar e quantidade) são **O(1)**. As operações mais custosas do programa são listar a fila e liberar todos os veículos, ambas **O(n)**. Portanto, a complexidade final do programa por operação é **O(n)**. A memória utilizada também é **O(n)**, com um nó por veículo aguardando.
