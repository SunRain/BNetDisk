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
#include "BDiskShareDelegate.h"
#include "BDiskFileOperationDelegate.h"
#include "BDiskSearchDelegate.h"

#include "ApplicationUtility.h"
#include "BDiskEvent.h"

#include "DLRequest.h"
#include "DLTransmissionDatabaseKeys.h"

const static char *PLUGIN_URI = "com.sunrain.bnetdisk.qmlplugin";

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(new QGuiApplication(argc, argv));
    app.data()->setOrganizationName("SunRain");
    app.data()->setApplicationName("BNetDisk");

//    BDiskLogin login;
    QScopedPointer<BDiskLogin> login(new BDiskLogin());
    QScopedPointer<ApplicationUtility> appUtility(new ApplicationUtility());

    qmlRegisterType<BDiskDirListDelegate>(PLUGIN_URI, 1, 0, "DirListDelegate");
    qmlRegisterType<BDiskDownloadDelegate>(PLUGIN_URI, 1, 0, "BDiskDownloadDelegate");
    qmlRegisterType<BDiskShareDelegate>(PLUGIN_URI, 1, 0, "BDiskShareDelegate");
    qmlRegisterType<BDiskFileOperationDelegate>(PLUGIN_URI, 1, 0, "BDiskFileOperationDelegate");
    qmlRegisterType<BDiskSearchDelegate>(PLUGIN_URI, 1, 0, "BDiskSearchDelegate");

    qmlRegisterSingletonType<BDiskFileObjectKeyName>(PLUGIN_URI,
                                                     1, 0,
                                                     "FileObjectKey",
                                                     BDiskFileObjectKeyName::qmlSingleton);
    qmlRegisterSingletonType<YADownloader::DLTransmissionDatabaseKeysName>(
                PLUGIN_URI,
                1, 0,
                "DownloaderObjectKey",
                YADownloader::DLTransmissionDatabaseKeysName::qmlSingleton);


    BDiskEvent *be = BDiskEvent::instance();
    /// QObject singleton type instances are constructed and owned by the QQmlEngine,
    /// and will be destroyed when the engine is destroyed.
//    qmlRegisterSingletonType<BDiskEvent>(PLUGIN_URI,
//                                         1, 0, "BDiskEvent", BDiskEvent::qmlSingleton);
    qmlRegisterUncreatableType<BDiskEvent>(PLUGIN_URI, 1, 0, "BDiskEvent", "UncreatableType BDiskEvent");

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:///");

    QQmlContext *ctx = engine.rootContext ();
    ctx->setContextProperty ("LoginProvider", login.data());
    ctx->setContextProperty ("AppUtility", appUtility.data ());
    ctx->setContextProperty ("DiskEvent", be);

    QObject::connect(login.data(), &BDiskLogin::loginSuccess, [&](){
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    });
    QObject::connect(login.data(), &BDiskLogin::loginByCookieSuccess, [&](){
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    });
    QObject::connect(login.data(), &BDiskLogin::logouted, [&]() {
        QQmlComponent cmp(&engine, QUrl(QStringLiteral("qrc:/Login/main.qml")));
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
    engine.load(QUrl(QStringLiteral("qrc:/Login/main.qml")));

    return app.data()->exec();
}
