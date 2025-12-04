#ifndef PAWNOTECLIENT_H
#define PAWNOTECLIENT_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QJsonDocument>

class PawnoteClient : public QObject {
    Q_OBJECT

public:
    explicit PawnoteClient(QObject* parent = nullptr, const QString& baseUrl = "http://127.0.0.1:5000");
    ~PawnoteClient() = default;

    void fetchGrades();
    void fetchHomeworks();
    void fetchTimetable();
    void fetchMenu();

signals:
    void gotResponse(const QString& endpoint, const QJsonDocument& doc);
    void errorOccurred(const QString& endpoint, const QString& message);

private slots:
    void onReplyFinished();

private:
    void doGet(const QString& path);
    QNetworkAccessManager* m_mgr;
    QString m_baseUrl;
};

#endif // PAWNOTECLIENT_H