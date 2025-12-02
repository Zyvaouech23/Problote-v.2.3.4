// services/DevoirsService.cpp
// Service qui récupère les devoirs via PawnoteClient et normalise.
// Émet homeworksReady(arrayNormalized).

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include "PawnoteClient.cpp"

class DevoirsService : public QObject {
    Q_OBJECT
public:
    explicit DevoirsService(PawnoteClient* client, QObject* parent = nullptr)
        : QObject(parent), m_client(client) {
        connect(m_client, &PawnoteClient::gotResponse, this, &DevoirsService::onClientResponse);
        connect(m_client, &PawnoteClient::errorOccurred, this, &DevoirsService::onClientError);
    }

    void refresh() { if (m_client) m_client->fetchHomeworks(); }

signals:
    void homeworksReady(const QJsonArray& normalized);
    void serviceError(const QString& message);

private:
    PawnoteClient* m_client = nullptr;

    QJsonArray normalizeHomeworks(const QJsonArray& raw) {
        QJsonArray out;
        for (auto v : raw) {
            if (!v.isObject()) continue;
            QJsonObject o = v.toObject();
            QJsonObject n;
            QString subject = o.value("subject").toString();
            if (subject.isEmpty()) subject = o.value("matiere").toString();
            QString title = o.value("title").toString();
            if (title.isEmpty()) title = o.value("libelle").toString();
            QString due = o.value("dueDate").toString();
            if (due.isEmpty()) due = o.value("date").toString();

            // try ISO formatting if timestamp present
            if (o.contains("dueTimestamp")) {
                qint64 ts = o.value("dueTimestamp").toVariant().toLongLong();
                QDateTime dt = QDateTime::fromSecsSinceEpoch(ts);
                due = dt.toString(Qt::ISODate);
            }

            n.insert("subject", subject);
            n.insert("title", title);
            n.insert("dueDate", due);
            n.insert("description", o.value("description").toString());
            n.insert("assignedDate", o.value("assignedDate").toString());
            out.append(n);
        }
        return out;
    }

private slots:
    void onClientResponse(const QString& endpoint, const QJsonDocument& doc) {
        if (!endpoint.endsWith("/homeworks") && endpoint != "/homeworks") return;
        QJsonArray raw;
        if (doc.isObject()) raw = doc.object().value("data").toArray();
        else if (doc.isArray()) raw = doc.array();
        QJsonArray normalized = normalizeHomeworks(raw);
        emit homeworksReady(normalized);
    }

    void onClientError(const QString& endpoint, const QString& message) {
        Q_UNUSED(endpoint);
        emit serviceError(message);
    }
};

#include "DevoirsService.moc"
