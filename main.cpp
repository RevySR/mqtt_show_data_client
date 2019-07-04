#include <QApplication>
//#include <QQmlApplicationEngine>

#include <mosqclient.hpp>
#include <QtGlobal>
#include "mainwindow.h"
#include <QObject>

#include <string>
#include <QString>

int main(int argc, char *argv[])
{
//    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QApplication app(argc, argv);

//    QQmlApplicationEngine engine;
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
//    if (engine.rootObjects().isEmpty())
//        return -1;
    MainWindow mainWindow;
    mainWindow.show();
    QFont font("",72);

    auto mosqClient = MosqClient::getInstance();
    mosqClient->send_message("ControlData", "hello233");
    return app.exec();
}
