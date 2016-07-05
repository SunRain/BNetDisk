#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#include <QDebug>

#include "BDiskRequest/BDiskLogin.h"
#include "BDiskRequest/BDiskOperationRequest.h"
#include "BDiskRequest/BDiskHttpRequest.h"
#include "BDiskRequest/BDiskFileObjectKeys.h"

#include "BDiskDirListDelegate.h"
#include "BDiskDownloadDelegate.h"
#include "ApplicationUtility.h"
#include "BDiskEvent.h"

#include "DLRequest.h"
#include "DLTransmissionDatabaseKeys.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(new QGuiApplication(argc, argv));
    app.data()->setOrganizationName("SunRain");
    app.data()->setApplicationName("BNetDisk");

//    BDiskLogin login;
    QScopedPointer<BDiskLogin> login(new BDiskLogin());
    QScopedPointer<ApplicationUtility> appUtility(new ApplicationUtility());

    qmlRegisterType<BDiskDirListDelegate>("com.sunrain.bnetdisk.qmlplugin",
                                          1, 0, "DirListDelegate");
    qmlRegisterType<BDiskDownloadDelegate>("com.sunrain.bnetdisk.qmlplugin",
                                           1, 0, "BDiskDownloadDelegate");

    qmlRegisterSingletonType<BDiskFileObjectKeyName>("com.sunrain.bnetdisk.qmlplugin",
                                                     1, 0, "FileObjectKey", BDiskFileObjectKeyName::qmlSingleton);
    qmlRegisterSingletonType<YADownloader::DLTransmissionDatabaseKeysName>
            ("com.sunrain.bnetdisk.qmlplugin",
             1, 0,
             "DownloaderObjectKey",
             YADownloader::DLTransmissionDatabaseKeysName::qmlSingleton);


    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:///");

    QQmlContext *ctx = engine.rootContext ();
    ctx->setContextProperty ("LoginProvider", login.data());
    ctx->setContextProperty ("AppUtility", appUtility.data ());
    ctx->setContextProperty ("BDiskEvent", BDiskEvent::instance());

    QObject::connect(login.data(), &BDiskLogin::loginSuccess, [&](){
        engine.load(QUrl(QStringLiteral("main.qml")));
    });
    QObject::connect(login.data(), &BDiskLogin::loginByCookieSuccess, [&](){
        engine.load(QUrl(QStringLiteral("main.qml")));
    });
    QObject::connect(login.data(), &BDiskLogin::logouted, [&]() {
        QQmlComponent cmp(&engine, QStringLiteral("Login/main.qml"));
        QObject *obj = cmp.beginCreate(ctx);
        obj->setProperty("st", "ShowLoginView");
        cmp.completeCreate();
    });

//    YADownloader::DLRequest req;
//    req.setRequestUrl(QUrl("http://people.canonical.com/~alextu/tangxi/recovery/recovery.img"));
//    req.setSavePath(qApp->applicationDirPath());
//    req.setSaveName("bbbb.img");
//    req.setPreferThreadCount(1);
//    YADownloader::DownloadMgr *mgr = new YADownloader::DownloadMgr(app.data());
//    YADownloader::DLTask *task = mgr->get(req);
//    task->start();

//    QTimer timer;
//    QObject::connect(&timer, &QTimer::timeout, [&](){
//        task->abort();
////        task->deleteLater();
//    });
//    timer.setSingleShot(true);
//    timer.start(1000*10);


//    engine.load(QUrl(QStringLiteral("qrc:/Login/main.qml")));
    engine.load(QUrl(QStringLiteral("Login/main.qml")));

    return app.data()->exec();
}
