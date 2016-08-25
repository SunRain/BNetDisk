#ifndef BDISKLOGIN_H
#define BDISKLOGIN_H

#include <QObject>
#include <QUrl>
#include <QThread>
//#include <QMutex>
#include <QWaitCondition>

//class QTimer;
//class QNetworkAccessManager;
//class QNetworkReply;
//class QNetworkRequest;

class BDiskTokenProvider;
class InnerStateHandler;
//class BDiskCookieJar;
class BDiskLoginCookie;
class BDiskLoginManually;
class BDiskLogin : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString passWord READ passWord WRITE setPassWord NOTIFY passWordChanged)
    Q_PROPERTY(QString captchaText WRITE setCaptchaText NOTIFY captchaTextChanged)
    Q_PROPERTY(QUrl captchaImgUrl READ captchaImgUrl NOTIFY captchaImgUrlChanged)

public:
    explicit BDiskLogin(QObject *parent = 0);

    Q_INVOKABLE void login();
    Q_INVOKABLE void loginByCookie();
    Q_INVOKABLE void logout();

    QString userName() const;
    QString passWord() const;
    QUrl captchaImgUrl() const;

    // QObject interface
public:
    bool event(QEvent *e);

signals:
    void logouted();
    void loginAbort();
    void loginSuccess();
    void loginFailure(const QString &message);
    void loginByCookieSuccess();
    void loginByCookieFailure(const QString &message);
    void userNameChanged(const QString &userName);
    void passWordChanged(const QString &passWord);
    void captchaTextChanged(const QString &captchaText);
    void captchaImgUrlChanged(const QUrl &captchaImgUrl);

public slots:
    void setUserName(const QString &userName);
    void setPassWord(const QString &passWord);
    void setCaptchaText(const QString &captchaText);

private:
    BDiskTokenProvider *m_tokenProvider;
    InnerStateHandler *m_handler;
    BDiskLoginCookie *m_cookieLogin;
    BDiskLoginManually *m_manuallyLogin;
    QString m_userName;
    QString m_passWord;
    QString m_captchaText;
    QUrl m_captchaImgUrl;
};

#endif // BDISKLOGIN_H
