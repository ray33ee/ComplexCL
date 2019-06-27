#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <complex>
#include <cmath>
#include <bits/stdc++.h>
#include <QVector>
#include <QString>
#include <QRegularExpression>
#include <QStack>
#include <exception>

#include <QDebug>

enum TokenType
{
    INVALID = 0,
    VARIABLE = 1,
    OPERATOR = 2,
    CONSTANT = 3
};

namespace std
{
    QString toString(complex<double> z);
};

/**
 * @brief The Token class represents a single token in a mathematical formula
 */
template <class T>
class Token
{
public:



    /** List of all valid operators and functions */
    static QStringList allfunctions;

    union TokenData
    {
        std::complex<T>    _value;
        int                 _operator;

        TokenData(std::complex<T> val): _value(val) { }
        TokenData(int val): _operator(val) { }
    } _data;

    TokenType _type;

    Token() :  _data(0), _type(INVALID) {}

    Token(TokenType type, std::complex<T> data = 0.0) : _data(data), _type(type) {  }
    Token(int op) : _data(op), _type(OPERATOR) {  }

    QString toString() const
    {
        if (_type == VARIABLE)
            return "z";
        else if (_type == CONSTANT)
            return QString() + "{ " + QString::number(_data._value.real()) + ", " + QString::number(_data._value.imag()) + " }";
        else if (_type == OPERATOR)
            return QString() + "{ " + allfunctions[_data._operator] + " }";

        return QString() + "Error in Token::toString() function - Invalid type " + _type + ".";
    }
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
    QVector<Token<double> > _tokens;

    /**
     * @brief _tokens contains the list of tokens produced by the formula
     */
    QVector<Token<float> > _floatTokens;

    /**
     * @brief _formula the original string used
     */
    QString _formula;

    /** Pattern to match floating point numbers. Distinguishing unary and binary +- operators */
    static QString floatingRegex;

    /** Pattern to match supported functions */
    static QString functionsRegex;

    /** Pattern to match supported constants */
    static QString constantsRegex;

    /** Pattern to match supported operators */
    static QString operatorsRegex;

    /** Pattern to match string function labels */
    static QString labelRegex;

    /** Pattern to match tokens in equation */
    static QString equationRegex;


public:

    /* Base exception from which all parse-based exceptions inherit */
    class EvaluatorParseException : public std::exception
    {
    private:
        QString _token;

    public:

        EvaluatorParseException(QString token): std::exception(), _token(token) {}

        QString getToken() const { return _token; }

    };

    class FloatingFormatException : public EvaluatorParseException
    {
    public:
        FloatingFormatException(QString token): EvaluatorParseException(token) {}

        virtual const char* what() const throw() { return (QString() + "Invalid number format - " + getToken()).toStdString().data(); }
    };

    class FloatingOverflowException : public EvaluatorParseException
    {
    public:
        FloatingOverflowException(QString token): EvaluatorParseException(token) {}

        virtual const char* what() const throw() { return (QString() + "Number (" + getToken() + ") is too large to represent.").toStdString().data(); }
    };

    class InvalidOperatorUseException : public EvaluatorParseException
    {
    public:
        InvalidOperatorUseException(): EvaluatorParseException("") {}

        virtual const char* what() const throw() { return (QString() + "Invalid use of operator detected in formula.").toStdString().data(); }
    };

    class InvalidTokenException : public EvaluatorParseException
    {
    public:
        InvalidTokenException(QString token): EvaluatorParseException(token) {}

        virtual const char* what() const throw() { return (QString() + "Invalid token '" + getToken() + "' detected in formula.").toStdString().data(); }
    };

    class MissingLeftBracketException : public EvaluatorParseException
    {
    public:
        MissingLeftBracketException(): EvaluatorParseException("") {}

        virtual const char* what() const throw() { return (QString() + "Missing left bracket in formula").toStdString().data(); }
    };

    class MissingRightBracketException : public EvaluatorParseException
    {
    public:
        MissingRightBracketException(): EvaluatorParseException("") {}

        virtual const char* what() const throw() { return (QString() + "Missing right bracket in formula").toStdString().data(); }
    };



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
     * @brief setString converts the string into the token list
     * @param formula equation to convert
     */
    void setString(QString formula);

    /**
     * @brief getTokens gets the pointer to the array of tokens using double precision
     * @return the list of tokens
     */
    Token<double>* getTokens();

    /**
     * @brief getTokens gets the pointer to the array of tokens using single precision
     * @return the list of tokens
     */
    Token<float>* getFloatTokens();

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

    /**
     * @brief verify() performs a quick run of the equation, and returns true if a single element remains on the virtual stack, else returns false.
     * @return true if the tokenlist is correct, otherwise false
     */
    bool verify();

    /**
     * @brief isNum Returns true if token is a number as defined by the floatingRegex regex
     * @param token string to test
     * @return true if the token is a valid number
     */
    static bool isNum(QString token);

    /**
     * @brief isOp determines whether the token is an operator, as defined by the operatorRegex
     * @param token the token to test
     * @return true if the token is an operator
     */
    static bool isOp(QString token);

    /**
     * @brief isFunction determines whether the token is a function name, as defined by functionsRegex
     * @param token the token to test
     * @return true if the token is a function
     */
    static bool isFunction(QString token);

    /**
     * @brief getPrecedence gets the numerical precedence of the operator
     * @param token operator
     * @return precedence of operator
     */
    static int getPrecedence(QString token);

    /**
     * @brief getIndex gets thje numerical identifier of the function or operator, as defined by allfunctions
     * @param token the operator or function
     * @return an index from 0-25 representing the function
     */
    static int getIndex(QString token);
};

/**
 * @brief The Landscape class is functionally similar to the Evaluator class, but contains extra properties for the minimum and maximum domain. This
 * class contains all the information needed to plot a canvas
 */
class Landscape : public Evaluator
{
private:
    std::complex<double> _min;
    std::complex<double> _diff;

public:

    Landscape():
        Evaluator(), _min({-10, -10}), _diff({20, 20})
    {

    }

    Landscape(QString formula, const std::complex<double> &min, const std::complex<double> &max):
        Evaluator(formula), _min(min), _diff(max - min)
    {

    }

    void setDomain(const std::complex<double> &min, const std::complex<double> &max)
    {
        _min = min;
        _diff = max - min;
    }

    void setDomain(QString min, QString max)
    {
        Evaluator Min;
        Evaluator Max;

        /*try
        {*/
        Min.setString(min);
        Max.setString(max);

        /*}
        catch (Evaluator::EvaluatorParseException e)
        {
            QMessageBox::warning(this, "ComplexCL", "Formula parse error - " + QString(e.what()), QMessageBox::Ok);
            return;
        }*/

        _min = Min(0.0);
        _diff = Max(0.0) - _min;


    }

    void setMinDiff(const std::complex<double> &min, const std::complex<double> &diff)
    {
        _min = min;
        _diff = diff;
    }


    std::complex<double> operator()(const std::complex<double> &z) { return Evaluator::operator()(z); }


    std::complex<double> getMin() const { return _min; }
    std::complex<double> getMax() const { return _min + _diff; }
    std::complex<double> getDiff() const { return _diff; }

    QString toString() const { return QString() + getFormula() + ", from {" + QString::number(getMin().real()) + ", " + QString::number(getMin().imag()) + "} to {"  + QString::number(getMin().real()) + ", " + QString::number(getMin().imag()) + "}"; }



};



#endif // EVALUATOR_H
