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
    , m_cookieJar(new BDiskCookieJar(this))
    , m_requestAborted(true)
    , m_operationInitiated(false)
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
    m_networkMgr->setCookieJar(m_cookieJar);
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
//    BDiskBaseOperationRequest op = operation();
//    foreach (QString key, m_parameters.keys()) {
//        m_operation.setParameters(key, m_parameters.value(key));
//    }
    if (!m_operationInitiated) {
        m_operation = operation();
        m_operationInitiated = true;
    }
    m_operation.setParameters("bdstoken", BDiskTokenProvider::instance()->bdstoken());
//    m_operation.setParameters("logid", );
    QUrl url = m_operation.initUrl();

    qDebug()<<Q_FUNC_INFO<<" url is "<<url;

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    m_cookieJar->reload();
    QList<QNetworkCookie> cookies = m_cookieJar->cookieList();//BDiskCookieJar::instance()->cookieList();
    QStringList list;
    foreach (QNetworkCookie c, cookies) {
        list.append(QString("%1=%2").arg(QString(c.name())).arg(QString(c.value())));
    }
//    qDebug()<<Q_FUNC_INFO<<"append cookies "<<list;
    request.setRawHeader("Cookie", list.join(";").toUtf8());

    if (m_reply) {
        m_requestAborted = true;
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    if (m_operation.operationType() == BDiskBaseOperationRequest::OperationType::OPERATION_POST) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QByteArray data;
        foreach (QString k, m_operation.postDataParameters().keys()) {
            data.append(QString("%1=%2&").arg(k).arg(m_operation.postDataParameter(k)));
        }
        if (data.endsWith("&"))
            data = data.left(data.length() - 1);
        if (data.isEmpty())
            data = url.query(QUrl::FullyEncoded).toUtf8();
        request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

        qDebug()<<Q_FUNC_INFO<<" post data ["<<data<<"] to ["<<url<<"]";

        m_reply = m_networkMgr->post(request, data);
    } else if (m_operation.operationType() == BDiskBaseOperationRequest::OperationType::OPERATION_GET) {
        m_reply = m_networkMgr->get(request);
    } else {
        if (m_reply) {
            m_requestAborted = true;
            m_reply->abort();
            m_reply->deleteLater();
        }
        m_reply = nullptr;
    }
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

//void BDiskBaseRequest::setParameters(const QString &key, const QString &value)
//{
//    m_parameters.insert(key, value);
//}

BDiskBaseOperationRequest *BDiskBaseRequest::operationPtr()
{
    if (!m_operationInitiated) {
        m_operation = operation();
        m_operationInitiated = true;
    }
    return &m_operation;
}

bool BDiskBaseRequest::isFinished()
{
    return m_reply && m_reply->isFinished();
}

void BDiskBaseRequest::abort()
{
    if (m_reply)
        m_reply->abort();
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
