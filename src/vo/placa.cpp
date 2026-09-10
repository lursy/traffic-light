#include <stdexcept>
#include <stdio.h>

#include "vo/placa.vo.h"

void Placa::setCode(const char *code){
    int length = 0;

    for(length; code[length] != '\0'; length++){
        if(length > this->MAXLENGTH){
            throw std::invalid_argument("Maximum length exceeded!");
        }
    }

    if(length != this->MAXLENGTH) {
        throw std::invalid_argument("Invalid size!");
    }

    this->code = (char*) code;
}

char *Placa::getCode() { return this->code; }