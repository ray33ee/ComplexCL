#ifndef TOKEN_H
#define TOKEN_H

#include <complex>

/**
 * @brief The Token class represents a single token in a mathematical formula
 */
class Token
{
public:

    enum TokenType
    {
        INVALID = 0,
        VARIABLE = 1,
        OPERATOR = 2,
        CONSTANT = 3
    } _type;

    union TokenData
    {
        std::complex<double>    _value;
        int                     _operator;

        TokenData(std::complex<double> val): _value(val) { }
        TokenData(int val): _operator(val) { }
    } _data;

    Token() : _type(INVALID), _data(0) {}

    Token(TokenType type, std::complex<double> data) : _type(type), _data(data) {  }

    Token(TokenType type, int op) : _type(type), _data(op) {}
};

#endif // TOKEN_H
