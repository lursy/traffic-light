#include <stdexcept>
#include <utility>

#include "utils/queue.h"

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

// O(n)
template <typename T>
Queue<T>::~Queue(){
    this->clear();
}

// O(n)
template <typename T>
Queue<T>& Queue<T>::operator=(const Queue<T>& other){
    if(this != &other){
        Queue<T> copy(other);
        this->swap(copy);
    }

    return *this;
}

// O(1)
template <typename T>
void Queue<T>::swap(Queue<T>& other){
    std::swap(this->head_, other.head_);
    std::swap(this->tail_, other.tail_);
    std::swap(this->length_, other.length_);
}

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

// O(1)
template <typename T>
const T& Queue<T>::front() const {
    if(this->head_ == nullptr){
        throw std::out_of_range("There are no elements in this queue.");
    }

    return this->head_->value;
}

// O(1)
template <typename T>
bool Queue<T>::isEmpty() const { return this->head_ == nullptr; }

// O(1)
template <typename T>
int Queue<T>::size() const { return this->length_; }

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

// O(n)
template <typename T>
void Queue<T>::forEach(void (*visit)(const T&, int)) const {
    int position = 1;

    for(Node* node = this->head_; node != nullptr; node = node->next){
        visit(node->value, position++);
    }
}

#include "entities/veiculo.h"
template class Queue<Veiculo>;
