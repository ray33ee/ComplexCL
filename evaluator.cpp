#include "evaluator.h"

QString Evaluator::floatingRegex = "[-+]?(?:\\b[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+\\b)(?:[eE][-+]?[0-9]+\\b)?";


QString Evaluator::functionsRegex = "log|neg|conj|sqrt|ln|exp|sinh|cosh|tanh|sin|cos|tan|asinh|acosh|atanh|asin|acos|atan|inv|mod|arg";

QString Evaluator::constantsRegex = "e|pi|i";

QString Evaluator::operatorsRegex = "[*/^]|[+-]+";

QString Evaluator::labelRegex = "[a-z]+";

QString Evaluator::equationRegex = "(" + floatingRegex + "|" +  labelRegex + "|" + constantsRegex + "|z|" + operatorsRegex +  "|[()\\[\\]{},]"  + ")";

template <class T>
QStringList Token<T>::allfunctions = { "+", "-", "*", "/", "^",
                                        "log", "neg", "conj", "sqrt", "ln", "exp",
                                        "sinh", "cosh", "tanh", "sin", "cos", "tan", "asinh", "acosh", "atanh", "asin", "acos", "atan",
                                        "inv", "mod", "arg" };

Evaluator::Evaluator()
{
    //setString("(z^2+i)*(z^2-3)/(z+1)*(z-4)");
    setString("z^z");
    //setString("z ^ 2");
}

Evaluator::Evaluator(QString formula)
{
    setString(formula);
}

void Evaluator::setString(QString formula)
{
    _formula = formula;

    _tokens.clear();

    int pos = 0;

    QString prevToken = "";

    QStack<QString> opStack;

    static QRegExp rx(equationRegex);

    static auto isUnaryNegate = [=](QString token) { return token == "-" && ((_tokens.isEmpty() && opStack.isEmpty()) || prevToken == "(" || prevToken == "neg" || isOp(prevToken)); };

    while ((pos = rx.indexIn(formula, pos)) != -1)
    {
        QString token = rx.cap(1);

        qDebug() << "Token: " << token;

        if (token == "z")
        {
            _tokens.append(Token<double>(VARIABLE));
        }
        else if (token == "e")
        {
            _tokens.append(Token<double>(CONSTANT, M_E));
        }
        else if (token == "pi")
        {
            _tokens.append(Token<double>(CONSTANT, M_PI));
        }
        else if (token == "i")
        {
            _tokens.append(Token<double>(CONSTANT, std::complex<double>(0, 1)));
        }
        else if (isNum(token))
        {
            _tokens.append(Token<double>(CONSTANT, token.toDouble()));
        }
        else if (isOp(token))
        {
            if (isUnaryNegate(token))
            {
                opStack.append("neg");
            }
            else
            {
                while (!opStack.isEmpty())
                {
                    if ((isOp(opStack.top()) && getPrecedence(opStack.top()) > getPrecedence(token)) || (isOp(opStack.top()) && getPrecedence(opStack.top()) == getPrecedence(token) && token != "^" && opStack.top() != "("))
                        _tokens.append(Token<double>(getIndex(opStack.pop())));
                    else
                        break;
                }
                opStack.append(token);
            }
        }
        else if (isFunction(token))
        {
            opStack.append(token);
        }
        else if (token == ",")
        {
            while (opStack.size() > 0)
            {
                if (opStack.top() == "(")
                    break;
                _tokens.append(Token<double>(getIndex(opStack.pop()))); //Fix this shit - Using getIndex on token when the token is ','
            }
        }
        else if (token == "(" || token == "{" || token == "[") //Left bracket
        {
            opStack.append("(");
        }
        else if (token == ")"|| token == "}" || token == "]") //Right bracket
        {
            while (!opStack.isEmpty())
            {
                if (opStack.top() != "(")
                    _tokens.append(Token<double>(getIndex(opStack.pop())));
                else
                    break;
            }

            if (opStack.isEmpty())
            {
                throw MissingLeftBracketException();
            }

            opStack.pop(); //Pop the bracket

            if (opStack.isEmpty())
                return;

            if (isFunction(opStack.top()) || isOp(opStack.top()))
                _tokens.append(Token<double>(getIndex(opStack.pop())));

        }
        else
        {
            throw InvalidTokenException(token);
        }

        prevToken =  token;
        pos += rx.matchedLength();
    }

    while (!opStack.isEmpty())
    {

        QString item  = opStack.pop();
        if (item != "(")
        {
            _tokens.append(Token<double>(getIndex(item)));

        }
        else
        {
            //throw missing right bracket exception
            throw MissingRightBracketException();
        }
    }

    //Verify token list here
    if (!verify())
        throw InvalidOperatorUseException();

    for (Token<double> tok : _tokens)
    {
        qDebug() << "Prefix Token list: " << tok.toString();
    }

}


bool Evaluator::verify()
{
    int pretend_stack_size = 0;
    for (int i = 0; i < _tokens.size(); ++i)
    {
        if (_tokens[i]._type == TokenType::CONSTANT || _tokens[i]._type == TokenType::VARIABLE) //If its z or constant we push (so add 1)
            ++pretend_stack_size;
        else if (_tokens[i]._type == TokenType::OPERATOR && _tokens[i]._data._operator < 6) //If its binary operator we pop 2 and push 1 (so decrement 1)
            --pretend_stack_size;
    }
    return (pretend_stack_size == 1);
}

bool Evaluator::isNum(QString token)
{
    bool isnum;
    token.toDouble(&isnum);
    return isnum;
}

bool Evaluator::isOp(QString token)
{
    static QRegExp rgx(operatorsRegex);
    return rgx.exactMatch(token);
}

bool Evaluator::isFunction(QString token)
{
    static QRegExp rgx(functionsRegex);
    return rgx.exactMatch(token);
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

    QStack<std::complex<double> > stack;

    for (Token<double> &tok : _tokens)
    {
        auto end = stack.end()-1;

        switch (tok._type)
        {
        case OPERATOR:
            switch (tok._data._operator)
            {
            case 0:
                *(end-1) = *(end-1) + *end;
                stack.pop();
                break;
            case 1:
                *(end-1) = *(end-1) - *end;
                stack.pop();
                break;
            case 2:
                *(end-1) = *(end-1) * *end;
                stack.pop();
                break;
            case 3:
                *(end-1) = *(end-1) / *end;
                stack.pop();
                break;
            case 4:
                *(end-1) = pow(*(end-1), *end);
                stack.pop();
                break;
            case 5:
                *(end-1) = log(*(end-1)) / log(*end);
                stack.pop();
                break;
            case 6:
                *end = -*end;
                break;
            case 7:
                *end = conj(*end);
                break;
            case 8:
                *end = sqrt(*end);
                break;
            case 9:
                *end = log(*end);
                break;
            case 10:
                *end = exp(*end);
                break;
            case 11:
                *end = sinh(*end);
                break;
            case 12:
                *end = cosh(*end);
                break;
            case 13:
                *end = tanh(*end);
                break;
            case 14:
                *end = sin(*end);
                break;
            case 15:
                *end = cos(*end);
                break;
            case 16:
                *end = tan(*end);
                break;
            case 17:
                *end = asinh(*end);
                break;
            case 18:
                *end = acosh(*end);
                break;
            case 19:
                *end = atanh(*end);
                break;
            case 20:
                *end = asin(*end);
                break;
            case 21:
                *end = acos(*end);
                break;
            case 22:
                *end = atan(*end);
                break;
            case 23:
                *end = 1.0 / (*end);
                break;
            case 24:
                *end = abs(*end);
                break;
            case 25:
                *end = arg(*end);
                break;

            }
            break;
        case CONSTANT:
            stack.push(tok._data._value);
            break;
        case VARIABLE:
            stack.push(z);
            break;
        case INVALID:
            throw InvalidTokenException(tok.toString());
        }

    }

    return stack[0];
}

int Evaluator::getPrecedence(QString token)
{
    if (token == "+" || token == "-")
        return 1;
    else if (token == "*" || token == "/")
        return 2;
    else if (token == "neg")
        return 3;
    else {
        return 4;
    }
}

int Evaluator::getIndex(QString token)
{
    int ind = Token<double>::allfunctions.indexOf(token);

    if (ind == -1)
    {
        //throw invalid operator/function exception
    }

    return ind;
}
