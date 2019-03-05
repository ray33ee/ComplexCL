#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <complex>
#include <QVector>
#include <QString>

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

/**
 * @brief The Evaluator class converts a string to a list of tokens and evaluating the resulting function.
 */
class Evaluator
{
private:
    /**
     * @brief _tokens contains the list of tokens produced by the formula
     */
    QVector<Token> _tokens;

    /**
     * @brief _formula the original string used
     */
    QString _formula;

public:

    /**
     * @brief Evaluator constructs an empty evaluator class
     */
    Evaluator();

    /**
     * @brief Evaluator constructs an evaluator from a formula
     * @param formula
     */
    Evaluator(QString formula);

    /**
     * @brief getTokens gets the pointer to the array of tokens
     * @return the list of tokens
     */
    Token* getTokens();

    /**
     * @brief getCount get the number of tokens used
     * @return number of tokens
     */
    int getCount();

    /**
     * @brief getFormula gets the original string formula
     * @return original string formula
     */
    QString getFormula() const;

    /**
     * @brief getStackMax gets the maximum size a stack would need to be to evaluate this
     * @return integer representing max stack size needed
     */
    int getStackMax() const;

    /**
     * @brief operator () evaluates the function
     * @param z the input complex value
     * @return the output value, f(z).
     */
    std::complex<double> operator()(const std::complex<double> &z);
};

#endif // EVALUATOR_H
