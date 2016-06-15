#ifndef BDISKHTTPREQUEST_H
#define BDISKHTTPREQUEST_H

#include <QObject>
#include <QQueue>
//#include <QList>
#include <QMutex>
#include <QThread>

#include "BDiskOperationRequest.h"

class QTimer;
//class QNetworkAccessManager;
class QNetworkReply;
class QNetworkDiskCache;

class RequestInnerStateHandler;
class BDiskHttpRequest : public QThread
{
    Q_OBJECT
public:
    enum RequestRet {
        RET_ABORT = 0x0,
        RET_SUCCESS,
        RET_FAILURE
    };
    Q_ENUM(RequestRet)

    explicit BDiskHttpRequest(QObject *parent = 0);
    virtual ~BDiskHttpRequest();
    void request(const BDiskBaseOperationRequest &req);

    // QObject interface
public:
    bool event(QEvent *e);

    // QThread interface
protected:
    void run();

signals:
    void requestSuccess(const BDiskBaseOperationRequest &req, const QString &replyData);
    void requestFailure(const BDiskBaseOperationRequest &req, const QString &replyData);
    void requestAbort(const BDiskBaseOperationRequest &req);
    void requestResult(RequestRet ret, const BDiskBaseOperationRequest &req, const QString &replyData);

public slots:


private:
    void freeReply();

private:
//    QNetworkAccessManager *m_networkMgr;
    QNetworkReply *m_reply;
    RequestInnerStateHandler *m_handler;
    QQueue<BDiskBaseOperationRequest> m_requestList;
//    QList<BDiskBaseOperationRequest> m_requestList;
    QMutex m_locker;

    bool m_requestAborted;
    bool m_breakThread;
};

#endif // BDISKHTTPREQUEST_H
