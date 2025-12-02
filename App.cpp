// src/App.cpp
// Implémentation de la fenêtre principale (classe App).
// Fichier autonome : contient déclaration + définition pour faciliter l'intégration.

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QApplication>
#include <QIcon>
#include "Navigation.h"

class App : public QMainWindow
{
    Q_OBJECT
public:
    App(QWidget* parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("MonAssistantScolaire");
        setWindowIcon(QIcon(":/assets/logo.png")); // mettre logo dans resources ou assets/

        // Charger style si présent
        loadStyleSheet("assets/style.qss");

        // Navigation principale : onglets Notes / Devoirs / Emploi / Cantine
        nav = new Navigation(this);
        setCentralWidget(nav);

        // Exemple : on peut exposer un slot pour démarrer des timers/services plus tard
        // connect(this, &App::startServices, this, &App::onStartServices);

        // taille par défaut et icon
        setMinimumSize(1000, 600);
    }

    ~App() override = default;

private:
    Navigation* nav = nullptr;

    void loadStyleSheet(const QString& path)
    {
        QFile f(path);
        if (f.open(QFile::ReadOnly | QFile::Text)) {
            QString style = QString::fromUtf8(f.readAll());
            qApp->setStyleSheet(style);
            f.close();
        }
        else {
            qDebug() << "Style QSS introuvable:" << QDir::absoluteFilePath(path);
        }
    }

    // Exemple de méthode à implémenter plus tard
    void onStartServices()
    {
        // lancer backend local (Node.js pawnote) si besoin, ou vérifier qu'il tourne
        // et appeler nav->refreshAll() pour charger les données initiales.
    }

signals:
    void startServices();
};

// Qt meta-object macro nécessaire quand la classe contient Q_OBJECT et
// qu'on place la définition dans un .cpp. Il faut que le fichier soit compilé
// avec moc (CMake/Qt s'en chargera).
#include "App.moc"
