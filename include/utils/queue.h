#ifndef QUEUE_H
#define QUEUE_H

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
// A implementação fica em src/utils/queue.cpp, instanciada explicitamente para
// os tipos usados no programa.
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
        void forEach(void (*visit)(const T&, int)) const;
};

#endif
