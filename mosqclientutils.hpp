#ifndef MOSQCLIENTUTILS_HPP
#define MOSQCLIENTUTILS_HPP

#include <QObject>
#include <QSql>
#include <QMutex>
#include <QMutexLocker>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
class MosqClientUtils : public QObject
{
    Q_OBJECT
public:
    static MosqClientUtils* getInstance();
    void helperDealWithOnlineNode(QJsonDocument const& json);
    void helperDealWithUpdateNode(QJsonDocument const& json);
    void helperDealWithSensorData(QJsonDocument const& json);
private:
    static MosqClientUtils* instance;
    static QMutex mutex;
    explicit MosqClientUtils(QObject *parent = nullptr);
    QSqlDatabase database;
signals:

public slots:
};

#endif // MOSQCLIENTUTILS_HPP
