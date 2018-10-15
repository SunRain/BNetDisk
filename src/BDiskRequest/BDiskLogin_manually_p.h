#ifndef BDISKLOGIN_MANUALLY_P_H
#define BDISKLOGIN_MANUALLY_P_H

#include <QObject>
#include <QThread>
#include <QWaitCondition>

class QNetworkRequest;
class InnerStateHandler;
class BDiskTokenProvider;
class BDiskCookieJar;
class BDiskLoginManually : public QThread
{
    Q_OBJECT
public:
    explicit BDiskLoginManually(InnerStateHandler *handler, QObject *parent = 0);
    virtual ~BDiskLoginManually();
    QWaitCondition *cond();

    QString userName() const;
    void setUserName(const QString &userName);

    QString passWord() const;
    void setPassWord(const QString &passWord);

    QString captchaText() const;
    void setCaptchaText(const QString &captchaText);

    void refreshCaptchaImgUrl();

    // QThread interface
protected:
    void run();

private:
    void fillRequest(QNetworkRequest *req);
    QString truncateCallback(const QString &callback, const QString &callbackName) const;
    QString truncateYunData(const QString &data) const;
    QString getCodeStringFromPostData(const QByteArray &data) const;
    int getErrorFromPostData(const QByteArray &data) const;

private:
    InnerStateHandler *m_handler;
    BDiskTokenProvider *m_tokenProvider;
    BDiskCookieJar *m_cookieJar;
    QWaitCondition m_wait;

    QString m_userName;
    QString m_passWord;
    QString m_captchaText;
    QString m_uuid;
    QString m_callbackName;
};

#endif // BDISKLOGIN_MANUALLY_P_H
