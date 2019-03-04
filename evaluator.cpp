#include "evaluator.h"

Evaluator::Evaluator()
{
    _formula = "z";
    _tokens.append(Token(Token::VARIABLE, 0));
}
