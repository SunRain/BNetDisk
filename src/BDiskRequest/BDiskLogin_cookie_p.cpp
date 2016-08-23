#include "BDiskLogin_cookie_p.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

#include "BDiskConst.h"
#include "BDiskLogin_p.h"
#include "BDiskTokenProvider.h"

BDiskLoginCookie::BDiskLoginCookie(InnerStateHandler *handler, QObject *parent)
    : QThread(parent)
    , m_handler(handler)
{

}

void BDiskLoginCookie::run()
{
    QNetworkAccessManager networkMgr;
    QNetworkReply *reply;
//    QNetworkRequest request;
    bool requestAborted = false;
    bool finish = false;

#define FREE_REPLY  if (reply) { \
                        if (!reply->isFinished()) { \
                            requestAborted = true; \
                            reply->abort(); \
                        } \
                        disconnect(reply, 0, 0, 0); \
                        reply->deleteLater(); \
                        reply = Q_NULLPTR; \
                    }

#define DO_LOOP_BLOCK   timer.start(); loop.exec();

#define STOP_LOOP_BLOCK if (timer.isActive()) timer.stop(); \
                        if (loop.isRunning()) loop.quit();

    QEventLoop loop;
    QTimer timer;
    timer.setInterval(BDISK_REQUEST_TIMEOUT);
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout,
            [&]() {
        if (reply && !reply->isFinished()) {
            requestAborted = true;
            reply->abort();
        }
        STOP_LOOP_BLOCK;
    });


    // loop to check http header
    QUrl url(BDISK_URL_DISK_HOME);

    do {
        FREE_REPLY;
        requestAborted = false;

        QNetworkRequest req(url);
        req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
        req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

        reply = networkMgr.head(req);
        if (reply) {
            connect(reply, &QNetworkReply::finished, [&](){
                STOP_LOOP_BLOCK;
            });
        } else {
            m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_FAILURE, QString("No http header"));
            finish = true;
            break;
        }
        DO_LOOP_BLOCK;

        if (!reply)
            continue;

        while(!reply->isFinished()) {
            if (currentThread()->isInterruptionRequested()) {
                requestAborted = true;
                break;
            }
            qApp->processEvents();
        }

        if (requestAborted) {
            FREE_REPLY;
            m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_ABORT, QString("Http time out"));
            finish = true;
            break;
        }

        QNetworkReply::NetworkError e = reply->error ();
        bool success = (e == QNetworkReply::NoError);
        if (!success) {
            QString str = reply->errorString();
            FREE_REPLY;
            m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_FAILURE, str);
            finish = true;
            break;
        }
        const QVariant code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        const QUrl newUrl = reply->header(QNetworkRequest::LocationHeader).toUrl();
        FREE_REPLY;
        if (code.isNull() || !code.isValid()) {
            m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_FAILURE, QString("Invalid http code."));
            finish = true;
            break;
        }
        bool ok = false;
        const int ret = code.toInt(&ok);
        if (!ok) {
            m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_FAILURE, QString("Invalid http code to int."));
            finish = true;
            break;
        } else if (ret == 200) {
//            m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_SUCCESS, QString());
            break;
        } else if (ret == 302) { //redirect url
            url = newUrl;
            continue;
        } else {
            m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_FAILURE,
                              QString("Http code [%1] error.").arg(QString::number(ret)));
            finish = true;
            break;
        }
    } while (true);

    if (finish)
        return;

    FREE_REPLY;

    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
    req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

    reply = networkMgr.get(req);
    if (reply) {
        connect(reply, &QNetworkReply::finished, [&](){
            STOP_LOOP_BLOCK;
        });
    } else {
        m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_FAILURE, QString("No http reply"));
        return;
    }
    DO_LOOP_BLOCK;

    while(!reply->isFinished()) {
        if (currentThread()->isInterruptionRequested()) {
            requestAborted = true;
            break;
        }
        qApp->processEvents();
    }

    if (requestAborted) {
        FREE_REPLY;
        m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_ABORT, QString("Http time out"));
        return;
    }

    QNetworkReply::NetworkError e = reply->error ();
    bool success = (e == QNetworkReply::NoError);
    if (!success) {
        QString str = reply->errorString();
        FREE_REPLY;
        m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_FAILURE, str);
        return;
    }
    QByteArray qba = reply->readAll();
    qDebug()<<Q_FUNC_INFO<<" reply data "<<qba;
    FREE_REPLY;
    if (qba.trimmed().isEmpty()) {
//        emit loginByCookieSuccess();
//        return;
        //TODO login success or failure?
    }

    //TODO get bdstoken from response by key yunData.MYBDSTOKEN or FileUtils.bdstoken
//                    if (m_tokenProvider->bdstoken().isEmpty()) {
    QString value = truncateYunData(QString(qba));
    if (value.isEmpty()) {
        qDebug()<<Q_FUNC_INFO<<"Can't get yunData values";
        m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_FAILURE, QString("Can't get yunData values"));
        return;
    }
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson (value.toLocal8Bit(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<Q_FUNC_INFO<<"Parse json error => "<<error.errorString ();
        m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_FAILURE,
                          QString("Parse json error [%1]").arg(error.errorString()));
        return;
    }
    QJsonObject obj = doc.object();
    QString bdstoken = obj.value("bdstoken").toString();
    if (bdstoken.isEmpty()) {
//        emit loginByCookieFailure("Can't get bdstoken");
        m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_FAILURE, QString("Can't get bdstoken"));
        return;
    }
    BDiskTokenProvider *tokenProvider = BDiskTokenProvider::instance();
    tokenProvider->setBdstoken(bdstoken);
    QString uname = obj.value("username").toString();
    //FIXME emit failure if uname is empty;
    tokenProvider->setUidStr(uname);
//                    }
    tokenProvider->flush();
    m_handler->dispatch(InnerEvent::EVENT_COOKIE_LOGIN_SUCCESS, QString());
}

QString BDiskLoginCookie::truncateYunData(const QString &data)
{
    if (data.isEmpty())
        return QString();
    QString str = data.simplified();

//    qDebug()<<Q_FUNC_INFO<<">>>>>>>>>>>>>>> simplified vale "<<str;

    //TODO, check for key yunData.MYBDSTOKEN and FileUtils.bdstoken

    int start = str.indexOf("context=");
    if (start <0)
        return QString();

    int lNum = 0; //number of "{"
    int rNum = 0; //number of "}"
    int end = -1;
    for (int i=start; i<str.length(); ++i) {
        if (QString(str.at(i)) == "{")
            lNum++;
        if (QString(str.at(i+1)) == "}")
            rNum++;

        if ((lNum == rNum) && (lNum != 0) && (rNum != 0)) {
            end = i+2;
            break;
        }
    }
//    qDebug()<<Q_FUNC_INFO<<"lnum "<<lNum<<" rnum "<<rNum<<" end "<<end;
    if (lNum == 0 || rNum == 0 || end <= 0)
        return QString();

    start += 8; //length of "context="
    if (start >= end)
        return QString();
    return str.mid(start, end - start);
}















