#include "BDiskEvent.h"

#include <QCoreApplication>
#include <qcoreevent.h>
#include <QDebug>

const static int EVENT_DL_TASKINFO = QEvent::Type(QEvent::User);
const static int EVENT_TASK_STATUS = QEvent::Type(QEvent::User+1);

class DownloadProgressEvent : public QEvent
{
public:
    explicit DownloadProgressEvent(const QString &hash, int bytesPerSecond, int bytesDownloaded)
        : QEvent((QEvent::Type)EVENT_DL_TASKINFO)
        , m_uuid(hash)
        , m_speed(bytesPerSecond)
        , m_percent(bytesDownloaded)
    {

    }
    virtual ~DownloadProgressEvent() {}
    QString uuid() const
    {
        return m_uuid;
    }
    int bytesPerSecond() const
    {
        return m_speed;
    }
    int bytesDownloaded() const
    {
        return m_percent;
    }
private:
    QString m_uuid;
    int m_speed;
    int m_percent;
};

class TaskStatusEvent : public QEvent
{
public:
    explicit TaskStatusEvent(const QString &hash, BDiskEvent::TaskStatus status)
        : QEvent((QEvent::Type)EVENT_TASK_STATUS)
        , m_hash(hash)
        , m_status(status)
    {
    }
    virtual ~TaskStatusEvent() {}
    QString hash() const
    {
        return m_hash;
    }

    BDiskEvent::TaskStatus status() const
    {
        return m_status;
    }
private:
    QString m_hash;
    BDiskEvent::TaskStatus m_status;
};

class EventDispatch : public QObject
{
    Q_OBJECT
//    DECLARE_SINGLETON_POINTER(EventDispatch)
public:
    explicit EventDispatch(QObject *parent = 0)
        : QObject(parent)
        , m_locker(QMutex::Recursive)
    {
    }
    void dispatchDownloadProgress(const QString &hash, int bytesPerSecond, int bytesDownloaded)
    {
        m_locker.lock();
        qApp->postEvent(parent(), new DownloadProgressEvent(hash, bytesPerSecond, bytesDownloaded));
        m_locker.unlock();
    }
    void dispatchTaskStatus(const QString &hash, BDiskEvent::TaskStatus status) {
        m_locker.lock();
        qApp->postEvent(parent(), new TaskStatusEvent(hash, status));
        m_locker.unlock();
    }

private:
    QMutex m_locker;
};


BDiskEvent::BDiskEvent(QObject *parent)
    : QObject(parent)
    , m_dispatch(new EventDispatch(this))
{

}

BDiskEvent::~BDiskEvent()
{
    if (m_dispatch)
        m_dispatch->deleteLater();
    m_dispatch = nullptr;
}

void BDiskEvent::dispatchDownloadProgress(const QString &hash, int bytesPerSecond, int bytesDownloaded)
{
    m_dispatch->dispatchDownloadProgress(hash, bytesPerSecond, bytesDownloaded);
}

void BDiskEvent::dispatchTaskStatus(const QString &hash, BDiskEvent::TaskStatus status)
{
    m_dispatch->dispatchTaskStatus(hash, status);
}

bool BDiskEvent::event(QEvent *event)
{
    if (event->type() == EVENT_DL_TASKINFO) {
//        qDebug()<<Q_FUNC_INFO<<"========== EVENT_DL_TASKINFO";
        DownloadProgressEvent *e = (DownloadProgressEvent *)event;
        QString hash = e->uuid();
        int speed = e->bytesPerSecond();
        int percent = e->bytesDownloaded();
        emit downloadProgress(hash, speed, percent);
        return true;
    }
    if (event->type() == EVENT_TASK_STATUS) {
        TaskStatusEvent *e = (TaskStatusEvent *)event;
        emit taskStatusChanged(e->hash(), e->status());
        return true;
    }
    return QObject::event(event);
}



#include "BDiskEvent.moc"





