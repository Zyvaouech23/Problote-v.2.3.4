// services/PawnoteClient.cpp
// Client HTTP minimal pour le service Pawnote (Node.js).
// Fournit des méthodes asynchrones fetchGrades(), fetchHomeworks(), fetchTimetable(), fetchMenu().
// Émet des signaux with QJsonDocument résultats ou erreur.

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class PawnoteClient : public QObject {
    Q_OBJECT
public:
    explicit PawnoteClient(QObject* parent = nullptr, const QString& baseUrl = QStringLiteral("http://127.0.0.1:5000"))
        : QObject(parent), m_baseUrl(baseUrl) {
        m_mgr = new QNetworkAccessManager(this);
    }

    // Requêtes asynchrones
    void fetchGrades() { doGet("/grades"); }
    void fetchHomeworks() { doGet("/homeworks"); }
    void fetchTimetable() { doGet("/timetable"); }
    void fetchMenu() { doGet("/menu"); }

signals:
    void gotResponse(const QString& endpoint, const QJsonDocument& doc);
    void errorOccurred(const QString& endpoint, const QString& message);

private:
    QNetworkAccessManager* m_mgr;
    QString m_baseUrl;

    void doGet(const QString& path) {
        QUrl url(m_baseUrl + path);
        QNetworkRequest req(url);
        // header utile si besoin : req.setHeader(QNetworkRequest::UserAgentHeader, "QtPawnoteClient/1.0");
        QNetworkReply* rep = m_mgr->get(req);
        connect(rep, &QNetworkReply::finished, this, [this, rep, path]() {
            const QString endpoint = path;
            if (rep->error() != QNetworkReply::NoError) {
                QString msg = rep->errorString();
                emit errorOccurred(endpoint, msg);
                rep->deleteLater();
                return;
            }
            QByteArray bytes = rep->readAll();
            rep->deleteLater();
            QJsonParseError perr;
            QJsonDocument doc = QJsonDocument::fromJson(bytes, &perr);
            if (perr.error != QJsonParseError::NoError) {
                emit errorOccurred(endpoint, QStringLiteral("JSON parse error: %1").arg(perr.errorString()));
                return;
            }
            emit gotResponse(endpoint, doc);
            });
    }
};

#include "PawnoteClient.moc"
