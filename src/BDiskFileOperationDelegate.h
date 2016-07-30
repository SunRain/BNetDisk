#ifndef BDISKFILEOPERATIONDELEGATE_H
#define BDISKFILEOPERATIONDELEGATE_H

#include <QObject>

#include "BDiskRequest/BDiskActions.h"

class BDiskFileOperationDelegate : public QObject
{
    Q_OBJECT
public:
    enum OperationType{
        OPERATION_RENAME = 0x0
    };
    Q_ENUM(OperationType)

    explicit BDiskFileOperationDelegate(QObject *parent = 0);
    virtual ~BDiskFileOperationDelegate();

    Q_INVOKABLE void rename(const QString &path, const QString &newName);
private:
    BDiskActionFileRename *m_fileRename;

signals:
    void startRequest(OperationType t);
    void finishRequest(OperationType t);
    void requestFailure(OperationType t);
    void requestSuccess(OperationType t);
};

#endif // BDISKFILEOPERATIONDELEGATE_H
