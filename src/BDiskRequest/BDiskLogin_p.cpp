#include "BDiskLogin_p.h"

#include <QDebug>
#include <QCoreApplication>

InnerEvent::InnerEvent(InnerEvent::EventState ret, const QString &value)
    : QEvent(INNER_EVENT)
    , m_url(QUrl())
{
    m_ret = ret;
    m_data = value;
}

InnerEvent::InnerEvent(InnerEvent::EventState ret, const QUrl &value)
    : QEvent(INNER_EVENT)
    , m_data(QString())
{
    m_ret = ret;
    m_url = value;
}

InnerEvent::EventState InnerEvent::state() const {
    return m_ret;
}

QString InnerEvent::data() const {
    return m_data;
}

QUrl InnerEvent::url() const {
    return m_url;
}

InnerStateHandler::InnerStateHandler(QObject *parent)
    : QObject(parent)
    , m_mutex(QMutex::Recursive)
{

}

void InnerStateHandler::dispatch(InnerEvent::EventState ret, const QString &data) {
    m_mutex.lock();
    qApp->postEvent(parent(), new InnerEvent(ret, data));
    m_mutex.unlock();
}

void InnerStateHandler::dispatch(InnerEvent::EventState ret, const QUrl &data) {
    m_mutex.lock();
    qApp->postEvent(parent(), new InnerEvent(ret, data));
    m_mutex.unlock();
}
