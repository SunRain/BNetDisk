#ifndef BDISKLOGIN_P_H
#define BDISKLOGIN_P_H

#include <QEvent>
#include <QMutex>
#include <QObject>
#include <QUrl>

#define INNER_EVENT (QEvent::Type(QEvent::User))
//#define INNER_CAPTCHA_URL (QEvent::Type(QEvent::User + 1))

class InnerEvent : public QEvent
{
public:
    enum EventState {
        EVENT_LOGIN_ABORT = 0x0,
        EVENT_LOGIN_SUCCESS,
        EVENT_LOGIN_FAILURE,
        EVENT_CAPTCHA_URL,
        EVENT_CAPTCHA_TEXT,
        EVENT_CAPTCHA_URL_NEED_REFRESH,
        EVENT_COOKIE_LOGIN_FAILURE,
        EVENT_COOKIE_LOGIN_SUCCESS,
        EVENT_COOKIE_LOGIN_ABORT
    };

public:
    InnerEvent(InnerEvent::EventState ret, const QString &value);
    InnerEvent(InnerEvent::EventState ret, const QUrl &value);
    virtual ~InnerEvent() {}

    EventState state() const;
    QString data() const;
    QUrl url() const;
private:
    EventState m_ret;
    QString m_data;
    QUrl m_url;
};


class InnerStateHandler : public QObject
{
    Q_OBJECT
public:
    InnerStateHandler(QObject *parent = 0);
    virtual ~InnerStateHandler() {}

    void dispatch(InnerEvent::EventState ret, const QString &data = QString());
    void dispatch(InnerEvent::EventState ret, const QUrl &data);

private:
    QMutex m_mutex;
};

#endif // BDISKLOGIN_P_H
