#include "landscape.h"
#include <math.h>

#define R_PI M_PI
#define R_PI_2 M_PI_2
#define R_2_PI (6.28318530717958647692)     //  2*pi
#define R_1_2_PI (0.159154943091895335769)  //  1/(2*pi)

QString Evaluator::floatingRegex = "(?:(?:(?<=[(*/^])|^)[+-]*)?(?:\\b[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+\\b)(?:e[-+]*?[0-9]+\\b)?";


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

namespace std
{
    QString toString(const complex<double> &z, int precision)
    {
        /*double im = abs(z.imag());
        return QString() + QString::number(z.real(), 'g', precision) + (z.imag() < 0 ? " - " : " + ") + QString::number(im, 'g', precision) + "*i";
    */

        if (z.imag() == 0.0) //If the number is real
            return QString::number(z.real(), 'g', precision);

        if (z.real() == 0.0) //If the number is exclusively imaginary
            return abs(z.imag()) == 1.0 ? (z.imag() < 0 ? "-i" : "i") : (QString::number(z.imag(), 'g', precision) + "*i");

        return QString::number(z.real(), 'g', precision) + (z.imag() < 0 ? " - " : " + ") + (abs(z.imag()) == 1 ? "i" : (QString::number(abs(z.imag()), 'g', precision) + "*i"));

    }

    QString toPolarString(const complex<double> & z)
    {
        if (real(z) == 0.0 && imag(z) == 0.0)
            return QString("0");
        return QString::number(abs(z)) + "\u2220" + QString::number(arg(z));
    }

    inline bool isNaN(const complex<double> &z) { return isnan(z.real()) || isnan(z.imag()); }

    inline bool isInf(const complex<double> &z) { return (isinf(z.real()) || isinf(z.imag())) && !isNaN(z); }

    inline bool isZero(const complex<double> &z) { return z.real() == 0.0 && z.imag() == 0.0; }

    /**
     * Function gets the absolute value of the complex number
     * @param z the complex number
     * @return the absolute value of the complex number z, |z|.
     */
    double c_abs(const complex<double> & z)
    {
        if (isNaN(z))
            return NAN;

        if (isInf(z))
            return INFINITY;

        return hypot(z.real(), z.imag());
    }

    /**
     * Function gets the argument of the complex number
     * @param z the complex number
     * @return the argument, arg(z).
     */
    double c_arg(const complex<double> & z) { return atan2(z.imag(), z.real()); }

    /**
     * HueToRGB is called by HLtoRGB to facilitate conversion
     */
    float  HueToRGB(float p, float q, float h)
    {
        if (h < 0) h += 1;

        if (h > 1 ) h -= 1;

        if (6 * h < 1) return p + ((q - p) * 6 * h);

        if (2 * h < 1 ) return  q;

        if (3 * h < 2) return p + ( (q - p) * 6 * ((2.0f / 3.0f) - h) );

        return p;
    }

    int ARGB_constructor(int r, int g, int b)
    {
        return b | g << 8 | r << 16 | 0xFF000000;
    }

    /**
     * HLtoRGB converts a hue and lightness value to an ARGB object
     * @param h hue number from 0 to 1
     * @param l lightness value from 0 to 1
     * @return a converted ARGB object
     */
    int HLtoRGB(float h, float l)
    {
        float q = l < 0.5 ? l*2 : 1;

        float p = 2 * l - q;

        float r = max(0.0f, HueToRGB(p, q, h + (1.0f / 3.0f)));
        float g = max(0.0f, HueToRGB(p, q, h));
        float b = max(0.0f, HueToRGB(p, q, h - (1.0f / 3.0f)));

        r = min(r, 1.0f);
        g = min(g, 1.0f);
        b = min(b, 1.0f);

        return ARGB_constructor( r*255, g*255, b*255);
    }

    /**
     * Gets the colour of the complex number as per the domain colouring algorithm
     * @param z the complex number
     * @return the colour, as an ARGB type
     */
    int c_colour(const complex<double> &z)
    {

        //If z is zero, or contains a NaN component display as white
        if (isZero(z) || isNaN(z) )
            return ARGB_constructor (255, 255, 255);

        //Both components are +/-inf
        if (isinf(z.real()) && isinf(z.imag()))
        {
            if (z.real() > 0 && z.imag() > 0)
                return ARGB_constructor (255, 0, 191) ;
            else if (z.real() < 0 && z.imag() > 0)
                return ARGB_constructor (0, 64, 255) ;
            else if (z.real() < 0 && z.imag() < 0)
                return ARGB_constructor (0, 255, 64) ;
            else
                return ARGB_constructor (255, 191, 0) ;
        }

        //Either one, or the other component is +/1 inf
        if (isInf(z))
        {
            if (isinf(z.real()) && z.real() > 0)
                return ARGB_constructor (255, 0, 0) ;
            else if (isinf(z.real()) && z.real() < 0)
                return ARGB_constructor (0, 255, 255) ;
            else if(isinf(z.imag()) && z.imag() > 0)
                return ARGB_constructor (128, 0, 255) ;
            else
                return ARGB_constructor (128, 255, 0) ;
        }

        double arg = c_arg(z);
        double hue = arg;
        double modarg = log(c_abs(z));
        double lightness;

        //Convert argument from -pi to pi --> 0 to 2pi
        if (arg < 0)
            hue = R_2_PI + arg;

        //Convert from 0 to 2pi --> 0 to 1
        hue = 1.0 - hue * R_1_2_PI;

        if (modarg < 0)
        {
            lightness = 0.75 - c_abs(z) / 2.0;
        }
        else
        {
            if (!((int)modarg & 1)) //If whole part of modarg is even, 0 --> 1 maps to black --> white
                    lightness =  fmin( modarg - floor(modarg), (double)(0x1.fffffep-1f) ) / 2.0 + 0.25;
            else //If whole part of modarg is odd 0 --> 1 maps to white --> black
                    lightness = 0.75 - (modarg - floor(modarg)) / 2.0;
        }

        return HLtoRGB(hue, lightness);
    }

    int toColor(const complex<double> &z)
    {

        return c_colour(z);
    }
};

Evaluator::Evaluator()
{
    setString("z");
}

Evaluator::Evaluator(QString formula)
{
    setString(formula);
}

bool Evaluator::isUnaryNegate(QVector<Token<double> > tokens, QStack<QString> opstack, QString prev)
{

    return ((tokens.isEmpty() && opstack.isEmpty()) || prev == "(" || prev == "neg" || isOp(prev));
}

void Evaluator::setString(QString formula)
{
    _formula = formula;

    _tokens.clear();

    formula.remove(QRegExp("[ \n\t]"));

    QString prevToken = "";

    QStack<QString> opStack;

    static QRegularExpression rx(equationRegex);

    //static auto isUnaryNegate = [this, opStack, prevToken](QString token) { return token == "-" && ; };

    auto global = rx.globalMatch(formula);

    while (global.hasNext())
    {

        QString token = global.next().captured(1);

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
            if (token == "-" && isUnaryNegate(_tokens, opStack, prevToken))
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
                {
                    break;
                }
            }

            if (opStack.isEmpty())
            {
                throw MissingLeftBracketException();
            }

            opStack.pop(); //Pop the bracket

            if (!opStack.isEmpty())
                if (isFunction(opStack.top()) || isOp(opStack.top()))
                    _tokens.append(Token<double>(getIndex(opStack.pop())));

        }
        else
        {
            throw InvalidTokenException(token);
        }

        prevToken =  token;
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


   /* qDebug() << "Formula: " << formula;

    for (int cnt = 0; cnt < getCount(); cnt++)
        qDebug() << "    " << getTokens()[cnt].toString();*/

    //Verify token list here
    if (!verify())
        throw InvalidOperatorUseException();

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

std::complex<double> Evaluator::operator()(const std::complex<double> &z) const
{

    QStack<std::complex<double> > stack;

    for (const Token<double> &tok : _tokens)
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
