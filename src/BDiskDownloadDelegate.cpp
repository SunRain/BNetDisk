#include "BDiskDownloadDelegate.h"

#include <QCoreApplication>
#include <QDebug>
#include <QVariantList>
#include <QTimer>

#include <QNetworkCookie>

#include "DLRequest.h"
#include "DLTask.h"

#include "BDiskRequest/BDiskCookieJar.h"

#include "BDiskEvent.h"

using namespace YADownloader;

BDiskDownloadDelegate::BDiskDownloadDelegate(QObject *parent)
    : QObject(parent)
    , m_downloadMgr(new DLTaskAccessMgr(this))
    , m_timer(new QTimer(this))
    , m_timerCount(0)
{
    m_timer->setInterval(1000);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, [&]{
        m_timerCount++;
        QStringList cntHashKeys = m_taskStartTimeCntHash.keys();
        foreach (DLTask *task, m_taskHash) {
            QString key = task->uuid();
            if (!cntHashKeys.contains(key))
                continue;
            int startOffset = m_taskStartTimeCntHash.value(key);
            int elapse = m_timerCount - startOffset;
            int fSize = task->bytesFileSize();
            int bd = task->bytesDownloaded();
//            int boffset = task->bytesStartOffest();
            int br = task->bytesReceived();

            int dlSpeed = br/elapse;
            float dlPercent = (float)bd/(float)fSize;
            BDiskEvent::instance()->dispatchDownloadProgress(key,
                                                             QString::number(dlSpeed),
                                                             QString::number(dlPercent));
        }
    });

    connect(m_downloadMgr, &DLTaskAccessMgr::resumablesChanged, [&](const DLTaskInfoList &list) {
//        setTasks(parseDLTaskInfoList(list));
        parseDLTaskInfoList(list);
        setTasks(convertTaskInfoHash());
    });

//    setTasks(parseDLTaskInfoList(m_downloadMgr->resumables()));
    parseDLTaskInfoList(m_downloadMgr->resumables());
    setTasks(convertTaskInfoHash());
    m_timer->start();
}

BDiskDownloadDelegate::~BDiskDownloadDelegate()
{
    qDebug()<<Q_FUNC_INFO<<"==============";

    if (m_timer->isActive())
        m_timer->stop();
    m_timer->deleteLater();
    m_timer = nullptr;

    foreach (DLTask *task, m_taskHash.values()) {
        task->abort();
    }
    qDeleteAll(m_taskHash);
    m_taskHash.clear();

    if (m_downloadMgr)
        m_downloadMgr->deleteLater();
    m_downloadMgr = nullptr;

}

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

    connect(task, &DLTask::statusChanged, [&](const QString &uuid, DLTask::TaskStatus status) {
        if (status == DLTask::TaskStatus::DL_FINISH) {
            m_taskHash.value(uuid)->deleteLater();
            m_taskHash.remove(uuid);
            QStringList idList = m_taskInfoHash.keys();
            if (idList.contains(uuid)) {
                m_taskInfoHash.remove(uuid);
                m_taskStartTimeCntHash.remove(uuid);
//                setTasks(parseDLTaskInfoList(m_taskInfoHash.values()));
                setTasks(convertTaskInfoHash());
            }
        } else  if (status == DLTask::TaskStatus::DL_STOP) {
            m_taskHash.value(uuid)->deleteLater();
            m_taskHash.remove(uuid);
            QStringList idList = m_taskInfoHash.keys();
            if (idList.contains(uuid)) {
                m_taskInfoHash.remove(uuid);
                m_taskStartTimeCntHash.remove(uuid);
//                setTasks(parseDLTaskInfoList(m_taskInfoHash.values()));
                setTasks(convertTaskInfoHash());
            }
        } else  if (status == DLTask::TaskStatus::DL_START) {
            QStringList idList = m_taskInfoHash.keys();
            if (idList.contains(uuid)) {
                DLTaskInfo info = m_taskInfoHash.value(uuid);
                info.setStatus(DLTaskInfo::TaskStatus::STATUS_RUNNING);
                m_taskInfoHash.insert(uuid, info);
            } else {
                /// tmp QHash to ensure order not changed
                QHash<QString, YADownloader::DLTaskInfo> hashs;
                foreach (QString key, m_taskInfoHash.keys()) {
                    DLTaskInfo info = m_taskInfoHash.value(key);
                    if (info.hasSameIdentifier(m_taskHash.value(uuid)->taskInfo())) {
                        hashs.insert(uuid, info);
                    } else {
                        hashs.insert(key, info);
                    }
                }
                m_taskInfoHash = hashs;
            }
            m_taskStartTimeCntHash.insert(uuid, m_timerCount);
//            setTasks(parseDLTaskInfoList(m_taskInfoHash.values()));
            setTasks(convertTaskInfoHash());
        }
    });
    connect(task, &DLTask::taskInfoChanged, [&](const QString &uuid, const DLTaskInfo &info) {
        QStringList idList = m_taskInfoHash.keys();
        if (idList.contains(uuid)) {
            m_taskInfoHash.insert(uuid, info);
        } else {
            /// tmp QHash to ensure order not changed
            QHash<QString, YADownloader::DLTaskInfo> hashs;
            foreach (QString key, m_taskInfoHash.keys()) {
                DLTaskInfo ii = m_taskInfoHash.value(key);
                if (ii.hasSameIdentifier(info)) {
                    hashs.insert(uuid, info);
                } else {
                    hashs.insert(key, info);
                }
            }
            m_taskInfoHash = hashs;
        }
//        setTasks(parseDLTaskInfoList(m_taskInfoHash.values()));
//        setTasks(convertTaskInfoHash());
    });

    m_taskHash.insert(task->uuid(), task);
//    m_taskInfoHash.insert(task->uuid(), task->taskInfo());
    task->start();
}

QVariantList BDiskDownloadDelegate::tasks() const
{
    return m_tasks;
}

void BDiskDownloadDelegate::setTasks(const QVariantList &tasks)
{
    if (m_tasks == tasks)
        return;

    qDebug()<<Q_FUNC_INFO<<" tasks changed ";
    m_tasks = tasks;
    emit tasksChanged(tasks);
}

void BDiskDownloadDelegate::parseDLTaskInfoList(const DLTaskInfoList &list)
{
    foreach (DLTaskInfo info, list) {
        if (info.identifier().isEmpty()) {
            info.setIdentifier(info.requestUrl()
                               + info.filePath()
                               + QString::number(info.totalSize()));
        }
        m_taskInfoHash.insert(info.identifier(), info);
    }
}

QVariantList BDiskDownloadDelegate::convertTaskInfoHash()
{
    QVariantList ll;
    foreach (DLTaskInfo info, m_taskInfoHash.values()) {
        ll.append(info.toMap());
    }
    return ll;
}
