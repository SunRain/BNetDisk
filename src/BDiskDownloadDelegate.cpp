#include "BDiskDownloadDelegate.h"

#include <QCoreApplication>
#include <QDebug>
#include <QVariantList>
#include <QTimer>
#include <QSharedData>

#include <QNetworkCookie>

#include "DLRequest.h"
#include "DLTask.h"

#include "BDiskRequest/BDiskCookieJar.h"

#include "BDiskEvent.h"

using namespace YADownloader;

class BDiskDownloadDelegateNodePriv : public QSharedData
{
public:
    BDiskDownloadDelegateNodePriv()
        : info(DLTaskInfo())
        , identifier(QString())
        , elapsedTimeOffset(0)
    {

    }
    virtual ~BDiskDownloadDelegateNodePriv() {}
    DLTaskInfo info;
    QString identifier;
    qint64 elapsedTimeOffset;
};

BDiskDownloadDelegateNode::BDiskDownloadDelegateNode()
    : m_task(nullptr)
    , d(new BDiskDownloadDelegateNodePriv())
{

}

BDiskDownloadDelegateNode::BDiskDownloadDelegateNode(const BDiskDownloadDelegateNode &other)
    : m_task(other.task())
    , d(other.d)
{
    m_task = other.task();
}

BDiskDownloadDelegateNode::~BDiskDownloadDelegateNode()
{
    m_task = nullptr;
}

bool BDiskDownloadDelegateNode::operator ==(const BDiskDownloadDelegateNode &other) const
{
    return m_task == other.task()
            && d.data()->elapsedTimeOffset == other.d.data()->elapsedTimeOffset
            && d.data()->identifier == other.d.data()->identifier;
}

bool BDiskDownloadDelegateNode::operator !=(const BDiskDownloadDelegateNode &other) const
{
    return !operator ==(other);
}

BDiskDownloadDelegateNode &BDiskDownloadDelegateNode::operator =(const BDiskDownloadDelegateNode &other)
{
    if (this != &other)
        d.operator =(other.d);
    return *this;
}

DLTask *BDiskDownloadDelegateNode::task() const
{
    return m_task;
}

DLTaskInfo BDiskDownloadDelegateNode::placeholderTaskInfo() const
{
    return d.data()->info;
}

QString BDiskDownloadDelegateNode::identifier() const
{
    return d.data()->identifier;
}

qint64 BDiskDownloadDelegateNode::elapsedTimeOffset() const
{
    return d.data()->elapsedTimeOffset;
}

void BDiskDownloadDelegateNode::setTask(DLTask *task)
{
    m_task = task;
}

void BDiskDownloadDelegateNode::setPlaceholderTaskInfo(const DLTaskInfo &info)
{
    d.data()->info = info;
}

void BDiskDownloadDelegateNode::setIdentifier(const QString &hash)
{
    d.data()->identifier = hash;
}

void BDiskDownloadDelegateNode::setElapsedTimeOffset(qint64 offset)
{
    d.data()->elapsedTimeOffset = offset;
}

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
        foreach (QString key, m_nodeHash.keys()) {
            BDiskDownloadDelegateNode node = m_nodeHash.value(key);
            if (!node.task() || (node.identifier() != node.task()->uuid()))
                continue;
            int elapsedOffset = node.elapsedTimeOffset();
            int elapse = m_timerCount - elapsedOffset;
            int fSize = node.task()->bytesFileSize();
            int bd = node.task()->bytesDownloaded();
//          int boffset = task->bytesStartOffest();
            int br = node.task()->bytesReceived();
            int dlSpeed = br/elapse;
            float dlPercent = (float)bd/(float)fSize;
            BDiskEvent::instance()->dispatchDownloadProgress(key,
                                                             QString::number(dlSpeed),
                                                             QString::number(dlPercent));
        }
    });

    connect(m_downloadMgr, &DLTaskAccessMgr::resumablesChanged, [&](const DLTaskInfoList &list) {
        m_locker.lock();
        parseDLTaskInfoList(list);
        QVariantList va(convertTaskInfoHash());
        m_locker.unlock();
        setTasks(va);
    });

    parseDLTaskInfoList(m_downloadMgr->resumables());
    setTasks(convertTaskInfoHash());

    qDebug()<<Q_FUNC_INFO<<" .............. m_nodeHash size "<<m_nodeHash.size();

    m_timer->start();
}

BDiskDownloadDelegate::~BDiskDownloadDelegate()
{
    qDebug()<<Q_FUNC_INFO<<"==============";

    if (m_timer->isActive())
        m_timer->stop();
    m_timer->deleteLater();
    m_timer = nullptr;

    //FIXME abort all tasks in destruction ?
    foreach (QString key, m_nodeHash.keys()) {
        BDiskDownloadDelegateNode node = m_nodeHash.value(key);
        if (node.task()) {
            node.task()->abort();
            node.task()->deleteLater();
        }
        node.setTask(nullptr);
    }
//    qDeleteAll(m_nodeHash);
    m_nodeHash.clear();

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
        QMutexLocker locker(&m_locker);
        if (status == DLTask::TaskStatus::DL_FINISH) {
            if (m_nodeHash.value(uuid).task() && m_nodeHash.value(uuid).identifier() == uuid) {
                m_nodeHash.value(uuid).task()->deleteLater();
                m_nodeHash.remove(uuid);
                QVariantList list = convertTaskInfoHash();
                locker.unlock();
                setTasks(list);
            }
        } else  if (status == DLTask::TaskStatus::DL_STOP) {
            if (m_nodeHash.value(uuid).task() && m_nodeHash.value(uuid).identifier() == uuid) {
                m_nodeHash.value(uuid).task()->deleteLater();
                QVariantList list = convertTaskInfoHash();
                locker.unlock();
                setTasks(list);
            }
        } else  if (status == DLTask::TaskStatus::DL_START) {
            qDebug()<<Q_FUNC_INFO<<" ............... DL_START";
            QStringList idList = m_nodeHash.keys();
            if (idList.contains(uuid)) {
                BDiskDownloadDelegateNode node = m_nodeHash.value(uuid);
                if (!node.task()) {
                    qWarning()<<Q_FUNC_INFO<<"Current running task not in nodelist!!";
                    return;
                }
                DLTaskInfo info = node.task()->taskInfo();
                info.setStatus(DLTaskInfo::TaskStatus::STATUS_RUNNING);
                node.setPlaceholderTaskInfo(info);
                node.setIdentifier(uuid);
                node.setElapsedTimeOffset(m_timerCount);
                m_nodeHash.insert(uuid, node);
            } else {
                /// tmp QHash to ensure order not changed
                QHash<QString, BDiskDownloadDelegateNode> hash;
                foreach (QString key, idList) {
                    BDiskDownloadDelegateNode node = m_nodeHash.value(key);
                    if (!node.task()) {
                        //TODO cause crash, better method needed
//                        QObject *t = sender();
//                        qDebug()<<Q_FUNC_INFO<<" ............... t is nullptr "<<(t == nullptr);
//                        if (t) {
//                            DLTask *ts = qobject_cast<DLTask*>(t);
//                            if (ts) {
//                                qDebug()<<Q_FUNC_INFO<<" ............... ts is nullptr "<<(ts == nullptr);
//                                if (node.placeholderTaskInfo().hasSameIdentifier(ts->taskInfo())) {
//                                    qDebug()<<Q_FUNC_INFO<<" ............. running task find in database";
//                                    node.setIdentifier(ts->uuid());
//                                    node.setTask(ts);
//                                    DLTaskInfo info = node.task()->taskInfo();
//                                    info.setStatus(DLTaskInfo::TaskStatus::STATUS_RUNNING);
//                                    node.setPlaceholderTaskInfo(info);
//                                    node.setElapsedTimeOffset(m_timerCount);
//                                    hash.insert(node.identifier(), node);
//                                }
//                            }
//                        } else {
//                            hash.insert(node.identifier(), node);
//                        }
                        continue;
                    }
                    DLTaskInfo placeholder = node.placeholderTaskInfo();
                    if (placeholder.hasSameIdentifier(node.task()->taskInfo())) {
                        node.setIdentifier(uuid);
                        node.setPlaceholderTaskInfo(node.task()->taskInfo());
                        node.setElapsedTimeOffset(m_timerCount);
                        hash.insert(uuid, node);
                    }
                }
                m_nodeHash = hash;
            }
            QVariantList list = convertTaskInfoHash();
            locker.unlock();
            setTasks(list);
        }

//        qDebug()<<Q_FUNC_INFO<<" .............. m_nodeHash size "<<m_nodeHash.size();

    });
    connect(task, &DLTask::taskInfoChanged, [&](const QString &uuid, const DLTaskInfo &info) {
        m_locker.lock();
        QStringList idList = m_nodeHash.keys();
        if (idList.contains(uuid)) {
            BDiskDownloadDelegateNode node = m_nodeHash.value(uuid);
            if (!node.task())
                return;
            node.setIdentifier(uuid);
            node.setPlaceholderTaskInfo(info);
            m_nodeHash.insert(uuid, node);
        } else {
            /// tmp QHash to ensure order not changed
            QHash<QString, BDiskDownloadDelegateNode> hash;
            foreach (QString key, idList) {
                BDiskDownloadDelegateNode node = m_nodeHash.value(key);
                if (!node.task()) {
                    hash.insert(node.identifier(), node);
                    continue;
                }
                DLTaskInfo placeholder = node.placeholderTaskInfo();
                if (placeholder.hasSameIdentifier(node.task()->taskInfo())) {
                    node.setIdentifier(uuid);
                    node.setPlaceholderTaskInfo(node.task()->taskInfo());
                    node.setElapsedTimeOffset(m_timerCount);
                    hash.insert(uuid, node);
                }
            }
            m_nodeHash = hash;
        }
        m_locker.unlock();

//        qDebug()<<Q_FUNC_INFO<<" .............. m_nodeHash size "<<m_nodeHash.size();

    });

//    m_locker.lock();
//    BDiskDownloadDelegateNode node;
//    node.setIdentifier(task->uuid());
//    node.setPlaceholderTaskInfo(task->taskInfo());
//    node.setTask(task);
//    m_nodeHash.insert(task->uuid(), node);
//    m_locker.unlock();

//    qDebug()<<Q_FUNC_INFO<<" .............. m_nodeHash size "<<m_nodeHash.size();

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
    QStringList keys = m_nodeHash.keys();

    foreach (DLTaskInfo info, list) {
        bool skip = false;
        foreach (QString key, keys) {
            BDiskDownloadDelegateNode node = m_nodeHash.value(key);
            if (node.placeholderTaskInfo().hasSameIdentifier(info) || node.identifier() == info.identifier()) {
                node.setPlaceholderTaskInfo(info);
                m_nodeHash.insert(node.identifier(), node);
                skip = true;
                break;
            }
        }
        if (skip)
            continue;
        if (info.identifier().isEmpty()) {
            info.setIdentifier(info.requestUrl()
                               + info.filePath()
                               + QString::number(info.totalSize()));
        }
        BDiskDownloadDelegateNode node;
        node.setIdentifier(info.identifier());
        node.setPlaceholderTaskInfo(info);
        node.setTask(nullptr);
        m_nodeHash.insert(node.identifier(), node);
    }
}

QVariantList BDiskDownloadDelegate::convertTaskInfoHash()
{
    QVariantList ll;
    foreach (QString key, m_nodeHash.keys()) {
        if (m_nodeHash.value(key).task()) {
            ll.append(m_nodeHash.value(key).task()->taskInfo().toMap());
        } else {
            if (!m_nodeHash.value(key).placeholderTaskInfo().isEmpty()) {
                ll.append(m_nodeHash.value(key).placeholderTaskInfo().toMap());
            }
        }
    }
    return ll;
}

