// ui/NotesPage.cpp
// Page affichant les notes, moyennes par matière et moyenne générale.

#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class NotesPage : public QWidget
{
    Q_OBJECT
public:
    explicit NotesPage(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout* v = new QVBoxLayout(this);

        table = new QTableWidget(0, 5, this);
        table->setHorizontalHeaderLabels({ "Matière", "Date", "Type", "Note", "Coef" });
        table->horizontalHeader()->setStretchLastSection(true);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);

        moyenneGen = new QLabel("Moyenne générale : —");

        v->addWidget(table);
        v->addWidget(moyenneGen);

        net = new QNetworkAccessManager(this);
    }

    void refresh()
    {
        table->setRowCount(0);

        QNetworkRequest req(QUrl("http://127.0.0.1:5000/grades"));
        QNetworkReply* rep = net->get(req);

        connect(rep, &QNetworkReply::finished, this, [this, rep]() {
            if (rep->error() != QNetworkReply::NoError) {
                rep->deleteLater();
                return;
            }
            QJsonDocument doc = QJsonDocument::fromJson(rep->readAll());
            rep->deleteLater();
            if (!doc.isObject()) return;

            QJsonArray arr = doc.object().value("data").toArray();

            struct Stats { double somme = 0; double coef = 0; };
            QMap<QString, Stats> mapStats;

            for (auto val : arr) {
                QJsonObject o = val.toObject();
                QString s = o["subject"].toString();
                double note = o["value"].toDouble();
                double c = o["coef"].toDouble();

                int r = table->rowCount();
                table->insertRow(r);
                table->setItem(r, 0, new QTableWidgetItem(s));
                table->setItem(r, 1, new QTableWidgetItem(o["date"].toString()));
                table->setItem(r, 2, new QTableWidgetItem(o["type"].toString()));
                table->setItem(r, 3, new QTableWidgetItem(QString::number(note)));
                table->setItem(r, 4, new QTableWidgetItem(QString::number(c)));

                mapStats[s].somme += note * c;
                mapStats[s].coef += c;
            }

            double totalSomme = 0;
            double totalCoef = 0;
            for (auto it = mapStats.begin(); it != mapStats.end(); ++it) {
                totalSomme += it.value().somme;
                totalCoef += it.value().coef;
            }

            if (totalCoef > 0)
                moyenneGen->setText("Moyenne générale : " + QString::number(totalSomme / totalCoef, 'f', 2));
            else
                moyenneGen->setText("Moyenne générale : —");
            });
    }

private:
    QTableWidget* table;
    QLabel* moyenneGen;
    QNetworkAccessManager* net;
};

#include "NotesPage.moc"
