#ifndef BDISKCOOKIEJAR_H
#define BDISKCOOKIEJAR_H

#include <QNetworkCookieJar>
//#include <QNetworkCookie>

#include <QMutex>
#include <QList>

#include "SingletonPointer.h"

class BDiskCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
    DECLARE_SINGLETON_POINTER(BDiskCookieJar)

public:
    QHash<QByteArray, QByteArray> cookieList() const;

    // QNetworkCookieJar interface
    QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);

private:
    QMutex m_locker;
//    QList<QNetworkCookie> m_cookieList;
    QHash<QByteArray, QByteArray> m_cookieList;
//    bool insertCookie(const QNetworkCookie &cookie);
//    bool updateCookie(const QNetworkCookie &cookie);
//    bool deleteCookie(const QNetworkCookie &cookie);
};

#endif // BDISKCOOKIEJAR_H
