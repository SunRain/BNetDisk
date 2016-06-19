#include "BDiskBaseRequest.h"

#include <QTimer>
#include <QUrl>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkCookie>

#include "BDiskConst.h"
#include "BDiskOperationRequest.h"
#include "BDiskTokenProvider.h"
#include "BDiskCookieJar.h"

static QMutex s_lock;
static QScopedPointer<QNetworkAccessManager> s_networkSP;

BDiskBaseRequest::BDiskBaseRequest(QObject *parent)
    : QObject(parent)
    , m_timeout(new QTimer(this))
    , m_reply(nullptr)
    , m_requestAborted(true)
{
    m_timeout->setInterval(BDISK_REQUEST_TIMEOUT);
    m_timeout->setSingleShot(true);
    connect (m_timeout, &QTimer::timeout,
             [&](){
        m_requestAborted = true;
        if (m_reply)
            m_reply->abort();
    });

    m_networkMgr = networkMgr();
//    m_networkMgr->setCookieJar(BDiskCookieJar::instance());
//    if (m_networkMgr) {
//        m_networkMgr->setCookieJar(BDiskCookieJar::instance());
//    }
}

BDiskBaseRequest::~BDiskBaseRequest()
{
    if (m_timeout->isActive()) {
        m_timeout->stop();
        m_timeout->deleteLater();
    }
    m_timeout = nullptr;

    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }
//    QObject::disconnect(m_networkMgr, 0, 0, 0);
    m_networkMgr = nullptr;
//    if (Q_LIKELY(!s_networkSP.isNull())) {
//        s_lock.lock();
//        s_networkSP.reset();
//        s_lock.unlock();
//    }
}

void BDiskBaseRequest::request()
{
    m_requestAborted = false;
    BDiskBaseOperationRequest op = operation();
    foreach (QString key, m_parameters.keys()) {
        op.setParameters(key, m_parameters.value(key));
    }
    QUrl url = op.initUrl();

    qDebug()<<Q_FUNC_INFO<<" url is "<<url;

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    QList<QNetworkCookie> cookies = BDiskCookieJar::instance()->cookieList();
    QStringList list;
    foreach (QNetworkCookie c, cookies) {
        list.append(QString("%1=%2").arg(QString(c.name())).arg(QString(c.value())));
    }
//    qDebug()<<Q_FUNC_INFO<<"append cookies "<<list;
    request.setRawHeader("Cookie", list.join(";").toUtf8());

    if (m_reply) {
        m_requestAborted = true;
        m_reply->abort();
    }
    //TODO post operation
    m_reply = m_networkMgr->get(request);
    if (m_reply) {
        connect(m_reply, &QNetworkReply::finished,
                [&](){
            if (m_timeout->isActive ())
                m_timeout->stop ();

            if (m_requestAborted) {
                m_requestAborted = false;
                m_reply->deleteLater();
                m_reply = nullptr;
                emit requestAbort();
                emit requestResult(BDiskBaseRequest::RET_ABORT, QString());
                return;
            }
//            foreach (QNetworkReply::RawHeaderPair p, m_reply->rawHeaderPairs ()) {
//                qDebug()<<Q_FUNC_INFO<<p.first<<"||"<<p.second;
//            }

            QNetworkReply::NetworkError e = m_reply->error ();
            bool success = (e == QNetworkReply::NoError);
            if (!success) {
                QString str = m_reply->errorString ();
                m_reply->deleteLater ();
                m_reply = nullptr;

                qDebug()<<Q_FUNC_INFO<<"request failure "<<str;
                emit requestFailure(str);
                emit requestResult(BDiskBaseRequest::RET_SUCCESS, str);
                return;
            }
            QByteArray qba = m_reply->readAll ();
            m_reply->deleteLater ();
            m_reply = nullptr;
            emit requestSuccess (QString(qba));
            emit requestResult (BDiskBaseRequest::RET_SUCCESS, QString(qba));
        });
        emit requestStarted();
    }
}

void BDiskBaseRequest::setParameters(const QString &key, const QString &value)
{
    m_parameters.insert(key, value);
}

BDiskBaseOperationRequest BDiskBaseRequest::operation()
{
    qWarning()<<Q_FUNC_INFO<<"Default empty BDiskBaseOperationRequest!!!";
    return BDiskBaseOperationRequest();
}

QNetworkAccessManager *BDiskBaseRequest::networkMgr() {
    if (Q_UNLIKELY(s_networkSP.isNull())) {
        s_lock.lock();
        if (Q_UNLIKELY(s_networkSP.isNull())) {
            s_networkSP.reset(new QNetworkAccessManager);
//            s_networkSP.data()->setCookieJar(BDiskCookieJar::instance());
        }
        s_lock.unlock();
    }
    return s_networkSP.data();
}
