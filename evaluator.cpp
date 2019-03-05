#include "evaluator.h"

Evaluator::Evaluator()
{
    _formula = "z";
    _tokens.append(Token(Token::VARIABLE));
    _tokens.append(Token(Token::LN));
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
    return 1;
}

std::complex<double> Evaluator::operator()(const std::complex<double> &z)
{
    return std::complex<double>(0);
}
