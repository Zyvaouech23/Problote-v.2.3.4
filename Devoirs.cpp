// ui/Devoirs.cpp
// Page des devoirs avec cases à cocher et sauvegarde locale automatique.

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QCheckBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

class Devoirs : public QWidget
{
    Q_OBJECT
public:
    explicit Devoirs(QWidget* parent = nullptr) : QWidget(parent)
    {
        QVBoxLayout* v = new QVBoxLayout(this);

        table = new QTableWidget(0, 4, this);
        table->setHorizontalHeaderLabels({ "Fait", "Matière", "Titre", "Date limite" });
        table->horizontalHeader()->setStretchLastSection(true);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);

        btnRefresh = new QPushButton("Rafraîchir");

        v->addWidget(table);
        v->addWidget(btnRefresh, 0, Qt::AlignRight);

        net = new QNetworkAccessManager(this);

        loadState();

        connect(btnRefresh, &QPushButton::clicked, this, &Devoirs::refresh);
    }

    void refresh()
    {
        table->setRowCount(0);

        QNetworkRequest req(QUrl("http://127.0.0.1:5000/homeworks"));
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
                QString subject = o["subject"].toString();
                QString title = o["title"].toString();
                QString date = o["dueDate"].toString();

                QString key = subject + "|" + title + "|" + date;

                int r = table->rowCount();
                table->insertRow(r);

                QWidget* w = new QWidget();
                QHBoxLayout* hl = new QHBoxLayout(w);
                hl->setContentsMargins(0, 0, 0, 0);

                QCheckBox* cb = new QCheckBox();
                if (states.contains(key) && states[key]) cb->setChecked(true);

                connect(cb, &QCheckBox::stateChanged, this, [this, key](int st) {
                    states[key] = (st == Qt::Checked);
                    saveState();
                    });

                hl->addWidget(cb);
                hl->addStretch();
                table->setCellWidget(r, 0, w);

                table->setItem(r, 1, new QTableWidgetItem(subject));
                table->setItem(r, 2, new QTableWidgetItem(title));
                table->setItem(r, 3, new QTableWidgetItem(date));
            }
            });
    }

private:
    QString statePath() const
    {
        QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        QDir().mkpath(dir);
        return dir + "/devoirs_state.json";
    }

    void loadState()
    {
        QFile f(statePath());
        if (!f.exists()) return;
        if (!f.open(QIODevice::ReadOnly)) return;

        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        if (!doc.isObject()) return;

        QJsonObject o = doc.object();
        for (auto key : o.keys()) states[key] = o[key].toBool();
    }

    void saveState()
    {
        QFile f(statePath());
        if (!f.open(QIODevice::WriteOnly)) return;

        QJsonObject o;
        for (auto it = states.begin(); it != states.end(); ++it)
            o[it.key()] = it.value();

        f.write(QJsonDocument(o).toJson(QJsonDocument::Compact));
        f.close();
    }

private:
    QTableWidget* table;
    QPushButton* btnRefresh;
    QNetworkAccessManager* net;
    QMap<QString, bool> states;
};

#include "Devoirs.moc"
