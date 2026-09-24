#ifndef PLACA_H
#define PLACA_H

#include <string>

class Placa {
    private:
        std::string code_;

        static std::string normalize(const std::string& code);
        static bool isValid(const std::string& code);

    public:
        static constexpr int LENGTH = 7;

        explicit Placa(const std::string& code);

        const std::string& getCode() const;
};

#endif
