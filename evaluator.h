#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <complex>
#include <QVector>
#include <QString>
#include <token.h>

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
