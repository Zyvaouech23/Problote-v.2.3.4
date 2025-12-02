// src/main.cpp
#include <QApplication>
#include "App.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    // Application principale
    App window;
    window.resize(1200, 800);
    window.show();

    return app.exec();
}
