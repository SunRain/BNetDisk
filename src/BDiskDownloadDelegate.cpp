#include "BDiskDownloadDelegate.h"

#include <QCoreApplication>
#include <QDebug>
#include <QVariantList>
#include <QTimer>
#include <QSharedData>
#include <QFileInfo>
#include <QDateTime>
#include <QStandardPaths>

//#include <QNetworkCookie>

#include "QCNetworkRequest.h"
#include "QCNetworkAccessManager.h"

#include "DLTransmissionDatabase.h"
#include "DLRequest.h"
#include "DLTask.h"

#include "BDiskRequest/BDiskCookieJar.h"

#include "BDiskEvent.h"

using namespace YADownloader;
using namespace QCurl;

class BDiskDownloadCompleteTaskDB : public YADownloader::DLTransmissionDatabase
{
    Q_OBJECT
public:
    explicit BDiskDownloadCompleteTaskDB(QObject *parent = 0)
        : DLTransmissionDatabase(parent)
    {
        initiate();
    }
    virtual ~BDiskDownloadCompleteTaskDB() {}

    // DLTransmissionDatabase interface
protected:
    QString cfgFile() {
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
        return QString("%1/complete_task.json").arg(dataPath);
    }
    void onAppendTaskInfo(const DLTaskInfo &info) {
        dataHash()->insert(info.identifier(), info);
    }
    int onRemoveTaskInfo(const DLTaskInfo &info) {
        return dataHash()->remove(info.identifier());
    }
};

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
    this->m_task = other.task();
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
    , m_completeDB(new BDiskDownloadCompleteTaskDB(this))
//    , m_diskEvent(BDiskEvent::instance())
    , m_timerCount(0)
{
    /*
     * set global rawheader
     */
//    QList<QNetworkCookie> cookies = BDiskCookieJar::instance()->cookieList();
//    QStringList list;
//    foreach (QNetworkCookie c, cookies) {
//        list.append(QString("%1=%2").arg(QString(c.name())).arg(QString(c.value())));
//    }
//    m_downloadMgr->setRawHeader("Cookie", list.join(";").toUtf8());
    m_downloadMgr->setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
    m_downloadMgr->setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

    m_timer->setInterval(1000);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, [&]{
        m_timerCount++;
        m_locker.lock();
        foreach (QString key, m_nodeHash.keys()) {
            BDiskDownloadDelegateNode node = m_nodeHash.value(key);
            if (!node.task() || (node.identifier() != node.task()->uuid()))
                continue;
            if (node.placeholderTaskInfo().status() == DLTaskInfo::TaskStatus::STATUS_STOP) {
//                node.setElapsedTimeOffset(m_timerCount);
                continue;
            }
            int elapsedOffset = node.elapsedTimeOffset();
            int elapse = m_timerCount - elapsedOffset;
            qint64 runOffset = m_timerStartedMSecsSinceEpoch + elapsedOffset*1000;
            qint64 runElapse = QDateTime::currentMSecsSinceEpoch() - runOffset;
//            qDebug()<<Q_FUNC_INFO<<"=========== runElapse "<<runElapse;

//            int fSize = node.task()->bytesFileSize();
            int bd = node.task()->bytesDownloaded();
//          int boffset = task->bytesStartOffest();
            int br = node.task()->bytesReceived();
            int dlSpeed = br/elapse;
//            float dlPercent = (float)bd/(float)fSize;
//            m_diskEvent->dispatchDownloadProgress(key,
//                                                  QString::number(dlSpeed),
//                                                  QString::number(dlPercent));
            BDiskEvent::instance()->dispatchDownloadProgress(key, dlSpeed, bd, runElapse);
        }
        m_locker.unlock();
    });

    connect(m_completeDB, &DLTransmissionDatabase::listChanged, [&]() {
        parseCompletedTasks(m_completeDB->list());
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

    parseCompletedTasks(m_completeDB->list());

    m_timer->start();
    m_timerStartedMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
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
    DLRequest req(url.toString(), savePath, saveName);
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
    req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
//    req.setRawHeader("Accept", "*/*");
    req.setPreferThreadCount(4);

//    QList<QNetworkCookie> cookies = BDiskCookieJar::instance()->cookieList();
//    QStringList list;
//    foreach (QNetworkCookie c, cookies) {
//        list.append(QString("%1=%2").arg(QString(c.name())).arg(QString(c.value())));
//    }
//    req.setRawHeader("Cookie", list.join(";").toUtf8());

    DLTask *task = m_downloadMgr->get(req);
    task->requestPtr()->setCookieFilePath(getCookieFile(), DLRequest::ReadOnly);

    connectTask(&task);

    m_locker.lock();
    BDiskDownloadDelegateNode node;
    node.setIdentifier(task->uuid());
    node.setPlaceholderTaskInfo(task->taskInfo());
    node.setTask(task);
//    m_tmpNode.insert(task->uuid(), node);
    m_nodeHash.insert(task->uuid(), node);
    m_locker.unlock();

    task->start();
}

bool BDiskDownloadDelegate::taskRunning(const QString &hash)
{
    if (hash.isEmpty())
        return false;
    if (!m_nodeHash.keys().contains(hash)) {
        qWarning()<<Q_FUNC_INFO<<QString("Current hash [%1] not existed in database!!!").arg(hash);
        return false;
    }
    if (m_nodeHash.value(hash).placeholderTaskInfo().status() == DLTaskInfo::TaskStatus::STATUS_STOP)
        return false;
    return true;
}

QVariantList BDiskDownloadDelegate::tasks() const
{
    return m_tasks;
}

QVariantList BDiskDownloadDelegate::completedTasks() const
{
    return m_completedTasks;
}

void BDiskDownloadDelegate::stop(const QString &hash)
{
    if (hash.isEmpty())
        return;
    if (!m_nodeHash.keys().contains(hash)) {
        qWarning()<<Q_FUNC_INFO<<QString("Current hash [%1] not existed in database!!!").arg(hash);
        return;
    }
    if (m_nodeHash.value(hash).task()) {
        m_nodeHash.value(hash).task()->suspend();
    }
}

void BDiskDownloadDelegate::resume(const QString &hash)
{
    if (hash.isEmpty())
        return;
    if (!m_nodeHash.keys().contains(hash)) {
        qWarning()<<Q_FUNC_INFO<<QString("Current hash [%1] not existed in database!!!").arg(hash);
        return;
    }
    QMutexLocker lock(&m_locker);
    if (m_nodeHash.value(hash).task()) {
        m_nodeHash.value(hash).task()->resume();
        lock.unlock();
    } else {
        DLTaskInfo info = m_nodeHash.value(hash).placeholderTaskInfo();
        lock.unlock();

//        QList<QNetworkCookie> cookies = BDiskCookieJar::instance()->cookieList();
//        QStringList list;
//        foreach (QNetworkCookie c, cookies) {
//            list.append(QString("%1=%2").arg(QString(c.name())).arg(QString(c.value())));
//        }
//        m_downloadMgr->setRawHeader("Cookie", list.join(";").toUtf8());

        DLTask *task = m_downloadMgr->get(info.identifier());
        task->requestPtr()->setCookieFilePath(getCookieFile(), DLRequest::ReadOnly);

        qDebug()<<Q_FUNC_INFO<<"------ from map, task uuid "<<task->uuid()
               <<" identifier "<<hash;

        connectTask(&task);

        m_locker.lock();
        BDiskDownloadDelegateNode node;
        node.setIdentifier(task->uuid());
        node.setPlaceholderTaskInfo(task->taskInfo());
        node.setTask(task);
        m_nodeHash.insert(task->uuid(), node);
        m_locker.unlock();

        task->start();
    }
}

void BDiskDownloadDelegate::setCompletedTasks(const QVariantList &completedTasks)
{
    if (m_completedTasks == completedTasks)
        return;

    m_completedTasks = completedTasks;
    emit completedTasksChanged(completedTasks);
}

void BDiskDownloadDelegate::setTasks(const QVariantList &tasks)
{
    if (m_tasks == tasks)
        return;

    //    qDebug()<<Q_FUNC_INFO<<"================== setTasks ";

    m_tasks = tasks;
    emit tasksChanged(tasks);
}

void BDiskDownloadDelegate::connectTask(DLTask **task)
{
    if (!task || !(*task))
        return;

    connect((*task), &DLTask::statusChanged, [&](const QString &uuid, DLTask::TaskStatus status) {
        QMutexLocker locker(&m_locker);
        if (status == DLTask::TaskStatus::DL_FINISH) {
            BDiskDownloadDelegateNode node = m_nodeHash.value(uuid);
            if (node.task() && node.identifier() == uuid) {
                DLTaskInfo info = node.task()->taskInfo();
                node.task()->deleteLater();
                node.setTask(nullptr);
                m_nodeHash.insert(uuid, node);
                m_nodeHash.remove(uuid);

                info.setIdentifier(QString::number(QDateTime::currentMSecsSinceEpoch()));
                m_completeDB->appendTaskInfo(info);

                QVariantList list = convertTaskInfoHash();
                locker.unlock();
                setTasks(list);
            }
        } else  if (status == DLTask::TaskStatus::DL_STOP) {
            if (m_nodeHash.value(uuid).task() && m_nodeHash.value(uuid).identifier() == uuid) {
                BDiskDownloadDelegateNode node = m_nodeHash.value(uuid);
                DLTaskInfo info = node.task()->taskInfo();//node.placeholderTaskInfo();
                info.setStatus(DLTaskInfo::TaskStatus::STATUS_STOP);
                node.setPlaceholderTaskInfo(info);
                node.task()->deleteLater();
                node.setTask(nullptr);
//                m_nodeHash.value(uuid).task()->deleteLater();
                m_nodeHash.insert(uuid, node);
                QVariantList list = convertTaskInfoHash();
                locker.unlock();
                setTasks(list);
                BDiskEvent::instance()->dispatchTaskStatus(uuid, BDiskEvent::TaskStatus::STATUS_STOP);
            }
        } else  if (status == DLTask::TaskStatus::DL_START) {
            qDebug()<<Q_FUNC_INFO<<" DL_START for hash "<<uuid;

            QStringList idList = m_nodeHash.keys();
            if (idList.contains(uuid)) {
                BDiskDownloadDelegateNode node = m_nodeHash.value(uuid);
//                if (!node.task()) {
//                    if (!m_tmpNode.isEmpty()) {
//                        if (m_tmpNode.keys().contains(uuid)) {
//                            qDebug()<<Q_FUNC_INFO<<QString("Insert running task [%1] to nodelist!!").arg(uuid);
//                            node = m_tmpNode.value(uuid);
//                            node.setIdentifier(uuid);
//                            DLTaskInfo info = m_tmpNode.value(uuid).task()->taskInfo();
//                            info.setStatus(DLTaskInfo::TaskStatus::STATUS_RUNNING);
//                            node.setPlaceholderTaskInfo(info);
//                            node.setElapsedTimeOffset(m_timerCount);
//                            m_tmpNode.remove(uuid);
//                            m_nodeHash.insert(uuid, node);
//                        }
//                    }
//                } else {
//                    DLTaskInfo info = node.task()->taskInfo();
//                    info.setStatus(DLTaskInfo::TaskStatus::STATUS_RUNNING);
//                    node.setPlaceholderTaskInfo(info);
//                    node.setIdentifier(uuid);
//                    node.setElapsedTimeOffset(m_timerCount);
//                    m_nodeHash.insert(uuid, node);
//                }
                DLTaskInfo info = node.task()->taskInfo();
                info.setStatus(DLTaskInfo::TaskStatus::STATUS_RUNNING);
                node.setPlaceholderTaskInfo(info);
                node.setElapsedTimeOffset(m_timerCount);
                m_nodeHash.insert(node.identifier(), node);
            } else {
                qWarning()<<Q_FUNC_INFO<<"============= not in list";
//                /// tmp QHash to ensure order not changed
//                QHash<QString, BDiskDownloadDelegateNode> hash;
//                foreach (QString key, idList) {
//                    BDiskDownloadDelegateNode node = m_nodeHash.value(key);
//                    if (!node.task()) {
//                        if (!m_tmpNode.isEmpty()) {
//                            if (m_tmpNode.keys().contains(uuid)) {
//                                qDebug()<<Q_FUNC_INFO<<" fond node in tmp node";
//                                BDiskDownloadDelegateNode n = m_tmpNode.value(uuid);
//                                n.setIdentifier(uuid);
//                                DLTaskInfo info = n.task()->taskInfo();
//                                info.setStatus(DLTaskInfo::TaskStatus::STATUS_RUNNING);
//                                n.setPlaceholderTaskInfo(info);
//                                n.setElapsedTimeOffset(m_timerCount);
//                                m_tmpNode.remove(uuid);
//                                hash.insert(uuid, n);
//                                continue;
//                            }
//                        }
//                        hash.insert(node.identifier(), node);
//                        continue;
//                    } else {
//                        DLTaskInfo placeholder = node.placeholderTaskInfo();
//                        if (placeholder.hasSameIdentifier(node.task()->taskInfo())) {
//                            node.setIdentifier(uuid);
//                            DLTaskInfo info = node.task()->taskInfo();
//                            info.setStatus(DLTaskInfo::TaskStatus::STATUS_RUNNING);
//                            node.setPlaceholderTaskInfo(info);
//                            node.setElapsedTimeOffset(m_timerCount);
//                            hash.insert(uuid, node);
//                        }
//                    }
//                }
//                m_nodeHash = hash;
            }
            QVariantList list = convertTaskInfoHash();
            locker.unlock();
            setTasks(list);
            BDiskEvent::instance()->dispatchTaskStatus(uuid, BDiskEvent::TaskStatus::STATUS_RUNNING);
        }
    });
    connect((*task), &DLTask::taskInfoChanged, [&](const QString &uuid, const DLTaskInfo &info) {
        QMutexLocker locker(&m_locker);
        QStringList idList = m_nodeHash.keys();
        if (idList.contains(uuid)) {
            BDiskDownloadDelegateNode node = m_nodeHash.value(uuid);
            if (node.task()) {
                node.setIdentifier(uuid);
                node.setPlaceholderTaskInfo(info);
                m_nodeHash.insert(uuid, node);
            }
        } /*else {
            /// tmp QHash to ensure order not changed
            QHash<QString, BDiskDownloadDelegateNode> hash;
            foreach (QString key, idList) {
                BDiskDownloadDelegateNode node = m_nodeHash.value(key);
                if (!node.task()) {
                    node.setPlaceholderTaskInfo(info);
                    hash.insert(node.identifier(), node);
                    continue;
                }
                DLTaskInfo placeholder = node.placeholderTaskInfo();
                if (placeholder.hasSameIdentifier(node.task()->taskInfo())) {
                    node.setIdentifier(uuid);
                    node.setPlaceholderTaskInfo(info);
                    node.setElapsedTimeOffset(m_timerCount);
                    hash.insert(uuid, node);
                }
            }
            m_nodeHash = hash;
        }*/
        QVariantList list = convertTaskInfoHash();
        locker.unlock();
        setTasks(list);

//        if (info.status() == DLTaskInfo::TaskStatus::STATUS_STOP) {
//            BDiskEvent::instance()->dispatchTaskStatus(uuid, BDiskEvent::TaskStatus::STATUS_STOP);
//        } else {
//            BDiskEvent::instance()->dispatchTaskStatus(uuid, BDiskEvent::TaskStatus::STATUS_RUNNING);
//        }
    });
}

void BDiskDownloadDelegate::parseDLTaskInfoList(const DLTaskInfoList &list)
{
    QStringList keys = m_nodeHash.keys();
    QHash<QString, BDiskDownloadDelegateNode> hash;

    /// change list in m_nodeHash
    ///
    foreach (DLTaskInfo info, list) {
        bool skip = false;
        foreach (QString key, keys) {
            BDiskDownloadDelegateNode node = m_nodeHash.value(key);
            if (node.placeholderTaskInfo().hasSameIdentifier(info) || node.identifier() == info.identifier()) {
                node.setPlaceholderTaskInfo(info);
                if (key != info.identifier()) {
                    hash.insert(info.identifier(), node);
                } else {
                    hash.insert(key, node);
                }
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
        hash.insert(node.identifier(), node);
    }
    /// append exist to hash
    ///
    QStringList tmpKeys;
    foreach (const QString key, keys) {
        if (hash.keys().contains(key))
            continue;
        tmpKeys.append(key);
    }
    foreach (QString key, tmpKeys) {
        BDiskDownloadDelegateNode node = m_nodeHash.value(key);
//        qDebug()<<Q_FUNC_INFO<<" insert exist, task pointer "<<(node.task());
        hash.insert(key, node);
    }
    m_nodeHash = hash;
}

void BDiskDownloadDelegate::parseCompletedTasks(const DLTaskInfoList &list)
{
    DLTaskInfoList tasks = list;
    qStableSort(tasks.begin(), tasks.end(), [](const DLTaskInfo &l, const DLTaskInfo &r) -> bool {
        return l.identifier() < r.identifier();
    });
    QVariantList ll;
    foreach (DLTaskInfo info, tasks) {
        ll.append(info.toMap());
    }
    setCompletedTasks(ll);
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
            } else {
                qWarning()<<Q_FUNC_INFO<<" skip hash "<<m_nodeHash.value(key).identifier()
                         <<"  as placeholderTaskInfo is empty";
            }
        }
    }
    return ll;
}


#include "BDiskDownloadDelegate.moc"

