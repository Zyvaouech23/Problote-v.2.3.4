// src/Navigation.cpp
// Widget de navigation qui contient les onglets : Notes, Devoirs, Emploi du temps, Cantine.
// Déclaration + définition regroupées pour simplifier l'ajout au projet.

#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>

// Header-like declaration
class Navigation : public QWidget
{
    Q_OBJECT
public:
    explicit Navigation(QWidget* parent = nullptr);
    ~Navigation() override = default;

    // API publique pour rafraîchir les données (appel depuis App ou un service)
    void refreshAll();                 // demande de rafraîchissement général
    void refreshNotesFromBackend();    // récupère /grades
    void refreshHomeworksFromBackend();// récupère /homeworks
    void refreshTimetableFromBackend();// récupère /timetable
    void refreshMenuFromBackend();     // récupère /menu

private:
    QTabWidget* tabs = nullptr;

    // Notes tab widgets
    QWidget* notesPage = nullptr;
    QTableWidget* notesTable = nullptr;
    QLabel* overallAverageLabel = nullptr;

    // Devoirs tab widgets
    QWidget* homeworksPage = nullptr;
    QTableWidget* homeworkTable = nullptr;
    QPushButton* homeworkRefreshBtn = nullptr;

    // Emploi tab widgets
    QWidget* timetablePage = nullptr;
    QListWidget* timetableList = nullptr;

    // Cantine tab widgets
    QWidget* menuPage = nullptr;
    QListWidget* menuList = nullptr;

    // Persistance locale pour états des devoirs
    QString homeworksStateFilePath() const;
    void loadLocalHomeworkState();
    void saveLocalHomeworkState();

    // local cache of homework states: key -> done
    QMap<QString, bool> localHomeworkStates;
};

// Implementation

Navigation::Navigation(QWidget* parent) : QWidget(parent)
{
    tabs = new QTabWidget(this);

    // ---------- NOTES ----------
    notesPage = new QWidget;
    {
        QVBoxLayout* v = new QVBoxLayout(notesPage);
        notesTable = new QTableWidget(0, 5, this);
        notesTable->setHorizontalHeaderLabels({ "Matière", "Date", "Type", "Note", "Coef" });
        notesTable->horizontalHeader()->setStretchLastSection(true);
        notesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        v->addWidget(notesTable);

        overallAverageLabel = new QLabel("Moyenne générale : —");
        v->addWidget(overallAverageLabel, 0, Qt::AlignLeft);
    }
    tabs->addTab(notesPage, "Notes");

    // ---------- DEVOIRS ----------
    homeworksPage = new QWidget;
    {
        QVBoxLayout* v = new QVBoxLayout(homeworksPage);
        homeworkTable = new QTableWidget(0, 4, this);
        homeworkTable->setHorizontalHeaderLabels({ "Fait", "Matière", "Titre", "Date limite" });
        homeworkTable->horizontalHeader()->setStretchLastSection(true);
        homeworkTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        v->addWidget(homeworkTable);

        homeworkRefreshBtn = new QPushButton("Rafraîchir depuis Pawnote");
        v->addWidget(homeworkRefreshBtn, 0, Qt::AlignRight);
        connect(homeworkRefreshBtn, &QPushButton::clicked, this, &Navigation::refreshHomeworksFromBackend);

        // charger états locaux
        loadLocalHomeworkState();
    }
    tabs->addTab(homeworksPage, "Devoirs");

    // ---------- EMPLOI DU TEMPS ----------
    timetablePage = new QWidget;
    {
        QVBoxLayout* v = new QVBoxLayout(timetablePage);
        timetableList = new QListWidget(this);
        v->addWidget(timetableList);
    }
    tabs->addTab(timetablePage, "Emploi du temps");

    // ---------- CANTINE ----------
    menuPage = new QWidget;
    {
        QVBoxLayout* v = new QVBoxLayout(menuPage);
        menuList = new QListWidget(this);
        v->addWidget(menuList);
    }
    tabs->addTab(menuPage, "Cantine");

    // Layout principal
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabs);
    setLayout(mainLayout);

    // Connexions internes (ex : sauvegarde automatique à chaque changement)
    // Nous écoutons les changements de checkbox via un timer ou via l'appel explicite saveLocalHomeworkState()
}

// Chemins pour sauvegarde locale
QString Navigation::homeworksStateFilePath() const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(dir);
    return dir + QDir::separator() + "homeworks_state.json";
}

void Navigation::loadLocalHomeworkState()
{
    QString path = homeworksStateFilePath();
    QFile f(path);
    if (!f.exists()) return;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();
    localHomeworkStates.clear();
    for (auto key : obj.keys()) localHomeworkStates[key] = obj.value(key).toBool();
}

void Navigation::saveLocalHomeworkState()
{
    QString path = homeworksStateFilePath();
    QJsonObject obj;
    for (auto it = localHomeworkStates.begin(); it != localHomeworkStates.end(); ++it) {
        obj.insert(it.key(), QJsonValue(it.value()));
    }
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        f.write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
        f.close();
    }
}

// Placeholder : appeler cette fonction pour rafraîchir toutes les vues (utilisé par App)
void Navigation::refreshAll()
{
    refreshNotesFromBackend();
    refreshHomeworksFromBackend();
    refreshTimetableFromBackend();
    refreshMenuFromBackend();
}

// Les méthodes suivantes sont des "stubs" : elles doivent être reliées au backend HTTP (Pawnote service).
// J'indique comment procéder : faire une requête GET vers http://127.0.0.1:5000/grades (grades -> JSON array data)
// et parser la réponse pour remplir notesTable, calculer moyennes, etc.
// Ici on fournit uniquement la logique d'insertion locale pour l'UI.

void Navigation::refreshNotesFromBackend()
{
    // TODO: appeler QNetworkAccessManager pour GET http://127.0.0.1:5000/grades
    // Exemple de parsing attendu :
    // JSON { data: [ { subject: "...", value: 14.5, coef: 3, date: "...", type: "DS" }, ... ] }
    // Pour l'instant on vide la table (stub)
    notesTable->setRowCount(0);
    // Exemple de ligne de test (supprimer après intégration backend)
    int r = notesTable->rowCount();
    notesTable->insertRow(r);
    notesTable->setItem(r, 0, new QTableWidgetItem("Maths"));
    notesTable->setItem(r, 1, new QTableWidgetItem("2025-06-10"));
    notesTable->setItem(r, 2, new QTableWidgetItem("DS"));
    notesTable->setItem(r, 3, new QTableWidgetItem("15.5"));
    notesTable->setItem(r, 4, new QTableWidgetItem("3"));

    // Après remplissage réel : appeler calcul des moyennes et mise à jour du label overallAverageLabel
    // computeAverages();
}

void Navigation::refreshHomeworksFromBackend()
{
    // TODO: GET http://127.0.0.1:5000/homeworks and parse JSON { data: [...] }
    // Ici on vide et ajoute un exemple. Lors du parsing, créer une clé unique pour chaque devoir et
    // appliquer localHomeworkStates[key] si présent pour cocher la checkbox.
    homeworkTable->setRowCount(0);

    // Exemple d'insertion (remplacer par parsing réel)
    QJsonObject example;
    example["subject"] = "Physique";
    example["title"] = "Exercice chap. 4";
    example["dueDate"] = "2025-06-12";

    int r = homeworkTable->rowCount();
    homeworkTable->insertRow(r);

    // Checkbox cell
    QWidget* cbw = new QWidget();
    QHBoxLayout* hl = new QHBoxLayout(cbw);
    hl->setContentsMargins(0, 0, 0, 0);
    QCheckBox* cb = new QCheckBox();
    QString key = example["subject"].toString() + "|" + example["title"].toString() + "|" + example["dueDate"].toString();
    if (localHomeworkStates.contains(key) && localHomeworkStates[key]) cb->setChecked(true);
    connect(cb, &QCheckBox::stateChanged, [this, key](int state) {
        localHomeworkStates[key] = (state == Qt::Checked);
        saveLocalHomeworkState();
        });
    hl->addWidget(cb);
    hl->addStretch();
    homeworkTable->setCellWidget(r, 0, cbw);

    homeworkTable->setItem(r, 1, new QTableWidgetItem(example["subject"].toString()));
    homeworkTable->setItem(r, 2, new QTableWidgetItem(example["title"].toString()));
    homeworkTable->setItem(r, 3, new QTableWidgetItem(example["dueDate"].toString()));
}

void Navigation::refreshTimetableFromBackend()
{
    // TODO: GET http://127.0.0.1:5000/timetable and parse.
    // Ici exemple statique :
    timetableList->clear();
    timetableList->addItem("Lundi 8:00 - 9:00 : Maths (Salle 12)");
    timetableList->addItem("Lundi 9:00 - 10:00 : Français (Salle 3)");
}

void Navigation::refreshMenuFromBackend()
{
    // TODO: GET http://127.0.0.1:5000/menu and parse.
    menuList->clear();
    menuList->addItem("Lundi : Poulet rôti / Riz / Yaourt");
    menuList->addItem("Mardi : Pâtes / Salade / Fruit");
}

// moc include for QObject meta-data (required when class is in .cpp)
#include "Navigation.moc"
