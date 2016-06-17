#ifndef BDISKFILEDIROPERATION_H
#define BDISKFILEDIROPERATION_H

#include <QObject>

#include "BDiskFileObject.h"
class BDiskHttpRequest;
class BDiskFileDirOperation : public QObject
{
    Q_OBJECT

public:
    explicit BDiskFileDirOperation(QObject *parent = 0);
    void listRoot();
    void list(const QString &dir);

signals:
    void fileObjectListChanged(const BDiskFileObjectList &list);

public slots:

private:
    void initiate();
private:
    BDiskHttpRequest *m_request;
};

#endif // BDISKFILEDIROPERATION_H
