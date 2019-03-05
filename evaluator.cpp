#include "evaluator.h"

Evaluator::Evaluator()
{
    _formula = "z";
    _tokens.append(Token(Token::VARIABLE));
    _tokens.append(Token(Token::CONSTANT, 2.0));
    _tokens.append(Token(Token::POWER));
}

Evaluator::Evaluator(QString formula)
{

}

Token* Evaluator::getTokens()
{
    return _tokens.data();
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

    for (const Token& token : _tokens)
    {
        if (token._type == Token::OPERATOR && token._data._operator < 6) //If its a unary operator, decrement
            size--;
        else if (token._type == Token::VARIABLE || token._type == Token::CONSTANT)
            size++;
        maxStack = size > maxStack ? size : maxStack;
    }
    return maxStack;
}

std::complex<double> Evaluator::operator()(const std::complex<double> &z)
{
    return std::complex<double>(0);
}
