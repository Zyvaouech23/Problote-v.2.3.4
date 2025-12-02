// services/EmploiService.cpp
// Service pour récupérer et normaliser l'emploi du temps.
// Émet timetableReady(arrayNormalized).

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include "PawnoteClient.cpp"

class EmploiService : public QObject {
    Q_OBJECT
public:
    explicit EmploiService(PawnoteClient* client, QObject* parent = nullptr)
        : QObject(parent), m_client(client) {
        connect(m_client, &PawnoteClient::gotResponse, this, &EmploiService::onClientResponse);
        connect(m_client, &PawnoteClient::errorOccurred, this, &EmploiService::onClientError);
    }

    void refresh() { if (m_client) m_client->fetchTimetable(); }

signals:
    void timetableReady(const QJsonArray& normalized);
    void serviceError(const QString& message);

private:
    PawnoteClient* m_client = nullptr;

    QJsonArray normalizeTimetable(const QJsonArray& raw) {
        // Expect entries with day, start, end, subject, room (but tolerate variations)
        QJsonArray out;
        for (auto v : raw) {
            if (!v.isObject()) continue;
            QJsonObject o = v.toObject();
            QJsonObject n;
            QString day = o.value("day").toString();
            if (day.isEmpty()) day = o.value("date").toString();
            QString start = o.value("start").toString();
            if (start.isEmpty()) start = o.value("heureDebut").toString();
            QString end = o.value("end").toString();
            if (end.isEmpty()) end = o.value("heureFin").toString();
            QString subject = o.value("subject").toString();
            if (subject.isEmpty()) subject = o.value("matiere").toString();
            QString room = o.value("room").toString();
            if (room.isEmpty()) room = o.value("salle").toString();

            n.insert("day", day);
            n.insert("start", start);
            n.insert("end", end);
            n.insert("subject", subject);
            n.insert("room", room);
            out.append(n);
        }
        return out;
    }

private slots:
    void onClientResponse(const QString& endpoint, const QJsonDocument& doc) {
        if (!endpoint.endsWith("/timetable") && endpoint != "/timetable") return;
        QJsonArray raw;
        if (doc.isObject()) raw = doc.object().value("data").toArray();
        else if (doc.isArray()) raw = doc.array();
        QJsonArray normalized = normalizeTimetable(raw);
        emit timetableReady(normalized);
    }

    void onClientError(const QString& endpoint, const QString& message) {
        Q_UNUSED(endpoint);
        emit serviceError(message);
    }
};

#include "EmploiService.moc"
