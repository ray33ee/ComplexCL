#ifndef TOKEN_H
#define TOKEN_H

#include <complex>

/**
 * @brief The Token class represents a single token in a mathematical formula
 */
class Token
{
public:

    enum OperatorType
    {
        ADD = 0,
        SUBTRACT = 1,
        MULTIPLY = 2,
        DIVIDE = 3,
        POWER = 4,
        LOG = 5,
        NEGATE = 6,
        CONJUGATE = 7,
        SQRT = 8,
        LN = 9,
        EXP = 10,
        SINH = 11,
        COSH = 12,
        TANH = 13,
        SIN = 14,
        COS = 15,
        TAN = 16,
        ASINH = 17,
        ACOSH = 18,
        ATANH = 19,
        ASIN = 20,
        ACOS = 21,
        ATAN = 22,
        ABS = 23,
        ARG = 24
    };

    union TokenData
    {
        std::complex<double>    _value;
        OperatorType            _operator;

        TokenData(std::complex<double> val): _value(val) { }
        TokenData(OperatorType val): _operator(val) { }
    } _data;

    enum TokenType
    {
        INVALID = 0,
        VARIABLE = 1,
        OPERATOR = 2,
        CONSTANT = 3
    }  _type;

    Token() :  _data(0), _type(INVALID) {}

    Token(TokenType type, std::complex<double> data = 0.0) : _data(data), _type(type) {  }
    Token(OperatorType op) : _data(op), _type(OPERATOR) {  }
};

#endif // TOKEN_H
