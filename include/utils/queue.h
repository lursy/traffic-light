#ifndef QUEUE_H
#define QUEUE_H

template <typename T>
class Queue {
    private:
        int size_;
        int length_ = 0;
        int next_ = 0;

        T** queue_;
    
    public:
        Queue(const Queue<T>& other);
        Queue(int size);
        ~Queue();

        void enqueue(T* obj);
        T* dequeue();
        T* next();
};

#endif