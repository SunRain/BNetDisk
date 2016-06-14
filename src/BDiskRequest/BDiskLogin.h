#ifndef BDISKLOGIN_H
#define BDISKLOGIN_H

#include <QObject>
#include <QUrl>
#include <QThread>
//#include <QMutex>
#include <QWaitCondition>

class QTimer;
class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;

class BDiskTokenProvider;
class InnerStateHandler;
class BDiskCookieJar;
class BDiskLogin : public QThread
{
    Q_OBJECT

    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString passWord READ passWord WRITE setPassWord NOTIFY passWordChanged)
    Q_PROPERTY(QString captchaText WRITE setCaptchaText NOTIFY captchaTextChanged)
    Q_PROPERTY(QUrl captchaImgUrl READ captchaImgUrl NOTIFY captchaImgUrlChanged)

public:
    explicit BDiskLogin(QObject *parent = 0);

    Q_INVOKABLE void login();

    QString userName() const;
    QString passWord() const;
    QUrl captchaImgUrl() const;

    // QObject interface
public:
    bool event(QEvent *e);

    // QThread interface
protected:
    void run();

signals:
    void loginAbort();
    void loginSuccess();
    void loginFailure(QString message);
    void userNameChanged(QString userName);
    void passWordChanged(QString passWord);
    void captchaTextChanged(QString captchaText);
    void captchaImgUrlChanged(QUrl captchaImgUrl);

public slots:
    void setUserName(QString userName);
    void setPassWord(QString passWord);
    void setCaptchaText(QString captchaText);

private:
    QString truncateCallback(const QString &callback);
    QString truncateYunData(const QString &data);
    int getErrorFromPostData(const QByteArray &data);
    QString getCodeStringFromPostData(const QByteArray &data);
    void reset();
    void freeReply();
    void freeAndStop();
    void fillRequest(QNetworkRequest *req);
private:
    QNetworkReply *m_reply;
    BDiskTokenProvider *m_tokenProvider;
    InnerStateHandler *m_handler;
    BDiskCookieJar *m_cookieJar;
    QString m_userName;
    QString m_passWord;
    QString m_captchaText;
    QUrl m_captchaImgUrl;
//    QMutex m_lock;
    QWaitCondition m_wait;
    bool m_requestAborted;
    bool m_abortLogin;
    bool m_breakThread;
    bool m_loginFailureOrAborted;
    ///
    /// \brief m_loginErrCode used to indicate wether we need to check cookies-login in login thread
    ///
    int m_loginErrCode;
};

#endif // BDISKLOGIN_H
