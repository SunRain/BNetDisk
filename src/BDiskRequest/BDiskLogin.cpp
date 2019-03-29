#include "BDiskLogin.h"

#include <QTimer>
#include <QDebug>
#include <QEvent>
#include <QObject>
#include <QCoreApplication>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>

#include <QEventLoop>

#include <QScopedPointer>

#include "BDiskConst.h"
#include "BDiskTokenProvider.h"
#include "BDiskCookieJar.h"
#include "BDiskLogin_p.h"
#include "BDiskLogin_cookie_p.h"
#include "BDiskLogin_manually_p.h"

BDiskLogin::BDiskLogin(QObject *parent)
    : QObject(parent)
    , m_tokenProvider(BDiskTokenProvider::instance())
    , m_handler(new InnerStateHandler(this))
    , m_cookieLogin(Q_NULLPTR)
    , m_manuallyLogin(Q_NULLPTR)
    , m_userName(QString())
    , m_passWord(QString())
    , m_captchaText(QString())
    , m_captchaImgUrl(QUrl())
{

}

void BDiskLogin::login()
{
    if (!m_manuallyLogin) {
        m_manuallyLogin = new BDiskLoginManually(m_handler);
    }
    m_manuallyLogin->setUserName(m_userName);
    m_manuallyLogin->setPassWord(m_passWord);
    m_manuallyLogin->setCaptchaText(m_captchaText);

    if (m_manuallyLogin->isRunning()) {
        m_manuallyLogin->cond()->wakeAll();
    } else {
        m_manuallyLogin->start();
    }
}

void BDiskLogin::loginByCookie()
{
    if (!m_cookieLogin) {
        m_cookieLogin = new BDiskLoginCookie(m_handler);
    }
    if (m_cookieLogin->isRunning()) {
        m_cookieLogin->requestInterruption();
        m_cookieLogin->quit();
        m_cookieLogin->wait();
    }
    m_cookieLogin->start();
}

void BDiskLogin::logout()
{
    BDiskCookieJar::instance()->clear();
    m_tokenProvider->clear();
    emit logouted();
}

void BDiskLogin::refreshCaptchaImgUrl()
{
    if (!m_manuallyLogin) {
        qWarning()<<Q_FUNC_INFO<<"This function only works on manually login";
        return;
    }
    m_manuallyLogin->refreshCaptchaImgUrl();
}

QString BDiskLogin::userName() const
{
    return m_userName;
}

QString BDiskLogin::passWord() const
{
    return m_passWord;
}

QUrl BDiskLogin::captchaImgUrl() const
{
    return m_captchaImgUrl;
}

bool BDiskLogin::event(QEvent *e)
{
    if (e->type() == INNER_EVENT) {
        InnerEvent *event = (InnerEvent*)e;
        const InnerEvent::EventState state =  event->state();
        if (state == InnerEvent::EVENT_LOGIN_ABORT) {
            emit loginAbort();
        }
        if (state == InnerEvent::EVENT_LOGIN_FAILURE) {
            if (m_manuallyLogin) {
                if (m_manuallyLogin->isRunning()) {
                    m_manuallyLogin->cond()->wakeAll();
                    m_manuallyLogin->quit();
                    m_manuallyLogin->wait();
                }
                m_manuallyLogin->deleteLater();
                m_manuallyLogin = Q_NULLPTR;
            }
            emit loginFailure(event->data());
        }
        if (state == InnerEvent::EVENT_LOGIN_SUCCESS) {
            if (m_manuallyLogin) {
                if (m_manuallyLogin->isRunning()) {
                    m_manuallyLogin->cond()->wakeAll();
                    m_manuallyLogin->quit();
                    m_manuallyLogin->wait();
                }
                m_manuallyLogin->deleteLater();
                m_manuallyLogin = Q_NULLPTR;
            }
            emit loginSuccess();
        }
        if (state == InnerEvent::EVENT_CAPTCHA_URL) {
            m_captchaImgUrl = event->url();
            emit captchaImgUrlChanged(m_captchaImgUrl);
        }
//        if (state == InnerEvent::EVENT_CAPTCHA_TEXT) {
//            emit captchaTextChanged(m_captchaText);
//        }
        if (state == InnerEvent::EVENT_CAPTCHA_URL_NEED_REFRESH) {
            qDebug()<<Q_FUNC_INFO<<"+++++ re-start thread";
            if (m_manuallyLogin->isRunning()) {
                m_manuallyLogin->cond()->wakeAll();
                m_manuallyLogin->quit();
                m_manuallyLogin->wait();
            }
            m_manuallyLogin->start();
        }
        if (state == InnerEvent::EVENT_COOKIE_LOGIN_ABORT || state == InnerEvent::EVENT_COOKIE_LOGIN_FAILURE) {
            if (m_cookieLogin) {
                if (m_cookieLogin->isRunning()) {
                    m_cookieLogin->requestInterruption();
                    m_cookieLogin->quit();
                    m_cookieLogin->wait();
                }
                m_cookieLogin->deleteLater();
                m_cookieLogin = Q_NULLPTR;
            }
            emit loginByCookieFailure(event->data());
        }
        if (state == InnerEvent::EVENT_COOKIE_LOGIN_SUCCESS) {
            if (m_cookieLogin) {
                if (m_cookieLogin->isRunning()) {
                    m_cookieLogin->requestInterruption();
                    m_cookieLogin->quit();
                    m_cookieLogin->wait();
                }
                m_cookieLogin->deleteLater();
                m_cookieLogin = Q_NULLPTR;
            }
            emit loginByCookieSuccess();
        }
        return true;
    }
    return QObject::event(e);
}

void BDiskLogin::setUserName(const QString &userName)
{
    if (m_userName == userName)
        return;

    m_userName = userName;
    emit userNameChanged(userName);
}

void BDiskLogin::setPassWord(const QString &passWord)
{
    if (m_passWord == passWord)
        return;

    m_passWord = passWord;
    emit passWordChanged(passWord);
}

void BDiskLogin::setCaptchaText(const QString &captchaText)
{
    if (m_captchaText == captchaText)
        return;

    m_captchaText = captchaText;
    emit captchaTextChanged(captchaText);
}
