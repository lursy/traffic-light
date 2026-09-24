#ifndef QUEUE_H
#define QUEUE_H

template <typename T>
class Queue {
    private:
        struct Node {
            T value;
            Node* next;

            // O(1)
            explicit Node(const T& value) : value(value), next(nullptr) {}
        };

        Node* head_ = nullptr;
        Node* tail_ = nullptr;
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

        void forEach(void (*visit)(const T&, int)) const;
};

#endif
