// ui/EmploiPage.cpp
// Page affichant l'emploi du temps.

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>

class EmploiPage : public QWidget
{
    Q_OBJECT
public:
    explicit EmploiPage(QWidget* parent = nullptr) : QWidget(parent)
    {
        QVBoxLayout* v = new QVBoxLayout(this);
        list = new QListWidget(this);

        v->addWidget(list);
        net = new QNetworkAccessManager(this);
    }

    void refresh()
    {
        list->clear();

        QNetworkRequest req(QUrl("http://127.0.0.1:5000/timetable"));
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
            for (auto val : arr) {
                QJsonObject o = val.toObject();
                QString s = o["day"].toString() + " "
                    + o["start"].toString() + " - " + o["end"].toString()
                    + " : " + o["subject"].toString()
                    + " (Salle " + o["room"].toString() + ")";
                list->addItem(s);
            }
            });
    }

private:
    QListWidget* list;
    QNetworkAccessManager* net;
};

#include "EmploiPage.moc"
