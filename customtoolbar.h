#ifndef CUSTOMTOOLBAR_H
#define CUSTOMTOOLBAR_H

#include <QToolBar>
#include <mainwindow.h>

class CustomToolbar : public QToolBar
{
private:



    QActionGroup* navigatorGroup;
    QActionGroup* allButtons; //Group of all actions that must be disabled during calculation

    QAction* newButton;
    QAction* saveButton;
    QAction* panButton;
    QAction* zoomButton;
    QAction* newtonButton;
    QAction* refreshButton;
    QAction* calculatorButton;
    QAction* undoButton;
    QAction* redoButton;
    QAction* zinButton;
    QAction* zoutButton;
    QAction* historyButton;
    QAction* centreButton;
    QAction* priorityButton;

public:
    CustomToolbar(QWidget* parent = nullptr);

public slots:

    virtual void buttonPressed(QAction *);
};

#endif // CUSTOMTOOLBAR_H
