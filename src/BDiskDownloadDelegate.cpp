#include "BDiskDownloadDelegate.h"

#include <QCoreApplication>
#include <QDebug>
#include <QVariantList>

#include <QNetworkCookie>

#include "DLRequest.h"
#include "DLTask.h"

#include "BDiskRequest/BDiskCookieJar.h"

const static QString KEY_TASK_HASH("TASK_HASH");
const static QString KEY_TASK_OBJECT("TASK_OBJECT");

using namespace YADownloader;

BDiskDownloadDelegate::BDiskDownloadDelegate(QObject *parent)
    : QObject(parent)
    , m_downloadMgr(new DLTaskAccessMgr(this))
{

    connect(m_downloadMgr, &DLTaskAccessMgr::resumablesChanged, [&](const DLTaskInfoList &list) {
        setTasks(parseDLTaskInfoList(list));
    });

    setTasks(parseDLTaskInfoList(m_downloadMgr->resumables()));
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
            m_taskInfoHash.remove(uuid);
            QVariantList list;
            foreach (DLTask *t, m_taskHash.values()) {
                list.append(t->taskInfo().toMap());
            }
//            setRunnings(list);
        }
    });
    connect(task, &DLTask::taskInfoChanged, [&](const QString &uuid, const DLTaskInfo &info) {
//        m_taskInfoHash.insert(uuid, info);
        for(int i=0; i<m_tasks.length(); ++i) {
            QVariantMap map = m_tasks.at(i).toMap();
            if (map.value(KEY_TASK_HASH).toString() == uuid) {
                QVariantMap mm;
                mm.insert(KEY_TASK_OBJECT, info.toMap());
                mm.insert(KEY_TASK_HASH, uuid);
                m_tasks.replace(i, mm);
                break;
            } else {
                DLTaskInfo info = DLTaskInfo::fromMap(map);
                if (info.isEmpty())
                    continue;


            }
        }
    });


    m_taskHash.insert(task->uuid(), task);
    m_taskInfoHash.insert(task->uuid(), task->taskInfo());
    task->start();
}

//QVariantList BDiskDownloadDelegate::runnings() const
//{
//    return m_runnings;
//}

//void BDiskDownloadDelegate::setRunnings(const QVariantList &runnings)
//{
//    if (m_runnings == runnings)
//        return;

//    m_runnings = runnings;
//    emit runningsChanged(runnings);
//}

QVariantList BDiskDownloadDelegate::tasks() const
{
    return m_tasks;
}

QString BDiskDownloadDelegate::KeyTaskHash() const
{
    return KEY_TASK_HASH;
}

QString BDiskDownloadDelegate::KeyTaskObject() const
{
    return KEY_TASK_OBJECT;
}

void BDiskDownloadDelegate::setTasks(const QVariantList &tasks)
{
    if (m_tasks == tasks)
        return;

    m_tasks = tasks;
    emit tasksChanged(tasks);
}

QVariantList BDiskDownloadDelegate::parseDLTaskInfoList(const DLTaskInfoList &list) const
{
    QVariantList ll;
    foreach (DLTaskInfo info, list) {
        QVariantMap map;
        map.insert(KEY_TASK_OBJECT, info.toMap());
        QString key = info.requestUrl() + info.filePath() + QString::number(info.totalSize());
        map.insert(KEY_TASK_HASH, key);
        ll.append(map);
    }
    return ll;
}
