#include <stdexcept>

#include "utils/queue.h"

template <typename T>
Queue<T>::Queue(int size){
    this->queue_ = new T*[size];
    this->size_ = size;
}

template <typename T>
Queue<T>::~Queue(){
    for(int i = 0; i != this->size_; i++){
        delete this->queue_[i];
    }

    delete[] this->queue_;
    this->next_ = 0;
    this->size_ = 0;
    this->length_ = 0;
}

template <typename T>
Queue<T>::Queue(const Queue<T> & other){
    this->length_ = other.length_;
    this->next_ = other.next_;
    this->size_ = other.size_;

    this->queue_ = new T*[this->size_];

    for(int i = 0; i != this->length_; i++){
        this->queue_[i] = other.queue_[this->next_%this->size_];
    }
}

template <typename T>
void Queue<T>::enqueue(T* obj){
    if(this->length_ >= this->size_){
        throw std::invalid_argument("Maximum length exceeded!");
    }

    if(obj == nullptr){
        throw std::invalid_argument("Invalid nullprt Object!");
    }

    *(this->queue_ + (this->next_+this->length_)%this->size_) = obj;

    this->length_++;
}

template <typename T>
T* Queue<T>::dequeue(){
    if(this->length_ == 0){
        throw std::invalid_argument("There are no elements in this queue.");
    }

    T* obj = *(this->queue_ + this->next_);
    
    if((++this->next_) == this->size_) this->next_ = 0;

    this->length_--;

    return obj;
}

template <typename T>
T* Queue<T>::next(){ return *(this->queue_ + this->next_); }

#include "entities/veiculo.h"
template class Queue<Veiculo>;