#ifndef QUEUE_H
#define QUEUE_H

#include <stdexcept>
#include <utility>

// Fila dinâmica simples (FIFO) implementada com lista encadeada.
//
//   head_ -> [A] -> [B] -> [C] -> nullptr
//                           ^
//                         tail_
//
// Cada elemento fica em um nó alocado com new na entrada (enqueue) e liberado
// com delete na saída (dequeue) ou quando a fila é destruída. A fila guarda
// CÓPIAS dos objetos recebidos, portanto é dona dos próprios dados: apagar o
// objeto original depois do enqueue não afeta o que está na fila.
//
// Por ser um template, a implementação precisa ficar no header.
template <typename T>
class Queue {
    private:
        struct Node {
            T value;
            Node* next;

            explicit Node(const T& value) : value(value), next(nullptr) {}
        };

        Node* head_ = nullptr;   // início: próximo elemento a sair
        Node* tail_ = nullptr;   // fim: onde entram os novos elementos
        int length_ = 0;

        void swap(Queue<T>& other);

    public:
        Queue() = default;
        Queue(const Queue<T>& other);
        ~Queue();

        Queue<T>& operator=(const Queue<T>& other);

        void enqueue(const T& obj);
        T dequeue();
        const T& front() const;

        bool isEmpty() const;
        int size() const;
        void clear();

        // Percorre a fila do início ao fim chamando visit(elemento, posicao),
        // com posicao começando em 1. Não altera a fila.
        template <typename Visitor>
        void forEach(Visitor visit) const;
};

// Cópia profunda: cada nó de `other` é copiado para um nó novo, na mesma ordem.
template <typename T>
Queue<T>::Queue(const Queue<T>& other){
    try {
        for(Node* node = other.head_; node != nullptr; node = node->next){
            this->enqueue(node->value);
        }
    } catch(...) {
        // o destrutor não roda se o construtor falha: libera o que já foi copiado
        this->clear();
        throw;
    }
}

template <typename T>
Queue<T>::~Queue(){
    this->clear();
}

// copy-and-swap: a cópia temporária fica com os dados antigos e os libera
// ao sair de escopo. Se a cópia falhar, esta fila continua intacta.
template <typename T>
Queue<T>& Queue<T>::operator=(const Queue<T>& other){
    if(this != &other){
        Queue<T> copy(other);
        this->swap(copy);
    }

    return *this;
}

template <typename T>
void Queue<T>::swap(Queue<T>& other){
    std::swap(this->head_, other.head_);
    std::swap(this->tail_, other.tail_);
    std::swap(this->length_, other.length_);
}

// Insere no fim da fila. O(1): o ponteiro tail_ evita percorrer a lista.
template <typename T>
void Queue<T>::enqueue(const T& obj){
    Node* node = new Node(obj);

    if(this->tail_ == nullptr){
        this->head_ = node;         // fila vazia: o novo nó é também o primeiro
    } else {
        this->tail_->next = node;
    }

    this->tail_ = node;
    this->length_++;
}

// Remove e devolve o elemento do início da fila. O(1).
template <typename T>
T Queue<T>::dequeue(){
    if(this->head_ == nullptr){
        throw std::out_of_range("There are no elements in this queue.");
    }

    Node* node = this->head_;
    T value = node->value;

    this->head_ = node->next;
    if(this->head_ == nullptr){
        this->tail_ = nullptr;      // era o último: a fila ficou vazia
    }

    delete node;
    this->length_--;

    return value;
}

// Consulta o elemento do início sem removê-lo. O(1).
template <typename T>
const T& Queue<T>::front() const {
    if(this->head_ == nullptr){
        throw std::out_of_range("There are no elements in this queue.");
    }

    return this->head_->value;
}

template <typename T>
bool Queue<T>::isEmpty() const { return this->head_ == nullptr; }

template <typename T>
int Queue<T>::size() const { return this->length_; }

// Libera todos os nós. O(n).
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

template <typename T>
template <typename Visitor>
void Queue<T>::forEach(Visitor visit) const {
    int position = 1;

    for(Node* node = this->head_; node != nullptr; node = node->next){
        visit(node->value, position++);
    }
}

#endif
