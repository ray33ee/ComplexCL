#ifndef HISTORY_H
#define HISTORY_H

#include <QVector>
#include <QString>

#ifdef QT_DEBUG
#include <QDebug>
#endif

template <class T>
class History
{
private:

    QVector<T> _history;

    int _pointer;

public:
    History() : _pointer(0) { }

    void add(const T &val)
    {
        if (_pointer == _history.size())
            _history.append(val);
        else
        {
            _history[_pointer] = val;
            _history = QVector<T>(_history.mid(0, _pointer+1));
        }

        ++_pointer;
    }

    T undo() { return _history[--_pointer-1]; }

    T redo() { return _history[_pointer++]; }

    T revert(int i) { _pointer = i+1; return _history[i]; }

    bool isAtBottom() const { return _pointer == 1; }

    bool isAtTop() const { return _pointer == _history.size(); }

    int getPointer() const { return _pointer; }

    QVector<T> getArray() const { return _history; }

    QString toString()
    {
        QString ans = "";

        for (const T &land : _history)
            ans += land.toString() + " ";

        return ans;
    }


};


#endif // HISTORY_H
