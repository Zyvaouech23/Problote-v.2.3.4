// services/NotesService.cpp
// Service qui récupère les notes via PawnoteClient, normalise les champs,
// calcule moyennes par matière et moyenne générale pondérée.
// Émet gradeListReady(arrNormalized) et averagesReady(mapSubject->avg, overall).

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QPair>
#include <QStringList>
#include "PawnoteClient.cpp" // inclure le client (déclaration + impl dans même fichier pour simplifier)

class NotesService : public QObject {
    Q_OBJECT
public:
    explicit NotesService(PawnoteClient* client, QObject* parent = nullptr)
        : QObject(parent), m_client(client) {
        connect(m_client, &PawnoteClient::gotResponse, this, &NotesService::onClientResponse);
        connect(m_client, &PawnoteClient::errorOccurred, this, &NotesService::onClientError);
    }

    void refresh() { if (m_client) m_client->fetchGrades(); }

signals:
    void gradeListReady(const QJsonArray& normalized);
    // subject -> average (double)
    void averagesReady(const QMap<QString, double>& perSubjectAvg, double overallWeightedAvg);
    void serviceError(const QString& message);

private:
    PawnoteClient* m_client = nullptr;

    // Normalisation tolérante : accepte plusieurs schémas JSON
    QJsonArray normalizeGrades(const QJsonArray& raw) {
        QJsonArray out;
        for (auto v : raw) {
            if (!v.isObject()) continue;
            QJsonObject o = v.toObject();
            QJsonObject n;
            // champs habituels possibles : subject, value, coef, date, type, comment
            QString subject = o.value("subject").toString();
            if (subject.isEmpty()) subject = o.value("matiere").toString();
            if (subject.isEmpty()) subject = o.value("discipline").toObject().value("label").toString();

            double value = 0;
            if (o.contains("value")) value = o.value("value").toDouble();
            else if (o.contains("note")) value = o.value("note").toDouble();
            else if (o.contains("valeur")) value = o.value("valeur").toDouble();

            double coef = 1.0;
            if (o.contains("coef")) coef = o.value("coef").toDouble(1.0);
            else if (o.contains("coefficient")) coef = o.value("coefficient").toDouble(1.0);

            n.insert("subject", subject);
            n.insert("value", value);
            n.insert("coef", coef);
            n.insert("date", o.value("date").toString());
            n.insert("type", o.value("type").toString());
            n.insert("comment", o.value("comment").toString());
            out.append(n);
        }
        return out;
    }

    void computeAveragesAndEmit(const QJsonArray& arr) {
        struct Tot { double sumWeighted = 0; double sumCoef = 0; };
        QMap<QString, Tot> map;
        for (auto v : arr) {
            if (!v.isObject()) continue;
            QJsonObject o = v.toObject();
            QString s = o.value("subject").toString();
            double val = o.value("value").toDouble();
            double coef = o.value("coef").toDouble(1.0);
            if (s.isEmpty()) s = QStringLiteral("Inconnue");
            // skip invalid notes (negative or zero?) we accept 0 but treat negative as missing
            if (val < 0) continue;
            Tot t = map.value(s);
            t.sumWeighted += val * coef;
            t.sumCoef += coef;
            map.insert(s, t);
        }

        QMap<QString, double> perSubjectAvg;
        double globalWeightedSum = 0;
        double globalCoefSum = 0;
        for (auto it = map.begin(); it != map.end(); ++it) {
            double avg = (it.value().sumCoef > 0) ? (it.value().sumWeighted / it.value().sumCoef) : 0.0;
            perSubjectAvg.insert(it.key(), avg);
            globalWeightedSum += it.value().sumWeighted; // sum(val*coef)
            globalCoefSum += it.value().sumCoef;         // sum(coef)
        }

        double overall = (globalCoefSum > 0) ? (globalWeightedSum / globalCoefSum) : 0.0;
        emit averagesReady(perSubjectAvg, overall);
    }

private slots:
    void onClientResponse(const QString& endpoint, const QJsonDocument& doc) {
        if (!endpoint.endsWith("/grades") && endpoint != "/grades") {
            // ignore other endpoints
            return;
        }
        QJsonArray raw;
        if (doc.isObject()) raw = doc.object().value("data").toArray();
        else if (doc.isArray()) raw = doc.array();

        QJsonArray normalized = normalizeGrades(raw);
        emit gradeListReady(normalized);
        computeAveragesAndEmit(normalized);
    }

    void onClientError(const QString& endpoint, const QString& message) {
        Q_UNUSED(endpoint);
        emit serviceError(message);
    }
};

#include "NotesService.moc"
