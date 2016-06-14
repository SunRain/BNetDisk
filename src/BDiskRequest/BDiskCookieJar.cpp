#include "BDiskCookieJar.h"

#include <QDebug>
#include <QNetworkCookie>

#define DBG_OUT 1

BDiskCookieJar::BDiskCookieJar(QObject *parent)
    : QNetworkCookieJar(parent)
    , m_locker(QMutex::Recursive)
{

}

QHash<QByteArray, QByteArray> BDiskCookieJar::cookieList() const
{
    return m_cookieList;
}

QList<QNetworkCookie> BDiskCookieJar::cookiesForUrl(const QUrl &url) const
{
#if DBG_OUT
    qDebug()<<Q_FUNC_INFO<<"cookiesForUrl "<<url;
#endif
    QList<QNetworkCookie> cookies = QNetworkCookieJar::cookiesForUrl(url);
#if DBG_OUT
    foreach (QNetworkCookie c, cookies) {
        qDebug()<<Q_FUNC_INFO<<"cookies name="<<c.name ()<<" value="<<c.value ();
    }
#endif
    return cookies;
}

bool BDiskCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
    m_locker.lock();
#if DBG_OUT
    qDebug()<<Q_FUNC_INFO<<"cookiesForUrl "<<url;
#endif

    foreach (QNetworkCookie c, cookieList) {
#if DBG_OUT
        qDebug()<<Q_FUNC_INFO<<"cookies name="<<c.name ()<<" value="<<c.value ();
#endif
        m_cookieList.insert(c.name(), c.value());
    }
    m_locker.unlock();

//    TokenProvider::instance ()->updateHackLoginCookies (cookieList);
    return QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
}

//bool BDiskCookieJar::insertCookie(const QNetworkCookie &cookie)
//{
//#if DBG_OUT
//    qDebug()<<Q_FUNC_INFO<<"cookies name="<<cookie.name ()<<" value="<<cookie.value ();
//#endif
//    return QNetworkCookieJar::insertCookie(cookie);
//}

//bool BDiskCookieJar::updateCookie(const QNetworkCookie &cookie)
//{
//#if DBG_OUT
//    qDebug()<<Q_FUNC_INFO<<"cookies name="<<cookie.name ()<<" value="<<cookie.value ();
//#endif
//    return QNetworkCookieJar::updateCookie(cookie);
//}

//bool BDiskCookieJar::deleteCookie(const QNetworkCookie &cookie)
//{
//#if DBG_OUT
//    qDebug()<<Q_FUNC_INFO<<"cookies name="<<cookie.name ()<<" value="<<cookie.value ();
//#endif
//    return QNetworkCookieJar::deleteCookie(cookie);
//}
