#include "evaluator.h"

Evaluator::Evaluator()
{
    setString("z");
}

Evaluator::Evaluator(QString formula)
{
    setString(formula);
}

void Evaluator::setString(QString formula)
{
    _formula = formula;
    _tokens.append(Token<double>(VARIABLE));
    _tokens.append(Token<double>(CONSTANT, 6.0));
    _tokens.append(Token<double>(POWER));
}

Token<double>* Evaluator::getTokens()
{
    return _tokens.data();
}

Token<float>* Evaluator::getFloatTokens()
{
    _floatTokens.clear();

    for (Token<double> &token : _tokens)
    {
        Token<float> item;

        item._type = token._type;

        if (item._type == CONSTANT)
            item._data._value = std::complex<float>(token._data._value.real(), token._data._value.imag());
        else
            item._data._operator = token._data._operator;

        _floatTokens.append(item);
    }

    return _floatTokens.data();
}

int Evaluator::getCount()
{
    return _tokens.count();
}

QString Evaluator::getFormula() const
{
    return _formula;
}

int Evaluator::getStackMax() const
{
    int maxStack = 1;
    int size = 0;

    for (const Token<double>& token : _tokens)
    {
        if (token._type == OPERATOR && token._data._operator < 6) //If its a unary operator, decrement
            size--;
        else if (token._type == VARIABLE || token._type == CONSTANT)
            size++;
        maxStack = size > maxStack ? size : maxStack;
    }
    return maxStack;
}

std::complex<double> Evaluator::operator()(const std::complex<double> &z)
{
    return std::complex<double>(0);
}
