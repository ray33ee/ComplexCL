#include "customtoolbar.h"

CustomToolbar::CustomToolbar(QWidget* parent) : QToolBar (parent)
{

    allButtons = new QActionGroup(this);

    // Initialise button group
    navigatorGroup = new QActionGroup(allButtons);

    navigatorGroup->setExclusive(true);

    // Initialise toollbar buttons
    newButton = new QAction(QIcon(":/resources/toolbar/new.png"), "", allButtons);

    saveButton = new QAction(QIcon(":/resources/toolbar/save.png"), "", allButtons);

    panButton = new QAction(QIcon(":/resources/toolbar/pan.png"), "", navigatorGroup);
    zoomButton = new QAction(QIcon(":/resources/toolbar/zoom.png"), "", navigatorGroup);
    newtonButton = new QAction(QIcon(":/resources/toolbar/newton.png"), "", navigatorGroup);

    refreshButton = new QAction(QIcon(":/resources/toolbar/refresh.png"), "", allButtons);

    calculatorButton = new QAction(QIcon(":/resources/toolbar/calculator.png"), "", allButtons);

    undoButton = new QAction(QIcon(":/resources/toolbar/undo.png"), "", allButtons);
    redoButton = new QAction(QIcon(":/resources/toolbar/redo.png"), "", allButtons);

    historyButton = new QAction(QIcon(":/resources/toolbar/history.png"), "", allButtons);

    centreButton = new QAction(QIcon(":/resources/toolbar/centre.png"), "", allButtons);

    zinButton = new QAction(QIcon(":/resources/toolbar/zoomin.png"), "", allButtons);
    zoutButton = new QAction(QIcon(":/resources/toolbar/zoomout.png"), "", allButtons);

    priorityButton = new QAction(QIcon(":/resources/toolbar/speed.png"), "", allButtons);


    //Pan and zoom are togglable
    panButton->setCheckable(true);
    zoomButton->setCheckable(true);
    newtonButton->setCheckable(true);

    //Start application with pan already checked
    panButton->setChecked(true);

    //Add buttons with tooltiptext and seperators to toolbar
    addAction(newButton);
    addSeparator();

    addAction(saveButton);
    addSeparator();

    addAction(panButton);
    addAction(zoomButton);
    addAction(newtonButton);
    addSeparator();

    addAction(refreshButton);
    addSeparator();

    addAction(calculatorButton);
    addSeparator();

    addAction(undoButton);
    addAction(redoButton);
    addSeparator();

    addAction(historyButton);
    addSeparator();

    addAction(centreButton);
    addSeparator();

    addAction(zinButton);
    addAction(zoutButton);
    addSeparator();

    addAction(priorityButton);

    newButton->setToolTip("New");

    saveButton->setToolTip("Save image as...");

    panButton->setToolTip("Pan tool");
    zoomButton->setToolTip("Rectangle zoom tool");
    newtonButton->setToolTip("Zero-finding tool");

    refreshButton->setToolTip("Refresh screen");

    calculatorButton->setToolTip("Calculator dialog");

    undoButton->setToolTip("Undo last action");
    redoButton->setToolTip("Redo last action");

    historyButton->setToolTip("Display canvas history");

    centreButton->setToolTip("Centre canvas on zero");

    zinButton->setToolTip("Zoom in");
    zoutButton->setToolTip("Zoom out");

    priorityButton->setToolTip("Toggle between speed and acuracy priority");

    connect(this, actionTriggered, this, buttonPressed);

}

void CustomToolbar::buttonPressed(QAction* button)
{
    if (button == newButton)
    {
        MainWindow* win = (MainWindow*)parent();

        win->newButtonClick();


    }
    else if (button == saveButton)
    {
        qDebug() << "Save";
    }

}
