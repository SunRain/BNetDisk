#ifndef SYNCNETWORKREQUEST_H
#define SYNCNETWORKREQUEST_H

#include<functional>

#include <QObject>

class QTimer;
class QEventLoop;
class QNetworkReply;
class QNetworkRequest;
class QNetworkCookieJar;
class QNetworkAccessManager;
class SyncNetworkRequest : public QObject
{
    Q_OBJECT
public:
    explicit SyncNetworkRequest(QObject *parent = 0);
    virtual ~SyncNetworkRequest();

    ///
    /// \brief setCookieJar
    /// \param cookieJar
    /// Note: This function manage ownership of cookieJar object same as QNetworkAccessManager.
    ///
    void setCookieJar(QNetworkCookieJar *cookieJar);

    ///
    /// \brief head
    /// Start sync network request.
    /// This will block eventloop until network finished or time out.
    /// \param request
    /// \param callback
    /// The reply parameter will contain a pointer to the reply that has just finished.
    /// NOTE Use deleteLater() to delete reply, reply may nullptr
    ///
    void head(const QNetworkRequest &request, const std::function<void(QNetworkReply *reply)> &callback);

    ///
    /// \brief get
    /// Start sync network request.
    /// This will block eventloop until network finished or time out.
    /// \param request
    /// \param callback
    /// The reply parameter will contain a pointer to the reply that has just finished.
    /// NOTE Use deleteLater() to delete reply, reply may nullptr
    ///
    void get(const QNetworkRequest &request, const std::function<void(QNetworkReply *reply)> &callback);

    ///
    /// \brief post
    /// Start sync network request.
    /// This will block eventloop until network finished or time out.
    /// \param request
    /// \param data
    /// \param callback
    /// The reply parameter will contain a pointer to the reply that has just finished.
    /// NOTE Use deleteLater() to delete reply, reply may nullptr
    ///
    void post(const QNetworkRequest &request, const QByteArray &data, const std::function<void(QNetworkReply *reply)> &callback);

    ///
    /// \brief setTimeOut
    /// \param msec milliseconds of time to abort request, 0 means disable timeout function.
    ///
    void setTimeOut(int msec = 0);

    void timeOut(const std::function<void()> &callback);

private:
    int                                     m_timeoutInterval;
    bool                                    m_requestAborted;
    QNetworkAccessManager                   *m_networkMgr;
    QTimer                                  *m_timer;
    QEventLoop                              *m_eventLoop;
    QNetworkReply                           *m_reply;
    std::function<void()>                   m_timeoutCallback;
};

#endif // SYNCNETWORKREQUEST_H
