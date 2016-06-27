#include "BDiskDownloadDelegate.h"

#include <QCoreApplication>
#include <QDebug>

#include <QNetworkCookie>

#include "DLRequest.h"

#include "BDiskRequest/BDiskCookieJar.h"

using namespace YADownloader;

BDiskDownloadDelegate::BDiskDownloadDelegate(QObject *parent)
    : QObject(parent)
    , m_downloadMgr(new DownloadMgr(this))
{

}

BDiskDownloadDelegate::~BDiskDownloadDelegate()
{
    qDebug()<<Q_FUNC_INFO<<"==============";
    if (m_downloadMgr)
        m_downloadMgr->deleteLater();
    m_downloadMgr = nullptr;

    if (m_downloadTask) {
        m_downloadTask->abort();
        m_downloadTask->deleteLater();
    }
    m_downloadTask = nullptr;

}

void BDiskDownloadDelegate::download()
{
    qDebug()<<Q_FUNC_INFO<<"========= ";
    m_downloadOp.setParameters("path", "/bcompare-3.3.7.15876.tar.gz");
    QUrl url = m_downloadOp.initUrl();
    qDebug()<<Q_FUNC_INFO<<"download url is "<<url;
    QString path = qApp->applicationDirPath();
    DLRequest req(url, path, "bibibi.7z");
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
    req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
//    req.setRawHeader("Accept", "*/*");
    req.setPreferThreadCount(4);

    QList<QNetworkCookie> cookies = BDiskCookieJar::instance()->cookieList();
    QStringList list;
    foreach (QNetworkCookie c, cookies) {
        list.append(QString("%1=%2").arg(QString(c.name())).arg(QString(c.value())));
    }
    req.setRawHeader("Cookie", list.join(";").toUtf8());

    m_downloadTask = m_downloadMgr->get(req);
    m_downloadTask->start();
}
