#ifndef BDISKLOGIN_COOKIE_H
#define BDISKLOGIN_COOKIE_H

#include <QObject>

class InnerStateHandler;
class BDiskLoginCookie : public QObject
{
    Q_OBJECT
public:
    explicit BDiskLoginCookie(QObject *parent = 0);
    void login(InnerStateHandler *handler);

private:
    QString truncateYunData(const QString &data);
};

#endif // BDISKLOGIN_COOKIE_H
