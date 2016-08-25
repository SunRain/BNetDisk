#ifndef BDISKLOGIN_COOKIE_H
#define BDISKLOGIN_COOKIE_H

#include <QObject>
#include <QThread>

class InnerStateHandler;
class BDiskLoginCookie : public QThread
{
    Q_OBJECT
public:
    explicit BDiskLoginCookie(InnerStateHandler *handler, QObject *parent = 0);

    // QThread interface
protected:
    void run();

private:
    QString truncateYunData(const QString &data);

private:
    InnerStateHandler *m_handler;
};

#endif // BDISKLOGIN_COOKIE_H
