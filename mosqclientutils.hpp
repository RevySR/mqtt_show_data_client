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
#include <QSet>
#include "mostype.h"

class MosqClientUtils : public QObject
{
    Q_OBJECT
public:
    static MosqClientUtils* getInstance();
    void helperDealWithOnlineNode(QJsonDocument const& json);
    void helperDealWithUpdateNode(QJsonDocument const& json);
    void helperDealWithSensorData(QJsonDocument const& json);
    void helperUpdateRoomId(int roomId);
    void deleteRoomId(int roomId);
    QSet<int> selectRoomId();
    int getNextRoomId();
    QSet<QString> selectNodeNotInRoom(int roomId);
    QSet<QString> selectNodeInRoom(int roomId);
    void updateNodeRoomId(int roomId, QStringList const& list);
    QVector<Sensor> selectSensorFromRoomId(int roomId);
    QVector<Controller> selectControllerFromRoomId(int roomId);
    void updateControllerValue(Controller const& controller);

private:
    static MosqClientUtils* instance;
    static QMutex mutex;
    explicit MosqClientUtils(QObject *parent = nullptr);
    QSqlDatabase database;
signals:

public slots:
};

#endif // MOSQCLIENTUTILS_HPP
