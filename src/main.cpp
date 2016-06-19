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
#include "ApplicationUtility.h"

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
    qmlRegisterSingletonType<BDiskFileObjectKeyName>("com.sunrain.bnetdisk.qmlplugin",
                                                     1, 0, "FileObjectKey", BDiskFileObjectKeyName::qmlSingleton);


    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:///");

    QObject::connect(login.data(), &BDiskLogin::loginSuccess,
                     [&](){
        qDebug()<<Q_FUNC_INFO<<">>>>>>>> loginSuccess";

//        BDiskOpListDir op;
//        BDiskHttpRequest *req = new BDiskHttpRequest(&app);
////        BDiskBaseOperationRequest &b = dynamic_cast<BDiskBaseOperationRequest&>(op);
//        req->request(dynamic_cast<BDiskBaseOperationRequest&>(op));
//        req->request(b);
//        BDiskDirListDelegate *a = new BDiskDirListDelegate(&app);
//        a->showRoot();

//        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
        engine.load(QUrl(QStringLiteral("main.qml")));
    });
    QObject::connect(login.data(), &BDiskLogin::loginByCookieSuccess,
                     [&](){
        engine.load(QUrl(QStringLiteral("main.qml")));
    });

    QQmlContext *ctx = engine.rootContext ();
    ctx->setContextProperty ("LoginProvider", login.data());
    ctx->setContextProperty ("AppUtility", appUtility.data ());

//    engine.load(QUrl(QStringLiteral("qrc:/Login/main.qml")));
    engine.load(QUrl(QStringLiteral("Login/main.qml")));

    return app.data()->exec();
}
