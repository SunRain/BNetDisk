#ifndef BDISKFILEOPERATIONDELEGATE_H
#define BDISKFILEOPERATIONDELEGATE_H

#include <QObject>

#include "BDiskRequest/BDiskActions.h"

class BDiskFileOperationDelegate : public QObject
{
    Q_OBJECT
public:
    enum OperationType{
        OPERATION_RENAME = 0x0,
        OPERATION_DELETE
    };
    Q_ENUM(OperationType)

    explicit BDiskFileOperationDelegate(QObject *parent = 0);
    virtual ~BDiskFileOperationDelegate();

    Q_INVOKABLE void rename(const QString &path, const QString &newName);
    Q_INVOKABLE void deleteFile(const QString &path);
private:
    BDiskActionFileRename *m_fileRename;
    BDiskActionFileDelete *m_fileDelete;

signals:
    void startRequest(OperationType t);
    void finishRequest(OperationType t);
    void requestFailure(OperationType t);
    void requestSuccess(OperationType t);
};

#endif // BDISKFILEOPERATIONDELEGATE_H
