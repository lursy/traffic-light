#ifndef PLACA_H
#define PLACA_H

#include <string>

// Value object: placa de veículo no padrão brasileiro.
//   - antigo:   ABC1234  (também aceito como ABC-1234)
//   - Mercosul: ABC1D23
// A entrada é normalizada (sem hífen/espaços, em maiúsculas) e validada no
// construtor, então todo objeto Placa existente é sempre uma placa válida.
class Placa {
    private:
        std::string code_;

        static std::string normalize(const std::string& code);
        static bool isValid(const std::string& code);

    public:
        static constexpr int LENGTH = 7;

        // Lança std::invalid_argument se a placa não estiver em um dos formatos.
        explicit Placa(const std::string& code);

        const std::string& getCode() const;
};

#endif
