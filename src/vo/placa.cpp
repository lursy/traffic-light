#include <cctype>
#include <stdexcept>

#include "vo/placa.vo.h"

// O(m)
Placa::Placa(const std::string& code) : code_(normalize(code)) {
    if(!isValid(this->code_)){
        throw std::invalid_argument("Placa inválida: use o formato ABC1234 ou ABC1D23.");
    }
}

// O(m)
std::string Placa::normalize(const std::string& code){
    std::string normalized;

    for(char c : code){
        if(c == '-' || c == ' ') continue;
        normalized += (char) std::toupper((unsigned char) c);
    }

    return normalized;
}

// O(1)
bool Placa::isValid(const std::string& code){
    if((int) code.size() != LENGTH) return false;

    for(int i = 0; i < 3; i++){
        if(!std::isalpha((unsigned char) code[i])) return false;
    }

    return std::isdigit((unsigned char) code[3])
        && std::isalnum((unsigned char) code[4])
        && std::isdigit((unsigned char) code[5])
        && std::isdigit((unsigned char) code[6]);
}

// O(1)
const std::string& Placa::getCode() const { return this->code_; }
