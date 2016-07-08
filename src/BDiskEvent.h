#ifndef BDISKEVENT_H
#define BDISKEVENT_H

#include <QEvent>
#include <QObject>
#include <QMutex>
#include <QJSEngine>
#include <QQmlEngine>

#include "SingletonPointer.h"

class EventDispatch;
class BDiskEvent : public QObject
{
    Q_OBJECT
    Q_ENUMS(TaskStatus)
    DECLARE_SINGLETON_POINTER(BDiskEvent)
public:
    enum TaskStatus {
        STATUS_RUNNING = 0x0,
        STATUS_STOP
    };
    Q_ENUM(TaskStatus)

    static QObject *qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return BDiskEvent::instance();
    }

    virtual ~BDiskEvent();

    void dispatchDownloadProgress(const QString &hash, int bytesPerSecond, int bytesDownloaded);
    void dispatchTaskStatus(const QString &hash, BDiskEvent::TaskStatus status);

    // QObject interface
public:
    bool event(QEvent *event);

signals:
    void downloadProgress(const QString &hash, int bytesPerSecond, double bytesDownloaded);
    void taskStatusChanged(const QString &hash, BDiskEvent::TaskStatus status);

private:
    EventDispatch *m_dispatch;
};

#endif // BDISKEVENT_H
