// ui/CantinePage.cpp
// Page affichant le menu de la semaine.

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>

class CantinePage : public QWidget
{
    Q_OBJECT
public:
    explicit CantinePage(QWidget* parent = nullptr) : QWidget(parent)
    {
        QVBoxLayout* v = new QVBoxLayout(this);
        list = new QListWidget(this);
        v->addWidget(list);

        net = new QNetworkAccessManager(this);
    }

    void refresh()
    {
        list->clear();

        QNetworkRequest req(QUrl("http://127.0.0.1:5000/menu"));
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
                QString line = o["day"].toString() + " : " + o["meal"].toString();
                list->addItem(line);
            }
            });
    }

private:
    QListWidget* list;
    QNetworkAccessManager* net;
};

#include "CantinePage.moc"
