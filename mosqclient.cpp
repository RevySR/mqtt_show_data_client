#include "mosqclient.hpp"
#include <iostream>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <string>
#include "mosqclientutils.hpp"
#include <QJsonDocument>

QMutex MosqClient::mutex;
MosqClient* MosqClient::instance = nullptr;
MosqClient* MosqClient::getInstance() {
    QMutexLocker locker(&mutex);
    if (instance == nullptr)
        instance = new MosqClient("ClientCenter");
    return MosqClient::instance;
}

MosqClient::MosqClient(QString const& id) :
    id{id}
{
    mosquitto_lib_init(); // Mandatory initialization for mosquitto library
    mosq = mosquitto_new(id.toStdString().c_str(), true, nullptr);
    this->keepalive = 60;

    // topic
    // OnlineNode
    // UpdateNode
    // SensorData

    // ControlData

    mosquitto_connect_async(mosq, HOST, PORT,keepalive);     // non blocking connection to broker request

    mosquitto_subscribe(mosq, nullptr, "OnlineNode", QOS_2);
    mosquitto_subscribe(mosq, nullptr, "UpdateNode", QOS_2);
    mosquitto_subscribe(mosq, nullptr, "SensorData", QOS_2);

    //mosquitto_subscribe(mosq, nullptr, "ControlData", QOS_2);

    auto on_connect = [](mosquitto *mosq, void *obj, int result) -> void {
        Q_UNUSED(mosq)
        Q_UNUSED(obj)
        std::cout << ">> MosqClient - connection(" << result << ")" << std::endl;
    };
    mosquitto_connect_callback_set(mosq, on_connect);

    auto on_disconnect = [](mosquitto *mosq, void *obj, int result) -> void {
        Q_UNUSED(mosq)
        Q_UNUSED(obj)
        std::cout << ">> MosqClient - disconnection(" << result << ")" << std::endl;
    };

    mosquitto_disconnect_callback_set(mosq, on_disconnect);

    auto message_callback = [](mosquitto *mosq, void *obj, mosquitto_message const *message) -> void {
        Q_UNUSED(mosq)
        Q_UNUSED(obj)
        auto dataType = QString{message->topic};
        QLatin1String data(static_cast<char*>(message->payload), message->payloadlen);
        qDebug() << "data context: " << data;
        auto util = MosqClientUtils::getInstance();

        QJsonDocument dataJson = QJsonDocument::fromJson(
                    QByteArray::fromRawData(
                        static_cast<char*>(message->payload),
                        message->payloadlen)
                    );
        qDebug() << dataJson;
        qDebug() << dataJson["nodeId"];
        if (dataType == "OnlineNode") {
            util->helperDealWithOnlineNode(dataJson);
        } else if (dataType == "UpdateNode") {
            util->helperDealWithUpdateNode(dataJson);
        } else if (dataType == "SensorData") {
            util->helperDealWithSensorData(dataJson);
        } else {
            qDebug() << "Unknown dataType: " << dataType;
        }
    };

    mosquitto_message_callback_set(mosq, message_callback);
    mosquitto_loop_start(mosq); // Start thread managing connection / publish / subscribe
};

MosqClient::~MosqClient() {
    mosquitto_unsubscribe(mosq, nullptr, "OnlineNode");
    mosquitto_unsubscribe(mosq, nullptr, "UpdateNode");
    mosquitto_unsubscribe(mosq, nullptr, "SensorData");
    mosquitto_loop_start(mosq);            // Kill the thread
    mosq = nullptr;
    mosquitto_lib_cleanup();    // Mosquitto library cleanup
}

bool MosqClient::send_message(QString const& topic, QString const& message)
{
    // Send message - depending on QoS, mosquitto lib managed re-submission this the thread
    //
    // * nullptr : Message Id (int *) this allow to latter get status of each message
    // * topic : topic to be used
    // * lenght of the message
    // * message
    // * qos (0,1,2)
    // * retain (boolean) - indicates if message is retained on broker or not
    // Should return MOSQ_ERR_SUCCESS
    int ret  = mosquitto_publish(
                mosq,
                nullptr,
                topic.toStdString().c_str(),
                static_cast<int>(message.length()),
                message.toStdString().c_str(),
                QOS_2,
                false
                );

    return ( ret == MOSQ_ERR_SUCCESS );
}
