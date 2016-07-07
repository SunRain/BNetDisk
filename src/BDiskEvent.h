#ifndef BDISKEVENT_H
#define BDISKEVENT_H

#include <QEvent>
#include <QObject>
#include <QMutex>

#include "SingletonPointer.h"

class EventDispatch;
class BDiskEvent : public QObject
{
    Q_OBJECT
    DECLARE_SINGLETON_POINTER(BDiskEvent)
public:
//    explicit BDiskEvent(QObject *parent = 0);
    virtual ~BDiskEvent();

    void dispatchDownloadProgress(const QString &hash, const QString &speed, const QString &percent);

    // QObject interface
public:
    bool event(QEvent *event);

signals:
    void downloadProgress(const QString &hash, const QString &speed, const QString &percent);

private:
    EventDispatch *m_dispatch;
};

#endif // BDISKEVENT_H
