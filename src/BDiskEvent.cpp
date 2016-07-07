#include "BDiskEvent.h"

#include <QCoreApplication>
#include <qcoreevent.h>
#include <QDebug>

const static int EVENT_DL_TASKINFO = QEvent::Type(QEvent::User);

class DownloadProgressEvent : public QEvent
{
public:
    explicit DownloadProgressEvent(const QString &hash, const QString &speed, const QString &percent)
        : QEvent((QEvent::Type)EVENT_DL_TASKINFO)
        , m_uuid(hash)
        , m_speed(speed)
        , m_percent(percent)
    {

    }
    virtual ~DownloadProgressEvent() {}
    QString uuid() const
    {
        return m_uuid;
    }
    QString speed() const
    {
        return m_speed;
    }
    QString percent() const
    {
        return m_percent;
    }
private:
    QString m_uuid;
    QString m_speed;
    QString m_percent;
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
    void dispatchDownloadProgress(const QString &hash, const QString &speed, const QString &percent)
    {
        m_locker.lock();
        qApp->postEvent(parent(), new DownloadProgressEvent(hash, speed, percent));
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

void BDiskEvent::dispatchDownloadProgress(const QString &hash, const QString &speed, const QString &percent)
{
    m_dispatch->dispatchDownloadProgress(hash, speed, percent);
}

bool BDiskEvent::event(QEvent *event)
{
    if (event->type() == EVENT_DL_TASKINFO) {
//        qDebug()<<Q_FUNC_INFO<<"========== EVENT_DL_TASKINFO";
        DownloadProgressEvent *e = (DownloadProgressEvent *)event;
        QString hash = e->uuid();
        QString speed = e->speed();
        QString percent = e->percent();
        emit downloadProgress(hash, speed, percent);
        return true;
    }
    return QObject::event(event);
}



#include "BDiskEvent.moc"
