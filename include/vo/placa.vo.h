#ifndef PLACA_H
#define PLACA_H

class Placa{
    private: 
        const int MAXLENGTH = 7;
        char *code = new char[MAXLENGTH];
    
    public:
        void setCode(const char *code);

        char *getCode();

};

#endif