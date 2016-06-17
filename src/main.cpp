#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QDebug>

#include "BDiskRequest/BDiskLogin.h"
#include "BDiskRequest/BDiskOperationRequest.h"
#include "BDiskRequest/BDiskHttpRequest.h"

#include "BDiskDirListDelegate.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    BDiskLogin login;

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:///");

    QObject::connect(&login, &BDiskLogin::loginSuccess,
                     [&](){
        qDebug()<<Q_FUNC_INFO<<">>>>>>>> loginSuccess";

//        BDiskOpListDir op;
//        BDiskHttpRequest *req = new BDiskHttpRequest(&app);
////        BDiskBaseOperationRequest &b = dynamic_cast<BDiskBaseOperationRequest&>(op);
//        req->request(dynamic_cast<BDiskBaseOperationRequest&>(op));
//        req->request(b);
//        BDiskDirListDelegate *a = new BDiskDirListDelegate(&app);
//        a->showRoot();

        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    });

    QQmlContext *ctx = engine.rootContext ();
    ctx->setContextProperty ("LoginProvider", &login);

    engine.load(QUrl(QStringLiteral("qrc:/Login/main.qml")));

    return app.exec();
}
