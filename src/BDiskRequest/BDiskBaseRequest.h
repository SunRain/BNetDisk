#ifndef BDISKBASEREQUEST_H
#define BDISKBASEREQUEST_H

#include <QObject>
#include <QMutex>
#include <QScopedPointer>

#include <QNetworkAccessManager>

#include "BDiskOperationRequest.h"

class QTimer;
class QNetworkReply;
class BDiskBaseRequest : public QObject
{
    Q_OBJECT
public:
    enum RequestRet {
        RET_ABORT = 0x0,
        RET_SUCCESS,
        RET_FAILURE
    };
    Q_ENUM(RequestRet)

    explicit BDiskBaseRequest(QObject *parent = 0);
    virtual ~BDiskBaseRequest();

    Q_INVOKABLE void request();
//    Q_INVOKABLE void setParameters(const QString &key, const QString &value);

    BDiskBaseOperationRequest *operationPtr();

    bool isFinished();

    void abort();

protected:
    virtual BDiskBaseOperationRequest operation();

signals:
    void requestStarted();
    void requestSuccess(const QString &replyData);
    void requestFailure(const QString &replyData);
    void requestAbort();
    void requestResult(RequestRet ret, const QString &replyData);

private:
    ///
    /// \brief networkMgr 返回一个静态的 QNetworkAccessManager
    /// 从Qt的文档中可以得知以下的内容
    ///     QNetworkAccessManager queues the requests it receives.
    ///     The number of requests executed in parallel is dependent on the protocol.
    ///     Currently, for the HTTP protocol on desktop platforms,
    ///         6 requests are executed in parallel for one host/port combination.
    /// 对于当前，由于同时请求api动作不可能超过六个，
    /// 所以使用静态的单一QNetworkAccessManager可以避免每次构造api请求的时候都同时构造一个QNetworkAccessManager
    /// 以便节省资源占用
    /// \return
    ///
   static QNetworkAccessManager *networkMgr();

private:
    QTimer *m_timeout;
    QNetworkReply *m_reply;
    QNetworkAccessManager *m_networkMgr;
    BDiskBaseOperationRequest m_operation;
//    QHash<QString, QString> m_parameters;

    bool m_requestAborted;
    bool m_operationInitiated;
};

#endif // BDISKBASEREQUEST_H
