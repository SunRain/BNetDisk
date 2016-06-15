#include "BDiskHttpRequest.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrlQuery>

#include <QEventLoop>
#include <QDebug>
#include <QEvent>
#include <QCoreApplication>

#include "BDiskConst.h"
#include "BDiskOperationRequest.h"
#include "BDiskCookieJar.h"
#include "BDiskTokenProvider.h"

#define INNER_EVENT (QEvent::Type(QEvent::User))

class RequestInnerEvent : public QEvent
{
public:
    enum EventState {
        EVENT_REQ_ABORT = 0x0,
        EVENT_REQ_SUCCESS,
        EVENT_REQ_FAILURE,
    };
public:
    RequestInnerEvent(RequestInnerEvent::EventState ret, const BDiskBaseOperationRequest &req, const QString &value)
        : QEvent(INNER_EVENT)
    {
        m_ret = ret;
        m_req = req;
        m_data = value;
    }
    virtual ~RequestInnerEvent() {}

    EventState state() const {
        return m_ret;
    }
    QString data() const {
        return m_data;
    }
    BDiskBaseOperationRequest req() const {
        return m_req;
    }
private:
    EventState m_ret;
    QString m_data;
    BDiskBaseOperationRequest m_req;
};

class RequestInnerStateHandler : public QObject
{
    Q_OBJECT
public:
    RequestInnerStateHandler(QObject *parent = 0)
        : QObject(parent)
        , m_mutex(QMutex::Recursive)
    {
    }
    virtual ~RequestInnerStateHandler() {}

    void dispatch(RequestInnerEvent::EventState ret, const BDiskBaseOperationRequest &req, const QString &data = QString()) {
        m_mutex.lock();
        qApp->postEvent(parent(), new RequestInnerEvent(ret, req, data));
        m_mutex.unlock();
    }
private:
    QMutex m_mutex;
};

BDiskHttpRequest::BDiskHttpRequest(QObject *parent)
    : QThread(parent)
    , m_reply(nullptr)
    , m_handler(new RequestInnerStateHandler(this))
    , m_requestAborted(false)
    , m_breakThread(false)
{

}

BDiskHttpRequest::~BDiskHttpRequest()
{
}

void BDiskHttpRequest::request(const BDiskBaseOperationRequest &req)
{
    m_locker.lock();
    m_requestList.enqueue(req);
    if (!this->isRunning())
        this->start();
    m_locker.unlock();
}

bool BDiskHttpRequest::event(QEvent *e)
{
    if (e->type() == INNER_EVENT) {
        RequestInnerEvent *event = (RequestInnerEvent*)e;
        RequestInnerEvent::EventState state =  event->state();
        BDiskBaseOperationRequest op = event->req();
        RequestRet ret;
        if (state == RequestInnerEvent::EVENT_REQ_ABORT) {
            ret = RequestRet::RET_ABORT;
            emit requestAbort(op);
        }
        if (state == RequestInnerEvent::EVENT_REQ_FAILURE) {
            ret = RequestRet::RET_FAILURE;
            emit requestFailure(op, event->data());
        }
        if (state == RequestInnerEvent::EVENT_REQ_SUCCESS) {
            ret = RequestRet::RET_SUCCESS;
            emit requestSuccess(op, event->data());
        }
        emit requestResult(ret, op, event->data());

        return true;
    }
    return QObject::event(e);
}

void BDiskHttpRequest::run()
{
    qDebug()<<Q_FUNC_INFO<<"===============================";

    QNetworkAccessManager *networkMgr = new QNetworkAccessManager();
    networkMgr->setCookieJar(BDiskCookieJar::instance());

    QEventLoop loop;
    QTimer timer;
    timer.setInterval(BDISK_REQUEST_TIMEOUT);
    timer.setSingleShot(true);

    connect(&timer, &QTimer::timeout,
            [&]() {
        qDebug()<<Q_FUNC_INFO<<">>>>>> QTimer timeout";
        freeReply();
        if (timer.isActive())
            timer.stop();
        if (loop.isRunning())
            loop.quit();
    });

    do {
        m_locker.lock();
        if (m_requestList.isEmpty()) {
            if (timer.isActive())
                timer.stop();
            if (loop.isRunning())
                loop.quit();
            m_locker.unlock();
            break;
        }
        BDiskBaseOperationRequest op = m_requestList.dequeue();
        m_locker.unlock();

        freeReply();
        m_requestAborted = false;

        QUrl url = op.initUrl();

        qDebug()<<Q_FUNC_INFO<<" url is "<<url;

        QNetworkRequest request(url);
        request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
        request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

        //TODO post operation
        m_reply = networkMgr->get(request);
        if (m_reply) {
            connect(m_reply, &QNetworkReply::finished,
                    [&](){
                if (m_requestAborted) {
                    m_requestAborted = false;
                    m_handler->dispatch(RequestInnerEvent::EVENT_REQ_ABORT, op);
                    m_breakThread = true;
                    freeReply();
                    if (timer.isActive()) timer.stop();
                    if (loop.isRunning()) loop.quit();
                    return;
                }
                QNetworkReply::NetworkError e = m_reply->error ();
                bool success = (e == QNetworkReply::NoError);
                if (!success) {
                    qDebug()<<Q_FUNC_INFO<<"request failure "<<m_reply->errorString();
                    m_handler->dispatch(RequestInnerEvent::EVENT_REQ_FAILURE, op, m_reply->errorString());
                    m_breakThread = true;
                    freeReply();
                    if (timer.isActive()) timer.stop();
                    if (loop.isRunning()) loop.quit();
                    return;
                }
                QByteArray qba = m_reply->readAll();
                qDebug()<<Q_FUNC_INFO<<" reply ok ";//<<qba;
                m_handler->dispatch(RequestInnerEvent::EVENT_REQ_SUCCESS, op, QString(qba));
                freeReply();
                if (timer.isActive()) timer.stop();
                if (loop.isRunning()) loop.quit();
            });
            timer.start();
            loop.exec();
        }
        if (m_breakThread)
            break;
    } while (true);

    freeReply();
    m_requestAborted = false;
    m_breakThread = false;
    networkMgr->deleteLater();
    networkMgr = nullptr;
    qDebug()<<Q_FUNC_INFO<<"============ finish";
}

void BDiskHttpRequest::freeReply()
{
    if (m_reply) {
        if (!m_reply->isFinished()) {
            m_requestAborted = true;
            m_reply->abort();
        }
        QObject::disconnect(m_reply, 0, 0, 0);
        m_reply->deleteLater();
        m_reply = nullptr;
    }
}


#include "BDiskHttpRequest.moc"
