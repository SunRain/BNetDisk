#include "SyncNetworkRequest.h"

#include <QTimer>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkCookieJar>

SyncNetworkRequest::SyncNetworkRequest(QObject *parent)
    : QObject(parent),
      m_timeoutInterval(0),
      m_requestAborted(false),
      m_networkMgr(new QNetworkAccessManager(this)),
      m_timer(new QTimer(this)),
      m_eventLoop(new QEventLoop(this)),
      m_reply(Q_NULLPTR),
      m_timeoutCallback(Q_NULLPTR)
{
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, [&](){
        if (m_reply && !m_reply->isFinished()) {
            m_requestAborted = true;
            m_reply->abort();
        }
        if (m_eventLoop->isRunning()) {
            m_eventLoop->quit();
        }
    });
}

SyncNetworkRequest::~SyncNetworkRequest()
{
    if (m_timer->isActive()) {
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = Q_NULLPTR;
    }
    if (m_eventLoop->isRunning()) {
        m_eventLoop->quit();
        m_eventLoop->deleteLater();
        m_eventLoop = Q_NULLPTR;
    }
    m_networkMgr->deleteLater();
    m_networkMgr = Q_NULLPTR;
}

void SyncNetworkRequest::setCookieJar(QNetworkCookieJar *cookieJar)
{
    m_networkMgr->setCookieJar(cookieJar);
}

void SyncNetworkRequest::head(const QNetworkRequest &request, const std::function<void (QNetworkReply *)> &callback)
{
    m_requestAborted = false;
    if (m_timer->isActive()) {
        m_timer->stop();
    }
//    if (m_reply) {
//        QObject::disconnect(m_reply, 0, 0, 0);
//        m_reply = Q_NULLPTR;
//    }
    m_reply = m_networkMgr->head(request);
    if (!m_reply) {
        callback(Q_NULLPTR);
        return;
    }
    connect(m_reply, &QNetworkReply::finished, [&](){
        if (m_timer->isActive()) {
            m_timer->stop();
        }
        if (m_eventLoop->isRunning()) {
            m_eventLoop->quit();
        }
    });
    if (m_timeoutInterval > 0) {
        m_timer->start();
    }
    m_eventLoop->exec();

    if (!m_requestAborted) {
        callback(m_reply);
    } else if (m_timeoutCallback) {
        m_timeoutCallback();
    }
}

void SyncNetworkRequest::get(const QNetworkRequest &request, const std::function<void (QNetworkReply *)> &callback)
{
    m_requestAborted = false;
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    m_reply = m_networkMgr->get(request);
    if (!m_reply) {
        callback(Q_NULLPTR);
        return;
    }
    connect(m_reply, &QNetworkReply::finished, [&](){
        if (m_timer->isActive()) {
            m_timer->stop();
        }
        if (m_eventLoop->isRunning()) {
            m_eventLoop->quit();
        }
    });
    if (m_timeoutInterval > 0) {
        m_timer->start();
    }
    m_eventLoop->exec();

    if (!m_requestAborted) {
        callback(m_reply);
    } else if (m_timeoutCallback) {
        m_timeoutCallback();
    }
}

void SyncNetworkRequest::post(const QNetworkRequest &request, const QByteArray &data, const std::function<void (QNetworkReply *)> &callback)
{
    m_requestAborted = false;
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    m_reply = m_networkMgr->post(request, data);
    if (!m_reply) {
        callback(Q_NULLPTR);
        return;
    }
    connect(m_reply, &QNetworkReply::finished, [&](){
        if (m_timer->isActive()) {
            m_timer->stop();
        }
        if (m_eventLoop->isRunning()) {
            m_eventLoop->quit();
        }
    });
    if (m_timeoutInterval > 0) {
        m_timer->start();
    }
    m_eventLoop->exec();

    if (!m_requestAborted) {
        callback(m_reply);
    } else if (m_timeoutCallback) {
        m_timeoutCallback();
    }
}

void SyncNetworkRequest::setTimeOut(int msec)
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    if (msec > 0) {
        m_timer->setInterval(msec);
    }
}

void SyncNetworkRequest::timeOut(const std::function<void ()> &callback)
{
    m_timeoutCallback = callback;
}





