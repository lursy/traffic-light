#include <stdexcept>
#include <utility>

#include "utils/queue.h"

template <typename T>
Queue<T>::Queue(int size){
    this->queue_ = new T*[size];
    this->size_ = size;
}

template <typename T>
Queue<T>::~Queue(){
    for(int i = 0; i != this->length_; i++){
        delete this->queue_[(this->next_ + i) % this->size_];
    }

    delete[] this->queue_;
}

template <typename T>
Queue<T>::Queue(const Queue<T> & other){
    this->length_ = other.length_;
    this->next_ = other.next_;
    this->size_ = other.size_;

    this->queue_ = new T*[this->size_];

    for(int i = 0; i != this->length_; i++){
        int index = (this->next_ + i) % this->size_;
        this->queue_[index] = new T(*other.queue_[index]);
    }
}

template <typename T>
Queue<T>& Queue<T>::operator=(const Queue<T> & other){
    if(this == &other) return *this;

    // copy-and-swap: a copia temporaria libera os dados antigos ao sair de escopo
    Queue<T> copy(other);

    std::swap(this->queue_, copy.queue_);
    std::swap(this->size_, copy.size_);
    std::swap(this->length_, copy.length_);
    std::swap(this->next_, copy.next_);

    return *this;
}

template <typename T>
void Queue<T>::enqueue(const T& obj){
    if(this->length_ >= this->size_){
        throw std::invalid_argument("Maximum length exceeded!");
    }

    *(this->queue_ + (this->next_+this->length_)%this->size_) = new T(obj);

    this->length_++;
}

template <typename T>
T Queue<T>::dequeue(){
    if(this->length_ == 0){
        throw std::invalid_argument("There are no elements in this queue.");
    }

    T* obj = *(this->queue_ + this->next_);
    T value = *obj;
    delete obj;
    
    if((++this->next_) == this->size_) this->next_ = 0;

    this->length_--;

    return value;
}

template <typename T>
T& Queue<T>::next(){
    if(this->length_ == 0){
        throw std::invalid_argument("There are no elements in this queue.");
    }

    return **(this->queue_ + this->next_);
}

#include "entities/veiculo.h"
template class Queue<Veiculo>;