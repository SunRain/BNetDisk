#ifndef APPLICATIONUTILITY_H
#define APPLICATIONUTILITY_H

#include <QObject>

class ApplicationUtility : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationUtility(QObject *parent = 0);

    Q_INVOKABLE QString fileObjectPathToPath(const QString &path);
    Q_INVOKABLE QString fileObjectPathToFileName(const QString &path);
};

#endif // APPLICATIONUTILITY_H
