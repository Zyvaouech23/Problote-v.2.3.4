// services/CantineService.cpp
// Service pour récupérer le menu de cantine.
// Émet menuReady(arrayNormalized).

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include "PawnoteClient.cpp"

class CantineService : public QObject {
    Q_OBJECT
public:
    explicit CantineService(PawnoteClient* client, QObject* parent = nullptr)
        : QObject(parent), m_client(client) {
        connect(m_client, &PawnoteClient::gotResponse, this, &CantineService::onClientResponse);
        connect(m_client, &PawnoteClient::errorOccurred, this, &CantineService::onClientError);
    }

    void refresh() { if (m_client) m_client->fetchMenu(); }

signals:
    void menuReady(const QJsonArray& normalized);
    void serviceError(const QString& message);

private:
    PawnoteClient* m_client = nullptr;

    QJsonArray normalizeMenu(const QJsonArray& raw) {
        QJsonArray out;
        for (auto v : raw) {
            if (!v.isObject()) continue;
            QJsonObject o = v.toObject();
            QJsonObject n;
            QString day = o.value("day").toString();
            if (day.isEmpty()) day = o.value("date").toString();
            QString meal = o.value("meal").toString();
            if (meal.isEmpty()) meal = o.value("menu").toString();
            n.insert("day", day);
            n.insert("meal", meal);
            out.append(n);
        }
        return out;
    }

private slots:
    void onClientResponse(const QString& endpoint, const QJsonDocument& doc) {
        if (!endpoint.endsWith("/menu") && endpoint != "/menu") return;
        QJsonArray raw;
        if (doc.isObject()) raw = doc.object().value("data").toArray();
        else if (doc.isArray()) raw = doc.array();
        QJsonArray normalized = normalizeMenu(raw);
        emit menuReady(normalized);
    }

    void onClientError(const QString& endpoint, const QString& message) {
        Q_UNUSED(endpoint);
        emit serviceError(message);
    }
};

#include "CantineService.moc"
