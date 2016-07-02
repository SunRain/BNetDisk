#ifndef BDISKCOOKIEJAR_H
#define BDISKCOOKIEJAR_H

#include <QNetworkCookieJar>

#include <QMutex>
#include <QList>

#include "SingletonPointer.h"

class QSettings;
class BDiskCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
    DECLARE_SINGLETON_POINTER(BDiskCookieJar)

public:
    virtual ~BDiskCookieJar();
    QList<QNetworkCookie> cookieList() const;

    ///
    /// \brief clear
    /// Clear all stored cookies data
    ///
    void clear();

    // QNetworkCookieJar interface
    QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
//    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
    bool insertCookie(const QNetworkCookie &cookie);
    bool updateCookie(const QNetworkCookie &cookie);
//    bool deleteCookie(const QNetworkCookie &cookie);

private:
    QMutex m_locker;
    QList<QNetworkCookie> m_cookieList;
    QString m_cookieFile;
};

#endif // BDISKCOOKIEJAR_H
