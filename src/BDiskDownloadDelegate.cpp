#include "BDiskDownloadDelegate.h"

#include <QCoreApplication>
#include <QDebug>

#include <QNetworkCookie>

#include "DLRequest.h"
#include "DLTask.h"

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
    foreach (DLTask *task, m_taskHash.values()) {
        task->abort();
    }
    qDeleteAll(m_taskHash);
    m_taskHash.clear();

    if (m_downloadMgr)
        m_downloadMgr->deleteLater();
    m_downloadMgr = nullptr;

}

//void BDiskDownloadDelegate::download(const QString &from, const QString &savePath, const QString &saveName)
//{
//    if (from.isEmpty() || savePath.isEmpty() || saveName.isEmpty())
//        return;
//    QString sp(savePath);
//    if (sp.startsWith("file://")) { //remove file:// scheme
//        sp = sp.remove(0, 7);
//    }
//    if (sp.endsWith("/")) { //remove last /
//        sp = sp.left(sp.length() -1);
//    }
//    QString sn(saveName);
//    if (sn.startsWith("/")) { //remove first /
//        sn = sn.remove(0, 1);
//    }
//    QString sv = QString("%1/%2").arg(sp).arg(sn);
//    download(from, sv);
//}

void BDiskDownloadDelegate::download(const QString &from, const QString &savePath, const QString &saveName)
{
    m_downloadOp.setParameters("path", from);
    QUrl url = m_downloadOp.initUrl();
    qDebug()<<Q_FUNC_INFO<<"download url is "<<url;
//    QString path = qApp->applicationDirPath();
    DLRequest req(url, savePath, saveName);
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

    DLTask *task = m_downloadMgr->get(req);
    m_taskHash.insert(task->uid(), task);
    task->start();
}
