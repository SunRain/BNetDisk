#include "BDiskCookieJar.h"

#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>

#include <QNetworkCookie>

#define DBG_OUT 0

BDiskCookieJar::BDiskCookieJar(QObject *parent)
    : QNetworkCookieJar(parent)
    , m_locker(QMutex::Recursive)
{
    QString dataPath = QStandardPaths::writableLocation (QStandardPaths::DataLocation);
    QDir dir(dataPath);
    if (!dir.exists())
        dir.mkpath(dataPath);
    m_cookieFile = QString("%1/cookie").arg(dataPath);

    QFile file(m_cookieFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))  {
        qDebug()<<Q_FUNC_INFO<<"Can't open exist cookies files";
    } else {
        QByteArray line;
        while (!file.atEnd()) {
            line = file.readLine();
            m_cookieList.append(QNetworkCookie::parseCookies(line));
        }
    }
    file.close();

}

BDiskCookieJar::~BDiskCookieJar()
{
    QFile file(m_cookieFile);
    if (file.exists()) {
        if (!file.remove())
            qDebug()<<Q_FUNC_INFO<<"Not remove file!!";
    }

    if (!file.open(QIODevice::WriteOnly)) {
        qDebug()<<Q_FUNC_INFO<<"Can't open to write cookies";
        return;
    }
    QTextStream out(&file);
    foreach (QNetworkCookie c, m_cookieList) {
        out<<c.toRawForm()<<"\n";
    }
    out.flush();
    file.close();
}

QList<QNetworkCookie> BDiskCookieJar::cookieList() const
{
    return m_cookieList;
}

void BDiskCookieJar::clear()
{
    m_locker.lock();
    m_cookieList.clear();
    QFile file(m_cookieFile);
    if (file.exists()) {
        if (!file.remove())
            qDebug()<<Q_FUNC_INFO<<"Not remove file!!";
    }
    m_locker.unlock();
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

//bool BDiskCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
//{
//    m_locker.lock();
//#if DBG_OUT
//    qDebug()<<Q_FUNC_INFO<<"cookiesForUrl "<<url;
//#endif

//    foreach (QNetworkCookie c, cookieList) {
//#if DBG_OUT
//        qDebug()<<Q_FUNC_INFO<<"cookies name="<<c.name ()<<" value="<<c.value ();
//#endif
//        if (!m_cookieList.contains(c)) {
//            m_cookieList.append(c);
//        }
//    }
//    m_locker.unlock();

//    return QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
//}

bool BDiskCookieJar::insertCookie(const QNetworkCookie &cookie)
{
    m_locker.lock();
#if DBG_OUT
    qDebug()<<Q_FUNC_INFO<<"cookies name="<<cookie.name ()<<" value="<<cookie.value ();
#endif
    foreach (QNetworkCookie c, m_cookieList) {
        if (c.hasSameIdentifier(cookie)) {
            m_cookieList.removeOne(c);
        }
    }
    m_cookieList.append(cookie);
    m_locker.unlock();
    return QNetworkCookieJar::insertCookie(cookie);
}

bool BDiskCookieJar::updateCookie(const QNetworkCookie &cookie)
{
    m_locker.lock();
#if DBG_OUT
    qDebug()<<Q_FUNC_INFO<<"cookies name="<<cookie.name ()<<" value="<<cookie.value ();
#endif
    foreach (QNetworkCookie c, m_cookieList) {
        if (c.hasSameIdentifier(cookie)) {
            m_cookieList.removeOne(c);
        }
    }
    m_cookieList.append(cookie);
    m_locker.unlock();
    return QNetworkCookieJar::updateCookie(cookie);
}

//bool BDiskCookieJar::deleteCookie(const QNetworkCookie &cookie)
//{
//    m_locker.lock();
//#if DBG_OUT
//    qDebug()<<Q_FUNC_INFO<<"cookies name="<<cookie.name ()<<" value="<<cookie.value ();
//#endif
//    foreach (QNetworkCookie c, m_cookieList) {
//        if (c.hasSameIdentifier(cookie))
//            m_cookieList.removeOne(c);
//    }
//    m_locker.unlock();
//    return QNetworkCookieJar::deleteCookie(cookie);
//}
