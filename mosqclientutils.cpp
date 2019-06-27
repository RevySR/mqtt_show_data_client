#include "mosqclientutils.hpp"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValueRef>
MosqClientUtils* MosqClientUtils::instance = nullptr;

QMutex MosqClientUtils::mutex;

MosqClientUtils* MosqClientUtils::getInstance() {
    QMutexLocker locker(&mutex);
    if (instance == nullptr)
        instance = new MosqClientUtils;
    return instance;
}

MosqClientUtils::MosqClientUtils(QObject *parent) : QObject(parent)
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("data.db");

    if (!database.open()) {
        qDebug() << "database open failed: " << database.lastError();
    } else {
        qDebug() << "database open successful";
    }
// "create table node(id char(20) primary key, status int)"
// "create table controller(id char(20) primary key, type char(20), data float)"
// "create table sensor(id char(20) primary key, type char(20), data float)"
    QSqlQuery query;
    auto tableNodeCreateSql = "CREATE TABLE IF NOT EXISTS node(id char(20) primary key, status int)";
    if (!query.exec(tableNodeCreateSql)) {
        qDebug() << "Database table Node create failed: " << query.lastError();
    } else {
        qDebug() << "Database table Node create sucessful";
    }

    auto tableControllerCreateSql = "CREATE TABLE IF NOT EXISTS controller(id char(20) primary key, type char(20), data float)";
    if (!query.exec(tableControllerCreateSql)) {
        qDebug() << "Database table controller create failed: " << query.lastError();
    } else {
        qDebug() << "Database table controller create sucessful";
    }

    auto tableSensorCreateSql = "CREATE TABLE IF NOT EXISTS sensor(id char(20) primary key, type char(20), data float)";
    if (!query.exec(tableSensorCreateSql)) {
        qDebug() << "Database table sensor create failed: " << query.lastError();
    } else {
        qDebug() << "Database table sensor create sucessful";
    }
}

void MosqClientUtils::helperDealWithOnlineNode(QJsonDocument const& json){
    auto data = json["nodeId"].toString();
    if (data == "")
        return;
    auto updateTableOnlineNodeSql =
            QString("INSERT OR REPLACE INTO node(id, status) VALUES (\"%1\", 1)").arg(data);

    qDebug() << "updateTableOnlineNodeSql : " << updateTableOnlineNodeSql;
    QSqlQuery query;
    if (!query.exec(updateTableOnlineNodeSql)) {
        qDebug() << "Database table node insert failed: " << query.lastError();
    } else {
        qDebug() << "Database table node insert sucessful";
    }
}

void MosqClientUtils::helperDealWithUpdateNode(QJsonDocument const& json){
    auto nodeId = json["nodeId"].toString();
    auto sensorArray = json["sensor"].toArray();
    auto controllerArray = json["controller"].toArray();
    auto updateTableSensorSql =
            QString("INSERT OR REPLACE INTO sensor(id, type) VALUES (\"%1:%2\", \"%3\"");

    QSqlQuery query;
    for (auto sensor: sensorArray) {
        auto sensorJsonMap = sensor.toObject();
        auto type = sensorJsonMap["type"].toString();
        auto sensorId = sensorJsonMap["sensorId"].toString();
        auto updateTableSensorDataSql =
                updateTableSensorSql.arg(nodeId).arg(sensorId).arg(type);
        qDebug() << "updateTableSensorDataSql: " <<updateTableSensorDataSql;
        if (!query.exec(updateTableSensorDataSql)) {
            qDebug() << "Database table sensor insert failed: " << query.lastError();
        } else {
            qDebug() << "Database table sensor insert sucessful";
        }
    }

    auto updateTableControllerSql =
            QString("INSERT OR REPLACE INTO controller(id, type) VALUES (\"%1:%2\", \"%3\")");

    for (auto controller: controllerArray) {
        auto controllerJsonMap = controller.toObject();
        auto type = controllerJsonMap["type"].toString();
        auto controllerId = controllerJsonMap["controllerId"].toString();
        auto updateTableControllerDataSql =
                updateTableControllerSql.arg(nodeId).arg(controllerId).arg(type);
        qDebug() << "updateTableControllerSql: " <<updateTableControllerSql;
        if (!query.exec(updateTableControllerSql)) {
            qDebug() << "Database table controller insert failed: " << query.lastError();
        } else {
            qDebug() << "Database table controller insert sucessful";
        }
    }
}

void MosqClientUtils::helperDealWithSensorData(QJsonDocument const& json){
    auto nodeId = json["nodeId"].toString();
    auto sensorArray = json["sensor"].toArray();
    auto controllerArray = json["controller"].toArray();
    auto updateTableSensorSql =
            QString("INSERT OR REPLACE INTO sensor(id, data) VALUES (\"%1:%2\", \"%3\"");

    QSqlQuery query;
    for (auto sensor: sensorArray) {
        auto sensorJsonMap = sensor.toObject();
        auto data = sensorJsonMap["sensorVal"].toString();
        auto sensorId = sensorJsonMap["sensorId"].toString();
        auto updateTableSensorDataSql =
                updateTableSensorSql.arg(nodeId).arg(sensorId).arg(data);
        qDebug() << "updateTableSensorDataSql: " <<updateTableSensorDataSql;
        if (!query.exec(updateTableSensorDataSql)) {
            qDebug() << "Database table sensor insert failed: " << query.lastError();
        } else {
            qDebug() << "Database table sensor insert sucessful";
        }
    }
}
