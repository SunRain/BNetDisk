#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QDebug>

#include "BDiskRequest/BDiskLogin.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    BDiskLogin login;
//    login.login();

    QQmlApplicationEngine engine;

    QObject::connect(&login, &BDiskLogin::loginSuccess,
                     [&](){
        qDebug()<<Q_FUNC_INFO<<">>>>>>>> loginSuccess";
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    });

    QQmlContext *ctx = engine.rootContext ();
    ctx->setContextProperty ("LoginProvider", &login);
    engine.load(QUrl(QStringLiteral("qrc:/login.qml")));

    return app.exec();
}
